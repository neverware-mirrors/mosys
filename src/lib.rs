// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate getopts;
#[macro_use]
extern crate lazy_static;

mod bindings;
mod logging;

use std::error::Error;
use std::ffi::{CStr, CString, NulError};
use std::fmt;
use std::io;
use std::ptr::null;
use std::str::Utf8Error;
use std::sync::Mutex;

use bindings::*;
use getopts::Options;
use logging::{Log, LogError};

const PROG_NAME: &'static str = "mosys";
const VERSION: &'static str = env!("CARGO_PKG_VERSION");
const LOCK_TIMEOUT_SECS: i32 = 180;

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
}

#[derive(Debug)]
pub struct Mosys {
    program: String,
    args: Vec<String>,
    style: kv_pair_style,
    single_key: CString,
    platform_override: CString,
}

impl Mosys {
    pub fn new(mut args: Vec<String>) -> Result<Mosys> {
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function only mutates state owned by C components.
            unsafe {
                mosys_globals_init();
            }
        }

        Log::init(&PROG_NAME, Log::Warning)?;
        Ok(Mosys {
            program: args.remove(0),
            args: args,
            style: kv_pair_style_KV_STYLE_VALUE,
            single_key: CString::default(),
            platform_override: CString::default(),
        })
    }

    fn print_usage(&self, opts: Options) {
        let brief = format!("Usage: {} [options] {{commands}}", &self.program);
        print!("{}", opts.usage(&brief));
    }

    pub fn run(&mut self) -> Result<()> {
        let mut opts = Options::new();
        opts.optflag("k", "keyvalue", "print data in key=value format");
        opts.optflag("l", "long", "print data in long format");
        opts.optopt("s", "singlekey", "print value for single key", "[key]");
        opts.optflagmulti("v", "verbose", "verbose (can be used multiple times)");
        opts.optflag(
            "f",
            "force",
            "force (ignore mosys lock, sanity checks, etc)",
        );
        opts.optflag("t", "tree", "display command tree for detected platform");
        opts.optflag("S", "supported", "print supported platform ID's");
        opts.optopt(
            "p",
            "platform",
            "specify platform id (bypass auto-detection)",
            "[id]",
        );
        opts.optflag("h", "help", "print this help");
        opts.optflag("V", "version", "print version");

        let matches = opts.parse(&self.args)?;

        if matches.opt_present("h") {
            self.print_usage(opts);
            return Err(MosysError::Help);
        }

        if matches.opt_present("V") {
            Log::Warning.logln(&format!("{} version {}", &self.program, VERSION))?;
            return Ok(());
        }

        for _ in 0..matches.opt_count("v") {
            Log::inc_threshold();
        }

        if let Some(s) = matches.opt_str("s") {
            self.style = kv_pair_style_KV_STYLE_SINGLE;
            self.single_key = CString::new(s)?;

            // Safe because kv_set_single_key stores a const pointer to this string and this
            // CString will live for the duration of the program.
            unsafe {
                kv_set_single_key(self.single_key.as_ptr());
            }
        } else if matches.opt_present("k") {
            self.style = kv_pair_style_KV_STYLE_PAIR;
        } else if matches.opt_present("l") {
            self.style = kv_pair_style_KV_STYLE_LONG;
        }

        // Safe because the parameter is an enum constant
        unsafe {
            mosys_set_kv_pair_style(self.style);
        }

        if matches.opt_present("S") {
            // Safe because all resources in print_platforms are under C control
            unsafe {
                return match print_platforms() {
                    0 => Ok(()),
                    _ => Err(MosysError::NonzeroPlatformListRet),
                };
            }
        }

        if !matches.opt_present("f") {
            // Safe because argument and return are primitives. Note: this requires root perms
            let rc = unsafe { mosys_acquire_big_lock(LOCK_TIMEOUT_SECS) };

            if rc < 0 {
                Log::Err.logln("Acquiring lock failed")?;
                return Err(MosysError::AcqLockFail);
            }
        }

        let platform_interface = match matches.opt_str("p") {
            Some(p) => {
                self.platform_override = CString::new(p)?;

                // Safe because mosys_platform_setup only examines the string pointer during init
                // and the pointer lives for the duration of the program.
                unsafe { mosys_platform_setup(self.platform_override.as_ptr()) }
            }
            None => {
                // Safe because mosys_platform_setup uses null to skip override
                unsafe { mosys_platform_setup(null()) }
            }
        };

        if platform_interface.is_null() {
            Log::Err.logln("Platform not supported")?;
            return Err(MosysError::PlatformNotSupported);
        }

        // This is valid so convert it to a regular, mutable reference
        let platform_interface = unsafe { &mut *platform_interface };

        // Safe because the strings stored on the platform_intf struct are static
        let platform_name = unsafe { CStr::from_ptr(platform_interface.name).to_str()? };
        Log::Debug.logln(&format!("Platform: {}", platform_name))?;

        if matches.opt_present("t") {
            // Safe because the pointer is guaranteed to be valid at this point
            unsafe { print_tree(platform_interface) };
            return Ok(());
        }

        let _commands = match matches.free.get(0) {
            Some(c) => c,
            None => {
                self.print_usage(opts);
                return Err(MosysError::NoCommands);
            }
        };

        Log::Debug.logln("Completed successfully")?;
        Ok(())
    }
}

impl Drop for Mosys {
    fn drop(&mut self) {
        let mut m = INSTANCES.lock().unwrap();
        *m -= 1;
        Log::drop();
    }
}

#[derive(Debug)]
pub enum MosysError {
    Flag(getopts::Fail),
    Io(io::Error),
    Log(LogError),
    NullString(NulError),
    InvalidUtf8(Utf8Error),
    Help,
    NoCommands,
    NonzeroPlatformListRet,
    AcqLockFail,
    PlatformNotSupported,
}

impl fmt::Display for MosysError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            MosysError::Flag(ref err) => write!(f, "Flag error: {}", err),
            MosysError::Io(ref err) => write!(f, "IO error: {}", err),
            MosysError::Log(ref err) => write!(f, "Logging error: {}", err),
            MosysError::NullString(ref err) => write!(f, "String with null byte passed: {}", err),
            MosysError::InvalidUtf8(ref err) => {
                write!(f, "C string with invalid UTF-8 passed: {}", err)
            }
            MosysError::Help => write!(f, "Requested help"),
            MosysError::NoCommands => write!(f, "No commands given"),
            MosysError::NonzeroPlatformListRet => write!(f, "Platform list failed"),
            MosysError::AcqLockFail => write!(f, "Aquiring global, system lock failed"),
            MosysError::PlatformNotSupported => write!(f, "Platform not supported"),
        }
    }
}

impl Error for MosysError {
    fn description(&self) -> &str {
        match *self {
            MosysError::Flag(ref err) => err.description(),
            MosysError::Io(ref err) => err.description(),
            MosysError::Log(ref err) => err.description(),
            MosysError::NullString(ref err) => err.description(),
            MosysError::InvalidUtf8(ref err) => err.description(),
            MosysError::Help => "Requested help",
            MosysError::NoCommands => "No commands given",
            MosysError::NonzeroPlatformListRet => "Platform list failure",
            MosysError::AcqLockFail => "Aquiring global, system lock failed",
            MosysError::PlatformNotSupported => "Platform not supported",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            MosysError::Flag(ref err) => Some(err),
            MosysError::Io(ref err) => Some(err),
            MosysError::Log(ref err) => Some(err),
            MosysError::NullString(ref err) => Some(err),
            MosysError::InvalidUtf8(ref err) => Some(err),
            _ => None,
        }
    }
}

impl From<getopts::Fail> for MosysError {
    fn from(err: getopts::Fail) -> MosysError {
        MosysError::Flag(err)
    }
}

impl From<io::Error> for MosysError {
    fn from(err: io::Error) -> MosysError {
        MosysError::Io(err)
    }
}

impl From<LogError> for MosysError {
    fn from(err: LogError) -> MosysError {
        MosysError::Log(err)
    }
}

impl From<NulError> for MosysError {
    fn from(err: NulError) -> MosysError {
        MosysError::NullString(err)
    }
}

impl From<Utf8Error> for MosysError {
    fn from(err: Utf8Error) -> MosysError {
        MosysError::InvalidUtf8(err)
    }
}

type Result<T> = std::result::Result<T, MosysError>;

#[cfg(test)]
mod tests {
    macro_rules! vec_of_strings {
        ($($x:expr),*) => (vec![$($x.to_string()),*]);
    }

    use super::*;
    use logging::LAST_LOG;

    // Verbosity and kv_pair_style are global so this lock allows us to run tests at the same time
    // that won't stomp on each other.
    lazy_static! {
        static ref LOCK: Mutex<u8> = Mutex::new(0);
    }

    #[test]
    fn test_new() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-f", "-p", "Link", "command"];
        Mosys::new(args).expect("Instantiation failed");
    }

    #[test]
    fn test_help() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-h", "command"];
        let mut mosys = Mosys::new(args).unwrap();

        match mosys.run() {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }
    }

    /// This test will always fail as expected when not run as root (as tests should never be).
    #[test]
    fn test_lock_fail() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "command"];
        let mut mosys = Mosys::new(args).unwrap();

        match mosys.run() {
            Err(MosysError::AcqLockFail) => (),
            _ => panic!("Should have failed to acquire lock when not running as root"),
        }
    }

    #[test]
    fn test_platform_not_supported() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings![
            "someprogname",
            "-f",
            "-p",
            "nonexistant_platform",
            "command"
        ];
        let mut mosys = Mosys::new(args).unwrap();

        match mosys.run() {
            Err(MosysError::PlatformNotSupported) => (),
            _ => panic!("Should have failed to find a platform"),
        }
    }

    #[test]
    fn test_version() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-V"];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
        assert_eq!(
            &**LAST_LOG.lock().unwrap(),
            &format!("someprogname version {}\n", env!("CARGO_PKG_VERSION"))
        );
    }

    #[test]
    fn test_verbosity() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings![
            "someprogname",
            "-v",
            "-v",
            "-v",
            "-v",
            "-v",
            "-f",
            "-p",
            "Link",
            "command"
        ];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
        let t = Log::get_threshold();
        assert_eq!(t, Log::Spew, "Should have incremented verbosity to max");
    }

    #[test]
    fn test_platform_list() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-S", "-v"];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
    }

    #[test]
    fn test_misc_args() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings![
            "someprogname",
            "-s",
            "keyname",
            "-l",
            "-t",
            "-f",
            "-p",
            "Link",
            "command"
        ];

        let mut mosys = Mosys::new(args).unwrap();
        mosys
            .run()
            .expect("Should have succeeded with getopts arguments");
        let t = Log::get_threshold();
        assert_eq!(t, Log::Warning, "Should not have incremented verbosity");
    }

    #[test]
    fn test_single_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings![
            "someprogname",
            "-f",
            "-p",
            "Link",
            "-s",
            "keyname",
            "command"
        ];

        let mut mosys = Mosys::new(args).unwrap();
        mosys
            .run()
            .expect("Should have succeeded with getopts arguments");

        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r,
            kv_pair_style_KV_STYLE_SINGLE,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_long_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-f", "-p", "Link", "-l", "command"];

        let mut mosys = Mosys::new(args).unwrap();
        mosys
            .run()
            .expect("Should have succeeded with getopts arguments");
        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r,
            kv_pair_style_KV_STYLE_LONG,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_pair_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec_of_strings!["someprogname", "-f", "-p", "Link", "-k", "command"];

        let mut mosys = Mosys::new(args).unwrap();
        mosys
            .run()
            .expect("Should have succeeded with getopts arguments");
        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r,
            kv_pair_style_KV_STYLE_PAIR,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_ffi() {
        // Safe because functions only mutate state owned by C components.
        unsafe {
            mosys_globals_init();
            mosys_set_kv_pair_style(kv_pair_style_KV_STYLE_PAIR);
            let ret = mosys_get_kv_pair_style();
            assert_eq!(ret, kv_pair_style_KV_STYLE_PAIR);;
        }
    }
}

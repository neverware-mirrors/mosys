// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate getopts;
#[macro_use]
extern crate lazy_static;

mod bindings;
mod logging;

use std::error::Error;
use std::ffi::{CString, NulError};
use std::fmt;
use std::io;
use std::ptr::null;
use std::sync::Mutex;

use bindings::*;
use getopts::Options;
use logging::{Log, LogError};

const PROG_NAME: &'static str = "mosys";
const VERSION: &'static str = env!("CARGO_PKG_VERSION");

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
        opts.optflag("h", "help", "print this help\n");
        opts.optflag("V", "version", "print version\n");

        let matches = opts.parse(&self.args)?;

        if matches.opt_present("h") {
            self.print_usage(opts);
            return Err(MosysError::Help);
        }

        if matches.opt_present("V") {
            Log::Warning.log(&format!("{} version {}", &self.program, VERSION))?;
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

        match matches.opt_str("p") {
            Some(p) => {
                self.platform_override = CString::new(p)?;

                // Safe because mosys_platform_setup only examines the string pointer during init
                // and the pointer lives for the duration of the program.
                unsafe {
                    mosys_platform_setup(self.platform_override.as_ptr());
                }
            }
            None => {
                // Safe because mosys_platform_setup uses null to skip override
                unsafe {
                    mosys_platform_setup(null());
                }
            }
        }

        let _commands = match matches.free.get(0) {
            Some(c) => c,
            None => {
                self.print_usage(opts);
                return Err(MosysError::NoCommands);
            }
        };

        Log::Debug.log("Completed successfully\n")?;
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
    Unicode(NulError),
    Help,
    NoCommands,
    NonzeroPlatformListRet,
}

impl fmt::Display for MosysError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            MosysError::Flag(ref err) => write!(f, "Flag error: {}", err),
            MosysError::Io(ref err) => write!(f, "IO error: {}", err),
            MosysError::Log(ref err) => write!(f, "Logging error: {}", err),
            MosysError::Unicode(ref err) => write!(f, "Invalid unicode was passed: {}", err),
            MosysError::Help => write!(f, "Requested help"),
            MosysError::NoCommands => write!(f, "No commands given"),
            MosysError::NonzeroPlatformListRet => write!(f, "Platform list failed"),
        }
    }
}

impl Error for MosysError {
    fn description(&self) -> &str {
        match *self {
            MosysError::Flag(ref err) => err.description(),
            MosysError::Io(ref err) => err.description(),
            MosysError::Log(ref err) => err.description(),
            MosysError::Unicode(ref err) => err.description(),
            MosysError::Help => "Requested help",
            MosysError::NoCommands => "No commands given",
            MosysError::NonzeroPlatformListRet => "Platform list failure",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            MosysError::Flag(ref err) => Some(err),
            MosysError::Io(ref err) => Some(err),
            MosysError::Log(ref err) => Some(err),
            MosysError::Unicode(ref err) => Some(err),
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
        MosysError::Unicode(err)
    }
}

type Result<T> = std::result::Result<T, MosysError>;

#[cfg(test)]
mod tests {
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
        let args = vec!["someprogname".to_string(), "command".to_string()];
        Mosys::new(args).expect("Instantiation failed");
    }

    #[test]
    fn test_help() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec![
            "someprogname".to_string(),
            "-h".to_string(),
            "command".to_string(),
        ];
        let mut mosys = Mosys::new(args).unwrap();

        match mosys.run() {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }
    }

    #[test]
    fn test_version() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec!["someprogname".to_string(), "-V".to_string()];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
        assert_eq!(
            &**LAST_LOG.lock().unwrap(),
            &format!("someprogname version {}", env!("CARGO_PKG_VERSION"))
        );
    }

    #[test]
    fn test_verbosity() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec![
            "someprogname".to_string(),
            "-v".to_string(),
            "-v".to_string(),
            "-v".to_string(),
            "-v".to_string(),
            "-v".to_string(),
            "command".to_string(),
        ];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
        let t = Log::get_threshold();
        assert_eq!(t, Log::Spew, "Should have incremented verbosity to max");
    }

    #[test]
    fn test_platform_list() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec![
            "someprogname".to_string(),
            "-S".to_string(),
            "-v".to_string(),
        ];
        let mut mosys = Mosys::new(args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
    }

    #[test]
    fn test_misc_args() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec![
            "someprogname".to_string(),
            "-s".to_string(),
            "keyname".to_string(),
            "-l".to_string(),
            "-t".to_string(),
            "-f".to_string(),
            "-p".to_string(),
            "platformname".to_string(),
            "command".to_string(),
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
        let args = vec![
            "someprogname".to_string(),
            "-s".to_string(),
            "keyname".to_string(),
            "command".to_string(),
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
        let args = vec![
            "someprogname".to_string(),
            "-l".to_string(),
            "command".to_string(),
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
            kv_pair_style_KV_STYLE_LONG,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_pair_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = vec![
            "someprogname".to_string(),
            "-k".to_string(),
            "command".to_string(),
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

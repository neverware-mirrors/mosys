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
use std::fmt::{Display, Formatter};
use std::io;
use std::ptr::null;
use std::str::Utf8Error;
use std::sync::Mutex;

use bindings::*;
use getopts::Options;
use logging::{Log, LogError};

const PROG_NAME: &'static str = "mosys";

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
}

#[derive(Debug)]
pub struct Mosys<'a> {
    program: &'a str,
    args: Vec<&'a str>,
    style: kv_pair_style,
    single_key: CString,
    platform_override: CString,
}

impl<'a> Mosys<'a> {
    pub fn new(args: &'a [impl AsRef<str>]) -> Result<Mosys<'a>> {
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function only mutates state owned by C components.
            unsafe {
                mosys_globals_init();
            }
        }

        Log::init(&PROG_NAME, Log::Warning)?;
        let mut vec = Vec::new();
        let program = args[0].as_ref();
        vec.extend(args[1..].iter().map(|v| v.as_ref()));

        Ok(Mosys {
            program: program,
            args: vec,
            style: kv_pair_style_KV_STYLE_VALUE,
            single_key: CString::default(),
            platform_override: CString::default(),
        })
    }

    fn print_usage(&self, opts: &Options) {
        let brief = format!("Usage: {} [options] {{commands}}", &self.program);
        print!("{}", opts.usage(&brief));
    }

    pub fn run(&mut self) -> Result<()> {
        let mut opts = Options::new();
        opts.optflag("k", "keyvalue", "print data in key=value format");
        opts.optflag("l", "long", "print data in long format");
        opts.optopt("s", "singlekey", "print value for single key", "[key]");
        opts.optflagmulti("v", "verbose", "verbose (can be used multiple times)");
        opts.optflag("t", "tree", "display command tree for detected platform");
        opts.optflag("S", "supported", "print supported platform ID's");
        opts.optopt(
            "p",
            "platform",
            "specify platform id (bypass auto-detection)",
            "[id]",
        );
        opts.optflag("h", "help", "print this help");

        let matches = opts.parse(&self.args)?;

        if matches.opt_present("h") {
            self.print_usage(&opts);
            return Err(MosysError::Help);
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
                return match print_platforms(platform_intf_list.as_mut_ptr()) {
                    0 => Ok(()),
                    _ => Err(MosysError::NonzeroPlatformListRet),
                };
            }
        }

        let platform_interface = match matches.opt_str("p") {
            Some(p) => {
                self.platform_override = CString::new(p)?;

                // Safe because mosys_platform_setup only examines the string pointer during init
                // and the pointer lives for the duration of the program.
                unsafe {
                    mosys_platform_setup(
                        platform_intf_list.as_mut_ptr(),
                        self.platform_override.as_ptr(),
                    )
                }
            }
            None => {
                // Safe because mosys_platform_setup uses null to skip override
                unsafe { mosys_platform_setup(platform_intf_list.as_mut_ptr(), null()) }
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

        // Safe because we are testing for null before dereferencing
        unsafe {
            if platform_interface.sub.is_null() || (*platform_interface.sub).is_null() {
                return Err(MosysError::NoCommandsDefined);
            }
        }

        let commands = &matches.free[..];
        self.command_dispatch(platform_interface, &commands, &opts)?;

        Log::Debug.logln("Completed successfully")?;
        Ok(())
    }

    fn command_dispatch(
        &self,
        platform_intf: &mut bindings::platform_intf,
        commands: &[String],
        opts: &Options,
    ) -> Result<()> {
        let command = if commands.len() == 0 {
            String::from("help")
        } else {
            commands[0].to_lowercase()
        };

        let mut do_list = false;
        if command == "help" {
            self.print_usage(opts);
            do_list = true;
            println!("  Commands:");
        }

        let mut subcommand_ptr = platform_intf.sub;
        // Safe because subcommand_ptr is checked before dereferencing
        while !subcommand_ptr.is_null() && unsafe { !(*subcommand_ptr).is_null() } {
            // Safe because *subcommand_ptr is checked before dereferencing
            let mut subcommand = unsafe { &mut **subcommand_ptr };

            // Safe because subcommand_ptr isn't null
            subcommand_ptr = unsafe { subcommand_ptr.offset(1) };

            assert!(!subcommand.name.is_null());
            // Safe because sub.name is checked before dereferencing
            let name = unsafe { CStr::from_ptr(subcommand.name).to_str()? };
            if do_list {
                assert!(!subcommand.desc.is_null());
                // Safe because sub.desc is checked before dereferencing
                let desc = unsafe { CStr::from_ptr(subcommand.desc).to_str()? };
                println!("    {0: <12}  {1}", name, desc);
                continue;
            }

            Log::Debug.logln(&format!("Command: {} ({})", name, command))?;

            if name == command {
                assert!(!subcommand.desc.is_null());
                // Safe because sub.desc is checked before dereferencing
                let desc = unsafe { CStr::from_ptr(subcommand.desc).to_str()? };
                Log::Debug.logln(&format!("Found command {} ({})", name, desc))?;
                return self.subcommand_dispatch(platform_intf, &mut subcommand, &commands[1..]);
            }
        }

        if commands.len() == 0 {
            return Err(MosysError::NoCommands);
        }

        if command == "help" {
            return Err(MosysError::Help);
        }

        Log::Warning.logln("Command not found\n")?;
        // trigger a help listing.
        let _res = self.command_dispatch(platform_intf, &[String::from("help")], opts);
        return Err(MosysError::CommandNotSupported);
    }

    fn subcommand_dispatch(
        &self,
        platform_intf: &mut bindings::platform_intf,
        platform_cmd: &mut bindings::platform_cmd,
        commands: &[String],
    ) -> Result<()> {
        match platform_cmd.type_ {
            bindings::arg_type_ARG_TYPE_GETTER | bindings::arg_type_ARG_TYPE_SETTER => {
                // Leaf subcommands should have an associated function
                // Safe because func should always be defined
                let command_func = match unsafe { platform_cmd.arg.func } {
                    Some(x) => x,
                    None => return Err(MosysError::SubcommandFunctionUndefined),
                };

                if commands.len() > 0 && commands[0].to_lowercase() == "help" {
                    // Safe because function only modifies state owned by C components
                    unsafe { bindings::platform_cmd_usage(platform_cmd) };
                    return Err(MosysError::Help);
                }

                // Format commands to C array of C char*
                // CStrings will corrupt if this is done in one step
                let str_args: Vec<_> = commands
                    .iter()
                    .map(|arg| CString::new(arg.as_str()).unwrap())
                    .collect();
                let mut args: Vec<_> = str_args
                    .iter()
                    .map(|arg| arg.as_ptr() as *mut std::os::raw::c_char)
                    .collect();
                args.push(std::ptr::null_mut());
                let argv: *mut *mut std::os::raw::c_char = args.as_mut_ptr();

                // Safe because function only modifies state owned by C components
                let res = unsafe {
                    command_func(platform_intf, platform_cmd, commands.len() as i32, argv)
                };
                if res == 0 {
                    return Ok(());
                } else {
                    // Log so we can catch mosys failures
                    Log::Warning.logln(&format!("mosys invocation was: {:?}", &self.args))?;
                    return Err(MosysError::SubcommandFunctionError(res));
                }
            }
            bindings::arg_type_ARG_TYPE_SUB => {
                match commands.first() {
                    None => {
                        self.list_subcommands(platform_cmd)?;
                        return Err(MosysError::NotEnoughSubcommands);
                    }
                    Some(x) if x == "help" => {
                        self.list_subcommands(platform_cmd)?;
                        return Err(MosysError::Help);
                    }
                    _ => (),
                }

                let command = &commands[0].to_lowercase();

                // Safe because sub union field should exist
                let mut subcommand_ptr = unsafe { platform_cmd.arg.sub };

                // Safe because cmd is checked before dereferencing
                while !subcommand_ptr.is_null() && unsafe { !(*subcommand_ptr).name.is_null() } {
                    // Safe because cmd is checked before dereferencing
                    let mut subcommand = unsafe { &mut *subcommand_ptr };

                    assert!(!subcommand.name.is_null());
                    let name = unsafe { CStr::from_ptr(subcommand.name).to_str()? };
                    if name == command {
                        assert!(!subcommand.desc.is_null());
                        let desc = unsafe { CStr::from_ptr(subcommand.desc).to_str()? };
                        Log::Debug.logln(&format!("Subcommand {} ({})", name, desc))?;
                        return self.subcommand_dispatch(
                            platform_intf,
                            &mut subcommand,
                            &commands[1..],
                        );
                    }

                    // Safe because cmd isn't null
                    subcommand_ptr = unsafe { subcommand_ptr.offset(1) };
                }
            }
            _ => {
                Log::Err.logln("Unknown subcommand type")?;
                return Err(MosysError::CommandNotSupported);
            }
        }
        Log::Warning.logln("Command not found")?;
        self.list_subcommands(platform_cmd)?;
        return Err(MosysError::CommandNotSupported);
    }

    fn list_subcommands(&self, platform_cmd: &bindings::platform_cmd) -> Result<()> {
        println!("  Commands:");

        // Safe because sub union field should exist
        let mut subcommand = unsafe { platform_cmd.arg.sub };

        // Safe because subcommand is checked before derefencing
        while !subcommand.is_null() && unsafe { !(*subcommand).name.is_null() } {
            // subcommand isn't null so save *sub as a mutable refence
            let subcommand_struct = unsafe { &mut *subcommand };

            // Safe because subcommand isn't null
            subcommand = unsafe { subcommand.offset(1) };

            assert!(!subcommand_struct.name.is_null());
            assert!(!subcommand_struct.desc.is_null());
            let name = unsafe { CStr::from_ptr(subcommand_struct.name).to_str()? };
            let desc = unsafe { CStr::from_ptr(subcommand_struct.desc).to_str()? };

            println!("    {0: <12}  {1}", name, desc);
        }
        Ok(())
    }
}

impl<'a> Drop for Mosys<'a> {
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
    NoCommandsDefined,
    NonzeroPlatformListRet,
    AcqLockFail,
    PlatformNotSupported,
    CommandNotSupported,
    NotEnoughSubcommands,
    SubcommandFunctionError(i32),
    SubcommandFunctionUndefined,
}

impl Display for MosysError {
    fn fmt(&self, f: &mut Formatter) -> std::fmt::Result {
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
            MosysError::NoCommandsDefined => write!(f, "No commands defined for platform"),
            MosysError::NonzeroPlatformListRet => write!(f, "Platform list failed"),
            MosysError::AcqLockFail => write!(f, "Aquiring global, system lock failed"),
            MosysError::PlatformNotSupported => write!(f, "Platform not supported"),
            MosysError::CommandNotSupported => write!(f, "Command not supported on this platform"),
            MosysError::NotEnoughSubcommands => write!(f, "Not enough subcommands were given"),
            MosysError::SubcommandFunctionError(ref err) => {
                write!(f, "Subcommand execution finished with error {}", err)
            }
            MosysError::SubcommandFunctionUndefined => {
                write!(f, "Subcommand dosen't have a defined function")
            }
        }
    }
}

impl Error for MosysError {
    // Remove this when https://github.com/rust-lang/rfcs/pull/2230 lands
    fn description(&self) -> &str {
        match *self {
            MosysError::Flag(ref err) => err.description(),
            MosysError::Io(ref err) => err.description(),
            MosysError::Log(ref err) => err.description(),
            MosysError::NullString(ref err) => err.description(),
            MosysError::InvalidUtf8(ref err) => err.description(),
            MosysError::Help => "Requested help",
            MosysError::NoCommands => "No commands given",
            MosysError::NoCommandsDefined => "No commands defined for platform",
            MosysError::NonzeroPlatformListRet => "Platform list failure",
            MosysError::AcqLockFail => "Aquiring global, system lock failed",
            MosysError::PlatformNotSupported => "Platform not supported",
            MosysError::CommandNotSupported => "Command not supported on this platform",
            MosysError::NotEnoughSubcommands => "Not enough subcommands were given",
            MosysError::SubcommandFunctionError(ref _err) => {
                "Subcommand execution finished with an error"
            }
            MosysError::SubcommandFunctionUndefined => "Subcommand doesn't have a defined function",
        }
    }

    fn cause(&self) -> Option<&dyn Error> {
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
        let args = ["someprogname", "-f", "-p", "Link", "command"];
        Mosys::new(&args).expect("Instantiation failed");
    }

    #[test]
    fn test_help() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-h", "command"];
        let mut mosys = Mosys::new(&args).unwrap();

        match mosys.run() {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }
    }

    /// This test will always fail as expected when not run as root (as tests should never be).
    #[test]
    fn test_lock_fail() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "command"];
        let mut mosys = Mosys::new(&args).unwrap();

        match mosys.run() {
            Err(MosysError::AcqLockFail) => (),
            _ => panic!("Should have failed to acquire lock when not running as root"),
        }
    }

    #[test]
    fn test_platform_not_supported() {
        let _test_lock = LOCK.lock().unwrap();
        let args = [
            "someprogname",
            "-f",
            "-p",
            "nonexistant_platform",
            "command",
        ];
        let mut mosys = Mosys::new(&args).unwrap();

        match mosys.run() {
            Err(MosysError::PlatformNotSupported) => (),
            _ => panic!("Should have failed to find a platform"),
        }
    }

    #[test]
    fn test_verbosity() {
        let _test_lock = LOCK.lock().unwrap();
        let args = [
            "someprogname",
            "-v",
            "-v",
            "-v",
            "-v",
            "-v",
            "-f",
            "-p",
            "Link",
            "command",
        ];
        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::CommandNotSupported) => (),
            _ => panic!("Should have succeeded with getopts arguments"),
        };
        let t = Log::get_threshold();
        assert_eq!(t, Log::Spew, "Should have incremented verbosity to max");
    }

    #[test]
    fn test_platform_list() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-S", "-v"];
        let mut mosys = Mosys::new(&args).unwrap();
        mosys.run().expect("Should have exited Ok(())");
    }

    #[test]
    fn test_misc_args() {
        let _test_lock = LOCK.lock().unwrap();
        let args = [
            "someprogname",
            "-s",
            "keyname",
            "-l",
            "-t",
            "-f",
            "-p",
            "Link",
            "command",
        ];

        let mut mosys = Mosys::new(&args).unwrap();
        mosys
            .run()
            .expect("Should have succeeded with getopts arguments");
        let t = Log::get_threshold();
        assert_eq!(t, Log::Warning, "Should not have incremented verbosity");
    }

    #[test]
    fn test_single_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = [
            "someprogname",
            "-f",
            "-p",
            "Link",
            "-s",
            "keyname",
            "command",
        ];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::CommandNotSupported) => (),
            _ => panic!("Should have succeeded with getopts arguments"),
        };

        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r, kv_pair_style_KV_STYLE_SINGLE,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_long_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "-l", "command"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::CommandNotSupported) => (),
            _ => panic!("Should have succeeded with getopts arguments"),
        };
        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r, kv_pair_style_KV_STYLE_LONG,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_pair_kv_pair() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "-k", "command"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::CommandNotSupported) => (),
            _ => panic!("Should have succeeded with getopts arguments"),
        };
        let r;

        unsafe {
            r = mosys_get_kv_pair_style();
        }

        assert_eq!(
            r, kv_pair_style_KV_STYLE_PAIR,
            "Should have change kv_pair_style"
        );
    }

    #[test]
    fn test_no_commands_defined() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Dummy", "-k", "command"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::NoCommandsDefined) => (),
            _ => panic!("Should have returned error that platform has no commands"),
        }
    }

    #[test]
    fn test_ffi() {
        // Safe because functions only mutate state owned by C components.
        unsafe {
            mosys_globals_init();
            mosys_set_kv_pair_style(kv_pair_style_KV_STYLE_PAIR);
            let ret = mosys_get_kv_pair_style();
            assert_eq!(ret, kv_pair_style_KV_STYLE_PAIR);
        }
    }

    #[test]
    fn test_sub_help() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "ec", "help"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }
    }

    #[test]
    fn test_subcommand() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "ec", "info"];

        let mut mosys = Mosys::new(&args).unwrap();
        mosys.run().expect("Complete command should return Ok(())");
    }

    #[test]
    fn test_full_subcommand_help() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "ec", "info", "help"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }
    }

    #[test]
    fn test_no_subcommand() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "ec"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::NotEnoughSubcommands) => (),
            _ => panic!("Should have returned no subcommands error code"),
        }
    }

    #[test]
    fn test_subcommand_not_supported() {
        let _test_lock = LOCK.lock().unwrap();
        let args = ["someprogname", "-f", "-p", "Link", "ec", "fake"];

        let mut mosys = Mosys::new(&args).unwrap();
        match mosys.run() {
            Err(MosysError::CommandNotSupported) => (),
            _ => panic!("Should have returned command not supported error code"),
        }
    }
}

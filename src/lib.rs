// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate getopts;
#[macro_use]
extern crate lazy_static;

mod bindings;
mod logging;

use std::error::Error;
use std::fmt;
use std::io;
use std::sync::Mutex;

use bindings::*;
use getopts::Options;
use logging::{Log, LogError};

const PROG_NAME: &str = "mosys";

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
}

#[derive(Debug)]
pub struct Mosys {}

impl Mosys {
    pub fn new(args: &[String]) -> Result<Mosys> {
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function only mutates state owned by C components.
            unsafe {
                mosys_globals_init();
            }
        }

        Log::init(&PROG_NAME, Log::Debug)?;
        let program = args.get(0).unwrap();
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

        let matches = opts.parse(args.get(1..).unwrap())?;

        if matches.opt_present("h") {
            Mosys::print_usage(&program, opts);
            return Err(MosysError::Help);
        }

        let _single_key = matches.opt_str("s");

        let _commands = match matches.free.get(0) {
            Some(c) => c,
            None => {
                Mosys::print_usage(&program, opts);
                return Err(MosysError::NoCommands);
            }
        };

        Ok(Mosys {})
    }

    fn print_usage(program: &str, opts: Options) {
        let brief = format!("Usage: {} [options] {{commands}}", program);
        print!("{}", opts.usage(&brief));
    }

    pub fn run(&self) -> Result<()> {
        Log::Debug.log("Hello World!\n")?;
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
    Help,
    NoCommands,
}

impl fmt::Display for MosysError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            MosysError::Flag(ref err) => write!(f, "Flag error: {}", err),
            MosysError::Io(ref err) => write!(f, "IO error: {}", err),
            MosysError::Log(ref err) => write!(f, "Logging error: {}", err),
            MosysError::Help => write!(f, "Requested help"),
            MosysError::NoCommands => write!(f, "No commands given"),
        }
    }
}

impl Error for MosysError {
    fn description(&self) -> &str {
        match *self {
            MosysError::Flag(ref err) => err.description(),
            MosysError::Io(ref err) => err.description(),
            MosysError::Log(ref err) => err.description(),
            MosysError::Help => "Requested help",
            MosysError::NoCommands => "No commands given",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            MosysError::Flag(ref err) => Some(err),
            MosysError::Io(ref err) => Some(err),
            MosysError::Log(ref err) => Some(err),
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

type Result<T> = std::result::Result<T, MosysError>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_new() {
        let args = vec!["someprogname".to_string(), "command".to_string()];
        Mosys::new(&args).expect("Instantiation failed");
    }

    #[test]
    fn test_args() {
        let args = vec![
            "someprogname".to_string(),
            "-h".to_string(),
            "command".to_string(),
        ];

        match Mosys::new(&args) {
            Err(MosysError::Help) => (),
            _ => panic!("Should have returned help error code"),
        }

        let args = vec![
            "someprogname".to_string(),
            "-s".to_string(),
            "keyname".to_string(),
            "command".to_string(),
        ];

        Mosys::new(&args).expect("Should have succeeded with getopts arguments");
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

// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#[macro_use]
extern crate lazy_static;

mod bindings;
mod logging;

use std::error::Error;
use std::fmt;
use std::io;
use std::sync::Mutex;

use bindings::*;
use logging::{Log, LogError};

const PROG_NAME: &str = "mosys";

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
}

#[derive(Debug)]
pub struct Mosys {}

impl Mosys {
    pub fn new(_args: &mut Vec<String>) -> Result<Mosys> {
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function only mutates state owned by C components.
            unsafe {
                mosys_globals_init();
            }
        }

        Log::init(&PROG_NAME, Log::Debug)?;
        Ok(Mosys {})
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
    Io(io::Error),
    Log(LogError),
}

impl fmt::Display for MosysError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            MosysError::Io(ref err) => write!(f, "IO error: {}", err),
            MosysError::Log(ref err) => write!(f, "Logging error: {}", err),
        }
    }
}

impl Error for MosysError {
    fn description(&self) -> &str {
        match *self {
            MosysError::Io(ref err) => err.description(),
            MosysError::Log(ref err) => err.description(),
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            MosysError::Io(ref err) => Some(err),
            MosysError::Log(ref err) => Some(err),
        }
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
        let mut args = Vec::new();
        Mosys::new(&mut args).expect("Instantiation failed");
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

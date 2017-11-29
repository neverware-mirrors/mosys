// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#[macro_use]
extern crate lazy_static;

mod bindings;
mod logging;

use std::error::Error;
use std::sync::Mutex;

use bindings::*;
use logging::Log;

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

type Result<T> = std::result::Result<T, Box<Error>>;

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

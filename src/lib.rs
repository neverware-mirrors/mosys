// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::error::Error;
use std::ffi::CStr;
use std::ffi::CString;
use std::ptr;

#[derive(Debug)]
pub struct Mosys {
    pub prog_name: CString,
}

impl Mosys {
    pub fn new(_args: &mut Vec<String>) -> Result<Mosys> {
        Ok(Mosys  {
            prog_name: CString::new("mosys")?,
        })
    }

    pub fn run(self) -> Result<()> {
        let hello_world = CStr::from_bytes_with_nul(b"Hello World!\n\0")?;

        // Safe because functions do not mutate passed pointers and strings are null terminated.
        unsafe {
            mosys_log_init(
                self.prog_name.as_ptr(),
                log_levels_LOG_DEBUG,
                ptr::null_mut(),
            );
            lprintf(log_levels_LOG_DEBUG, hello_world.as_ptr());
        }

        Ok(())
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

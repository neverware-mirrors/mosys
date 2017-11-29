// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use std::ptr;
use std::ffi::CStr;

fn main() {
    let prog_name = CStr::from_bytes_with_nul(b"mosys\0").unwrap();
    let hello_world = CStr::from_bytes_with_nul(b"Hello World!\n\0").unwrap();

    // Safe because functions do not mutate passed pointers and strings are null terminated.
    unsafe {
        mosys_log_init(prog_name.as_ptr(), log_levels_LOG_DEBUG, ptr::null_mut());
        lprintf(log_levels_LOG_DEBUG, hello_world.as_ptr());
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ffi() {
        unsafe {
            mosys_globals_init();
            mosys_set_kv_pair_style(kv_pair_style_KV_STYLE_PAIR);
            let ret = mosys_get_kv_pair_style();
            assert_eq!(ret, kv_pair_style_KV_STYLE_PAIR);;
        }
    }
}

// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use std;
use std::error::Error;
use std::ffi::CString;
use std::ptr;
use std::sync::Mutex;

use bindings::*;

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
}

/// A safe wrapper of the C mosys logging facilities
///
/// The enum members are the various log levels defined in the C enum in mosys/log.h. Should be
/// kept in sync.
#[allow(dead_code)]
pub enum Log {
    Emerg,   // system is unusable
    Alert,   // action must be taken immediately
    Crit,    // critical conditions
    Err,     // error conditions
    Warning, // warning conditions */
    Notice,  // normal but significant condition
    Info,    // informational messages
    Debug,   // debug-level messages
    Spew,    // excessive debug messages
}

impl Log {
    /// Initialize the logging system. Exactly one call to drop() is expected for every call to
    /// this function.
    pub fn init(prog_name: &str, threshold: Log) -> Result {
        let prog_name_cstr = CString::new(prog_name).unwrap().as_ptr();
        let threshold_cenum = Log::threshold_conv(threshold);
        let mut ret: i32 = 0;
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function doesn't mutate passed pointers and strings are null
            // terminated.
            unsafe {
                ret = mosys_log_init(prog_name_cstr, threshold_cenum, ptr::null_mut());
            }
        }

        match ret {
            0 => Ok(()),
            _ => Err(From::from(format!(
                "Logger initialization failed with return code [{}]",
                ret
            ))),
        }
    }

    pub fn log(self, message: &str) -> Result {
        let m = INSTANCES.lock().unwrap();

        if *m < 1 {
            return Err(From::from("Logger isn't ready"));
        }

        let message_cstring = CString::new(message)?;
        let threshold_cenum = Log::threshold_conv(self);

        // Safe because function doesn't mutate passed pointers and strings are null terminated.
        // Underlying impl is printf which is threadsafe in at least glibc.
        unsafe {
            lprintf(threshold_cenum, message_cstring.as_ptr());
        }

        Ok(())
    }

    /// Shut down the logging system. Exactly one call to this function is expected for every call
    /// to init().
    pub fn drop() {
        let mut m = INSTANCES.lock().unwrap();
        *m -= 1;

        if *m == 0 {
            // Safe because function only mutates state owned by C components.
            unsafe {
                mosys_log_halt();
            }
        }
    }

    fn threshold_conv(threshold: Log) -> u32 {
        match threshold {
            Log::Emerg => log_levels_LOG_EMERG,
            Log::Alert => log_levels_LOG_ALERT,
            Log::Crit => log_levels_LOG_CRIT,
            Log::Err => log_levels_LOG_ERR,
            Log::Warning => log_levels_LOG_WARNING,
            Log::Notice => log_levels_LOG_NOTICE,
            Log::Info => log_levels_LOG_INFO,
            Log::Debug => log_levels_LOG_DEBUG,
            Log::Spew => log_levels_LOG_SPEW,
        }
    }
}

type Result = std::result::Result<(), Box<Error>>;

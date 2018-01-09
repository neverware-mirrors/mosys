// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// non_upper_case_globals is for bindings
// dead_code is for conditional #[cfg(test)] usage of LAST_LOG lock
#![allow(non_upper_case_globals, dead_code)]

use std;
use std::error::Error;
use std::ffi::{CString, NulError};
use std::fmt;
use std::ptr;
use std::sync::Mutex;

use bindings::*;

lazy_static! {
    pub static ref INSTANCES: Mutex<u8> = Mutex::new(0);
    #[derive(Debug)]
    pub static ref LAST_LOG: Mutex<String> = Mutex::new(String::new());
}

/// A safe wrapper of the C mosys logging facilities
///
/// The enum members are the various log levels defined in the C enum in mosys/log.h. Should be
/// kept in sync.
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum Log {
    Emerg,   // system is unusable
    Alert,   // action must be taken immediately
    Crit,    // critical conditions
    Err,     // error conditions
    Warning, // warning conditions
    Notice,  // normal but significant condition
    Info,    // informational messages
    Debug,   // debug-level messages
    Spew,    // excessive debug messages
}

impl Log {
    /// Initialize the logging system. Exactly one call to drop() is expected for every call to
    /// this function.
    pub fn init(prog_name: &str, threshold: Log) -> Result {
        let prog_name_cstr = CString::new(prog_name).unwrap();
        let threshold_cenum = Log::renum_to_cenum(threshold);
        let mut ret: i32 = 0;
        let mut m = INSTANCES.lock().unwrap();
        *m += 1;

        if *m == 1 {
            // Safe because function doesn't mutate passed pointers and strings are null
            // terminated.
            unsafe {
                ret = mosys_log_init(prog_name_cstr.as_ptr(), threshold_cenum, ptr::null_mut());
            }
        }

        if ret != 0 {
            return Err(LogError::Init(ret));
        }

        Log::set_threshold(threshold)
    }

    pub fn log(self, message: &str) -> Result {
        let m = INSTANCES.lock().unwrap();

        if *m < 1 {
            return Err(LogError::NotReady);
        }

        let message_cstring = CString::new(message)?;
        let threshold_cenum = Log::renum_to_cenum(self);

        // Safe because function doesn't mutate passed pointers and strings are null terminated.
        // Underlying impl is printf which is threadsafe in at least glibc.
        unsafe {
            lprintf(threshold_cenum, message_cstring.as_ptr());
        }

        #[cfg(test)]
        {
            *LAST_LOG.lock().unwrap() = message.to_string();
        }

        Ok(())
    }

    pub fn logln(self, message: &str) -> Result {
        self.log(&format!("{}\n", message))
    }

    /// Get the current threshold for the logging system.
    pub fn get_threshold() -> Log {
        // Safe because log levels are always >0
        unsafe {
            let threshold = log_threshold_get() as u32;
            return Log::cenum_to_renum(threshold);
        }
    }

    /// Set the current threshold for the logging system. Returns LogError::InvalidThreshold
    /// if the provided threshold is unknown to the logging subsystem.
    pub fn set_threshold(threshold: Log) -> Result {
        let t = Log::renum_to_cenum(threshold);

        // Safe because all input states are handled by renum_to_cenum
        unsafe {
            match log_threshold_set(t) {
                0 => Ok(()),
                _ => Err(LogError::InvalidThreshold),
            }
        }
    }

    pub fn inc_threshold() {
        let mut t = Log::get_threshold() as u32;

        if t < Log::Spew as u32 {
            t += 1;
            Log::set_threshold(Log::from(t))
                .expect("Unexpected return set_threshold; are bindings out of sync?");
        }
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

    fn renum_to_cenum(threshold: Log) -> u32 {
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

    fn cenum_to_renum(threshold: u32) -> Log {
        match threshold {
            log_levels_LOG_EMERG => Log::Emerg,
            log_levels_LOG_ALERT => Log::Alert,
            log_levels_LOG_CRIT => Log::Crit,
            log_levels_LOG_ERR => Log::Err,
            log_levels_LOG_WARNING => Log::Warning,
            log_levels_LOG_NOTICE => Log::Notice,
            log_levels_LOG_INFO => Log::Info,
            log_levels_LOG_DEBUG => Log::Debug,
            log_levels_LOG_SPEW => Log::Spew,
            _ => Log::Spew,
        }
    }
}

impl From<u32> for Log {
    fn from(n: u32) -> Log {
        match n {
            0 => Log::Emerg,
            1 => Log::Alert,
            2 => Log::Crit,
            3 => Log::Err,
            4 => Log::Warning,
            5 => Log::Notice,
            6 => Log::Info,
            7 => Log::Debug,
            8 => Log::Spew,
            _ => Log::Spew,
        }
    }
}

#[derive(Debug)]
pub enum LogError {
    Init(i32),
    NotReady,
    NullTerm(NulError),
    InvalidThreshold,
}

impl fmt::Display for LogError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            LogError::Init(code) => write!(f, "Logger initialization error code: {}", code),
            LogError::NotReady => write!(f, "Logger not ready"),
            LogError::NullTerm(ref err) => write!(f, "String null termination error: {}", err),
            LogError::InvalidThreshold => write!(f, "Set invalid threshold"),
        }
    }
}

impl Error for LogError {
    fn description(&self) -> &str {
        match *self {
            LogError::Init(_) => "Initialization",
            LogError::NotReady => "Not ready",
            LogError::NullTerm(ref err) => err.description(),
            LogError::InvalidThreshold => "Invalid threshold",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            LogError::NullTerm(ref err) => Some(err),
            _ => None,
        }
    }
}

impl From<NulError> for LogError {
    fn from(err: NulError) -> LogError {
        LogError::NullTerm(err)
    }
}

type Result = std::result::Result<(), LogError>;

#[cfg(test)]
mod tests {
    use super::*;

    fn setup() {
        Log::init("progname", Log::Debug).expect("Should have succeeded");
    }

    fn teardown() {
        Log::drop();
    }

    #[test]
    fn test_init() {
        setup();
        teardown();
    }

    #[test]
    fn test_double_init() {
        setup();
        Log::init("progname", Log::Warning).expect("Should have succeeded on second init");
        let t = Log::get_threshold();
        assert_eq!(t, Log::Warning, "Should have modified first init threshold");
        Log::drop();
        teardown();
    }

    #[test]
    fn test_inc_threshold() {
        setup();
        Log::inc_threshold();
        let t = Log::get_threshold();
        assert_eq!(t, Log::Spew);
        teardown();
    }
}

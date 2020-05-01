// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#![no_main]

extern crate io_jail;

use std::os::raw::{c_char, c_int};
use std::path::Path;

use io_jail::Minijail;

extern "C" {
    fn mosys_main(argc: c_int, argv: *mut *mut c_char) -> c_int;
}

#[no_mangle]
extern "C" fn main(argc: c_int, argv: *mut *mut c_char) -> c_int {
    let mut j = Minijail::new().unwrap();

    // For unknown reasons, this code slows boot on arm machines.
    // See crbug.com/872187 for more information.
    #[cfg(target_arch = "x86_64")]
    {
        // needs CAP_SYS_RAWIO and CAP_SYS_ADMIN
        j.use_caps(0x220000);
        j.set_ambient_caps();

        // Don't set securebits because this may be inside another minijail.
        // See b/112030238.
        j.skip_setting_securebits(0xff);

        j.remount_proc_readonly();

        j.namespace_net();
    }

    j.no_new_privs();

    j.set_seccomp_filter_tsync();

    let policy_path = Path::new("/usr/share/policy/mosys-seccomp.policy");
    if let Err(err) = j.parse_seccomp_filters(policy_path) {
        eprintln!("Minijail failed to parse seccomp policy. Error: {}", err);
        eprintln!("This error is expected in initramfs environments.");
    } else {
        j.use_seccomp_filter();
    }

    j.run_as_init();

    // Jail will be destroyed when it is dropped.
    j.enter();

    unsafe {
        return mosys_main(argc, argv);
    }
}

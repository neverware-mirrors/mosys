// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate mosys;
extern crate io_jail;

use std::env;
use std::path::Path;
use std::process;

use mosys::Mosys;
use io_jail::Minijail;

fn main() {
    let args: Vec<String> = env::args().collect();

    let mut mosys = Mosys::new(&args).unwrap_or_else(|err| {
        eprintln!("Problem creating program: {}", err);
        process::exit(1);
    });

    let mut j = Minijail::new().unwrap();
    // needs CAP_SYS_RAWIO and CAP_SYS_ADMIN
    j.use_caps(0x220000);
    j.set_ambient_caps();

    // Don't set securebits because this may be inside another minijail.
    // See b/112030238.
    j.skip_setting_securebits(0xff);

    j.remount_proc_readonly();

    j.namespace_net();
    j.no_new_privs();

    j.set_seccomp_filter_tsync();
    j.log_seccomp_filter_failures();

    let policy_path = Path::new("/usr/share/policy/mosys-seccomp.policy");
    if let Err(err) = j.parse_seccomp_filters(policy_path) {
        eprintln!("Minijail failed to parse seccomp policy. Error: {}", err);
        eprintln!("This error is expected in initramfs environments.");
    } else {
        j.use_seccomp_filter();
    }

    j.run_as_init();

    // Jail will be destoryed when it is dropped
    j.enter();

    if let Err(err) = mosys.run() {
        eprintln!("Application error: {}", err);
        process::exit(1);
    }
}

// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate mosys;

use std::env;
use std::process;

use mosys::Mosys;

fn main() {
    let args: Vec<String> = env::args().collect();

    let mut mosys = Mosys::new(&args).unwrap_or_else(|err| {
        eprintln!("Problem creating program: {}", err);
        process::exit(1);
    });

    if let Err(err) = mosys.run() {
        eprintln!("Application error: {}", err);
        process::exit(1);
    }
}

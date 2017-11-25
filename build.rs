// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use std::env;
use std::process::Command;

fn main() {
    Command::new("make").args(&["libmosys.a"]).status().unwrap();
    let out_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    println!("cargo:rustc-link-search=native={}", out_dir);
    println!("cargo:rustc-link-lib=static=mosys");
}

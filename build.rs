// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use std::env;
use std::path::Path;

fn main() {
    let meson_build_root = env::var("MESON_BUILD_ROOT").unwrap();
    let meson_build_root = Path::new(&meson_build_root);
    assert!(
        meson_build_root.join("libmosys.a").exists(),
        "libmosys.a is not present, see the README for instructions on building Mosys"
    );
    println!(
        "cargo:rustc-link-search=native={}",
        meson_build_root.display()
    );
    println!("cargo:rustc-link-lib=dylib=mosys");
    println!("cargo:rustc-link-lib=dylib=uuid");
    println!("cargo:rustc-link-lib=dylib=fmap");
}

// Copyright 2017 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

extern crate bindgen;

use std::env;
use std::fs::{self, File};
use std::io::{self, Write};
use std::path::{Path, PathBuf};
use std::process::Command;

fn main() {
    let status = Command::new("make")
        .args(&["libmosys.a"])
        .status()
        .expect("Failed to execute make");
    assert!(status.success(), "make exited with a non-zero status");
    let root_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let root_dir = PathBuf::from(&root_dir);
    println!("cargo:rustc-link-search=native={}", root_dir.display());
    println!("cargo:rustc-link-lib=static=mosys");
    println!("cargo:rustc-link-lib=dylib=uuid");
    println!("cargo:rustc-link-lib=dylib=fmap");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let prune = vec![
        root_dir.join(".git"),
        root_dir.join("include/config"),
        root_dir.join("include/generated"),
        root_dir.join("scripts"),
        root_dir.join("src"),
        root_dir.join("target"),
    ];
    let wrapper = out_path.join("wrapper.h");
    generate_wrapper(root_dir.as_path(), &wrapper, &prune)
        .expect("Failed to generate wrapper.h");
    let bindings = bindgen::Builder::default()
        .header(wrapper.to_str().unwrap())
        .clang_arg("-Iinclude")
        .whitelist_function("mosys_.*")
        .whitelist_function("lprintf")
        .whitelist_function("lperror")
        .whitelist_function("log_.*")
        .generate()
        .expect("Unable to generate bindings");
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}

/// Generates a Bindgen wrapper at target (e.g. target/debug/wrapper.h) using root dir to search
/// for C headers to include. Takes slice of paths based at root to prune from search.
fn generate_wrapper(root: &Path, target: &Path, prune: &[PathBuf]) -> io::Result<()> {
    let mut found_headers = Vec::new();
    find_headers(root, prune, &mut found_headers).expect("Failed to build headers");
    let mut file = File::create(target)?;
    write!(&mut file, "// This file autogenerated from build.rs\n")?;

    for header in found_headers {
        write!(&mut file, "#include \"{}\"\n", header.display())?;
    }

    Ok(())
}

fn find_headers(dir: &Path, prune: &[PathBuf], found: &mut Vec<PathBuf>) -> io::Result<()> {
    if dir.is_dir() {
        'outer: for entry in fs::read_dir(dir)? {
            let path = entry?.path();

            for prune_path in prune {
                if path.starts_with(prune_path.as_path()) {
                    continue 'outer;
                }
            }

            if path.is_dir() {
                find_headers(&path, prune, found)?;
            } else if path.to_str().unwrap().ends_with(".h") {
                found.push(path);
            }
        }
    }
    Ok(())
}

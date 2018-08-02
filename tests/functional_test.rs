extern crate mosys;

use std::process::Command;

#[test]
fn dummy_ec() {
    let args = ["-f", "--no-jail", "-p", "Dummy", "ec", "info"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy ec info failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("name"));
    assert!(String::from_utf8_lossy(&output.stdout).contains("vendor"));
    assert!(String::from_utf8_lossy(&output.stdout).contains("roversion"));
}

#[test]
fn dummy_eeprom() {
    let args = ["-f", "--no-jail", "-p", "Dummy", "eeprom", "list"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy eeprom list failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("host_firmware"));
    assert!(String::from_utf8_lossy(&output.stdout).contains("8192"));
    assert!(String::from_utf8_lossy(&output.stdout).contains("bytes"));
    assert!(String::from_utf8_lossy(&output.stdout).contains("read,write"));
}

#[test]
fn cros_config() {
    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "model"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform model failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("dummy"));

    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "name"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform name failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("Dummy"));

    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "chassis"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform chassis failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("DUMMY"));

    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "brand"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform brand failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("DUMM"));

    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "customization"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform customization failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("dummy"));

    let args = ["-f", "--no-jail", "-p", "Dummy", "platform", "signature"];
    let output = Command::new("mosys")
        .args(&args)
        .env("PATH","target/debug")
        .output()
        .expect("dummy platform signature failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("dummy"));
}

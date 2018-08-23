#!/bin/bash

# Find lines with si_code=SYS_SECCOMP, implying a seccomp filter failure.
# Parse the line to find the si_syscall paratemeter than extract the
# system call itself.
strace -ff "$@" 2>&1 | grep si_code=SYS_SECCOMP | awk -F ',' '{print $4}' \
  | cut -c18-

# write_n

Simple kernel module to facilitate testing partial read and write handling in userspace programs

## How to use

Run:

    make
    sudo insmod write_n.ko

This should create the file `/dev/write_n`, which will only allow reading and writing up to 6 bytes
before returning ENOSPC. The maximum size of the file can be configured by the max_length parameter.

Run `sudo rmmod write_n.ko` to remove it.

## Why isn't this a gist

:shrug:

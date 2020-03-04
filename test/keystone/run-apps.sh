#!/bin/bash
expect -c '
    set timeout 60
    set env(HOST_PORT) 3396
    spawn "./run-qemu.sh"
    expect "*?ogin" { send "root\r" }
    expect "*?assword" { send "sifive\r" }

    expect "# " { send "insmod keystone-driver.ko\r" }
    expect "# " { send "./App.client  Enclave.eapp_riscv eyrie-rt\r" }
    expect "# " { send "poweroff\r" }
    expect eof
    '

#!/bin/bash

expect -c '
    set timeout 60
    send_user \"$env(PORT)\n\"
    spawn "./run-qemu.sh"
    expect "*?ogin" { send "root\r" }
    expect "*?assword" { send "sifive\r" }

    expect "# " { send "insmod keystone-driver.ko\r" }
    expect "# " { send "./App.client  Enclave.eapp_riscv eyrie-rt\r" }
    expect "# " { send "ls -ls .\r" }
    expect "# " { send "./analyzer shared_mem Enclave.nm\r" }
    expect "# " { send "poweroff\r" }
    expect eof
    '

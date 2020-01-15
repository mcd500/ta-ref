#!/bin/bash

expect -c '
    set timeout 180
    cd $::env(KEYSTONE_DIR)
    set env(HOST_PORT) 3396
    spawn "./scripts/run-qemu.sh"
    expect "*?ogin" { send "root\r" }
    expect "*?assword" { send "sifive\r" }

    expect "# " { send "insmod keystone-driver.ko\r" }

    expect "# " { send "cd edger-sample\r" }
    expect "# " { send "./App.client  Enclave.eapp_riscv eyrie-rt\r" }
    expect "# " { send "ls -ls .\r" }
    expect "# " { send "xxd -l12000 shared_mem\r" }
    expect eof
    '

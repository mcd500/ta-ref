#!/bin/bash

if [ $# -eq 1 ] && [ $1 = "perf" ]; then
    
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
    expect "# " { send "echo @enclave\r" }
    expect "# " { send "./analyzer shared_mem Enclave.nm\r" }
    expect "# " { send "echo @host\r" }
    expect "# " { send "./analyzer host_shared_mem App.nm\r" }
    expect "# " { send "poweroff\r" }
    expect eof
    '
else
    
expect -c '
    set timeout 60
    cd $::env(KEYSTONE_DIR)
    set env(HOST_PORT) 3396
    spawn "./scripts/run-qemu.sh"
    expect "*?ogin" { send "root\r" }
    expect "*?assword" { send "sifive\r" }

    expect "# " { send "insmod keystone-driver.ko\r" }

    expect "# " { send "cd edger-sample\r" }
    expect "# " { send "./App.client  Enclave.eapp_riscv eyrie-rt\r" }

    expect "# " { send "poweroff\r" }
    expect eof
    '
fi

#!/bin/bash

screen -L -dmS qemu /bin/bash -c "PORT=${PORT} ./run-qemu.sh"
sleep 5

expect -c "
    set timeout 60
    spawn ssh root@localhost -p $::env(PORT)
    expect \"*(yes/no)? \" { send \"yes\r\" }
    expect \"*?assword\" { send \"sifive\r\" }
    expect \"\# \" { send \"insmod keystone-driver.ko\r\" }
    expect \"\# \" { send \"cd out\r\" }
    expect \"\# \" { send \"./App.client  Enclave.eapp_riscv eyrie-rt\r\" }
    expect \"main end\" {} timeout { exit 1 }
    expect \"\# \" { send \"ls -ls .\r\" }
    expect \"\# \" { send \"./analyzer shared_mem Enclave.nm\r\" }
    expect \"\# \" { send \"poweroff\r\" }
    expect eof
"

exit

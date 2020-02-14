#!/bin/bash

scp -r ./output gitlab@${RPI3_IP_ADDR}:/home/gitlab
expect -c "
    set timeout 30
    spawn ssh gitlab@${RPI3_IP_ADDR}:/home/gitlab/output
    expect \"*$ \" { send \"sudo cp -r lib/optee_armtz/* /lib/optee_armtz/\r\" }
    expect \"*$ \" { send \"sudo usr/bin/optee_ref_ta\r\" }
    expect \"*$ \"  { send \"./root/analyzer ./shared_mem ./root/enclave_nm\r\" }
    expect \"* Invoking all ref tests in Enclave\"
    expect eof
"

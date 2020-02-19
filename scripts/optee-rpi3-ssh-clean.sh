#!/bin/bash

expect -c "
    set timeout 30
    spawn ssh gitlab@${RPI3_IP_ADDR}
    expect \"*$ \" { send \"sudo rm -rf ./output\r\" }
    expect eof
"

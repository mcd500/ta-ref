#!/bin/bash

xterm -e ./run-record.sh&
sleep 2

expect -c '
    set timeout 50

    spawn ./run-qemu.sh

    expect "*?ogin" { send "root\r" }

    expect "# " { send "optee_ref_ta\r" }

    expect "# " { send "poweroff\r" }

    expect "*?RROR:   QEMU System Off: operation not handled." { send "\001c" }
    '

./cut.py

exit

#!/bin/bash

./run-record.sh&
sleep 2

if [ $# -eq 1 ] && [ $1 = "perf" ]; then

expect -c '
set timeout 50

spawn ./run-qemu.sh

expect "*?ogin" { send "root\r" }

expect "# " { send "optee_ref_ta\r" }
expect "# " { send "ls -ls\r" }
expect "# " { send "./analyzer shared_mem enclave_nm\r" }
expect "# " { send "poweroff\r" }

expect "*?RROR:   QEMU System Off: operation not handled." { send "\001c" }
'


else
expect -c '
set timeout 50

spawn ./run-qemu.sh

expect "*?ogin" { send "root\r" }

expect "# " { send "optee_ref_ta\r" }
expect "# " { send "poweroff\r" }

expect "*?RROR:   QEMU System Off: operation not handled." { send "\001c" }
'

fi

./cut.py

exit

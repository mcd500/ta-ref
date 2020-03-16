#!/bin/bash

expect -c "
set timeout 50

send_user \"$env(PORT)\n\"
send_user \"$env(LOG_FILE)\n\"
spawn ./run-qemu.sh

expect \"*?ogin\" { send \"root\r\" }
expect \"# \" { send \"./optee_ref_ta\r\" }
expect \"start TEEC_InvokeCommand\"
expect \"TEEC_InvokeCommand succeeds!\"
expect \"# \" { send \"poweroff\r\" }

expect \"*?RROR:   QEMU System Off: operation not handled.\" { send \"\001c\" }
"

exit

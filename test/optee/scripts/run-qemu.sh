#!/bin/bash

expect -c "
set timeout 50

send_user \"$env(PORT)\n\"
send_user \"$env(LOG_FILE)\n\"
spawn ./scripts/launch-qemu.sh

expect \"*?ogin\" { send \"root\r\" }
expect \"# \" { send \"./optee_ref_ta\r\" }
expect \"*start*\"
expect \"*succeed*\"
expect \"# \" { send \"ls -lsa\r\" }
expect \"# \" { send \"test -f ./analyzer && ./analyzer shared_mem Enclave.nm\r\" }
expect \"# \" { send \"poweroff\r\" }

expect \"*?RROR:   QEMU System Off: operation not handled.\" { send \"\001c\" }
"

exit

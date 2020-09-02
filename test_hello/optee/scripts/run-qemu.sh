#!/bin/bash

expect -c "
set timeout 50

send_user \"$env(LOG_FILE)\n\"
spawn ./scripts/launch-qemu.sh

expect \"*?ogin\" { send \"root\r\" }
expect \"# \" { send \"cd /home/gitlab/out\r\" }
expect \"# \" { send \"./optee_ref_ta\r\" }
expect \"# \" { send \"poweroff\r\" }

expect \"*?RROR:   QEMU System Off: operation not handled.\" { send \"\001c\" }
"

exit

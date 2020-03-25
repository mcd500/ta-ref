#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 5

spawn ssh ${USER}@${IP_ADDR}

expect \"*(yes/no)? \" { send \"yes\r\" }
expect \"*password: \" { send \"gitlab\r\" }
expect \"*$ \" { send \"dirname $\(readlink /lib/optee_armtz/${UUID_TA}\)\r\" }
expect \"/home/gitlab/out\r\" {} timeout { exit 1 }
exit
"

exit

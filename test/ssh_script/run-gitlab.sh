#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 20

spawn ssh ${USER}@${IP_ADDR} /bin/bash -c '\cd /home/gitlab/out && ${COMMAND}\'

expect \"*(yes/no)? \" { send \"yes\r\" }
expect \"*password: \" { send \"gitlab\r\" }
expect eof
"

exit

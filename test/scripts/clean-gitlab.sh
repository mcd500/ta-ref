#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 60

spawn ssh -p ${PORT} ${USER}@${IP_ADDR} /bin/bash -c \'rm -rf /home/gitlab/out && mkdir -p /home/gitlab/out\'

expect \"?\" { send \"yes\r\" }
expect \"*password: \" { send \"${PASSWD}\r\" }
expect eof
"

exit

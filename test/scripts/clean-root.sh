#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 10

spawn ssh ${USER}@${IP_ADDR} /bin/bash -c \'rm -rf /home/gitlab/out && mkdir -p /home/gitlab/out\'

expect \"*(yes/no)? \" { send \"yes\r\" }
expect \"*password: \" { send \"sifive\r\" }
expect eof
"

exit

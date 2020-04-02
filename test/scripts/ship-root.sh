#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 20

spawn scp ${FILES} ${USER}@${IP_ADDR}:/home/gitlab/out

expect \"*(yes/no)? \" { send \"yes\r\" }
expect \"*password: \" { send \"sifive\r\" }
expect eof
"

exit

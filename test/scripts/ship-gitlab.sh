#!/bin/bash

rm -f ~/.ssh/known_hosts
expect -c "
set timeout 60

spawn scp -P ${PORT} ${FILES} ${USER}@${IP_ADDR}:/home/gitlab/out

expect \"?\" { send \"yes\r\" }
expect \"*password: \" { send \"${PASSWD}\r\" }
expect eof
"

exit

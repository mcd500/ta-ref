#!/bin/bash

rm -f ~/.ssh/known_hosts
sshpass -p ${PASSWD} scp -P ${PORT} -o "StrictHostKeyChecking no" ${USER}@${IP_ADDR} /bin/bash -c 'rm -rf /home/gitlab/out && mkdir -p /home/gitlab/out'

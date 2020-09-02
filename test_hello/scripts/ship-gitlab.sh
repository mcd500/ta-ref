#!/bin/bash

rm -f ~/.ssh/known_hosts
sshpass -p ${PASSWD} scp -P ${PORT} -o "StrictHostKeyChecking no" ${FILES} ${USER}@${IP_ADDR}:/home/gitlab/out

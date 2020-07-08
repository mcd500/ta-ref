#!/bin/bash

rm -f ~/.ssh/known_hosts
sshpass -p ${PASSWD} ssh -p ${PORT} -o "StrictHostKeyChecking no" ${USER}@${IP_ADDR} "rm -rf /home/gitlab/out && mkdir -p /home/gitlab/out"

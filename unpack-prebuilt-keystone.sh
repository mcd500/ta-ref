#/bin/bash -x

if ! [ -d build-keystone ]; then
  if ! [ -f keystone-prebuilt.tar.gz ]; then
    wget http://${GITLAB_HOST}/vc707/tee-ta-reference/raw/master/keystone-prebuilt.tar.gz
  fi
  tar -xzf keystone-prebuilt.tar.gz
  echo "Unpacked keystone"
fi

#/bin/bash -x

if ! [ -d build-optee ]; then
  if ! [ -f optee-prebuilt.tar.gz ]; then
    wget http://${GITLAB_HOST}:2000/optee-prebuilt.tar.gz
  fi
  tar -xzf optee-prebuilt.tar.gz
  echo "Unpacked keystone"
fi

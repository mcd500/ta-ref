# Documentation file

   [Draft TEE Internal API doc](./tee-internal-doc-draft.pdf)

# Prerequisites

```sh
sudo apt update
sudo apt upgrade
```

## For doc
```sh
sudo apt install texlive-full doxygen graphviz

```

## For keystone
```sh
sudo apt install autoconf automake autotools-dev bc bison build-essential curl expat libexpat1-dev flex gawk gcc git gperf libgmp-dev libmpc-dev libmpfr-dev libtool texinfo tmux patchutils zlib1g-dev wget bzip2 patch vim-common lbzip2 python pkg-config libglib2.0-dev libpixman-1-dev device-tree-compiler expect
```

## For keyedge
```sh
sudo apt install clang-tools-6.0 libclang-6.0-dev cmake
```

## For SGX
[Install SGX SDK and Linux driver](http://192.168.100.100/vc707/docs/blob/master/intel-sgx-remote-attestation-sample.md)

Don't for get adding PATH to `~/.profile`
```sh
export PATH=/opt/intel/sgxsdk/bin/:${PATH}
```


## For build tee-reference
```sh
sudo apt install makeself screen
```

# Clone sources
```sh
git clone http://192.168.100.100/vc707/tee-ta-reference.git
cd tee-ta-reference
git submodule update --init --recursive
```

# Prepare keystone
```sh
git clone http://192.168.100.100/vc707/keystone.git build-keystone
cd build-keystone
git checkout dev-gp-tee
git submodule update --init --recursive
./aist-setup.sh
. source.sh
make -j `nproc` run-tests
cd ..
```

# Build

```sh
. env-keystone.sh
make
```

# Generate PDF doc

```sh
$ make doc
```

# Documentation file

   [Draft TEE Internal API doc](./tee-internal-doc-draft.pdf)

# Prerequisites

```sh
sudo apt update
sudo apt upgrade
```

For doc
```sh
sudo apt install texlive-full doxygen graphviz

```

For keystone
```sh
sudo apt install autoconf automake autotools-dev bc bison build-essential curl expat libexpat1-dev flex gawk gcc git gperf libgmp-dev libmpc-dev libmpfr-dev libtool texinfo tmux patchutils zlib1g-dev wget bzip2 patch vim-common lbzip2 python pkg-config libglib2.0-dev libpixman-1-dev device-tree-compiler expect
```

For build tee-reference
```sh
sudo apt install makeself screen
```

# Generate PDF doc

```sh
$ make doc
```

# Build

```sh
$ make
```

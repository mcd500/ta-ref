# ref-ta

Improvement of http://192.168.100.100/vc707/tee-ta-reference.

Note) We recommend that we use docker images because we export multiple environment variables at the very beginning of build phase..

|TEE\|docker images|
|---|---|---|
|keystone|vc707/test:ta_ref_keystone_devel|
|intel_sgx|(not yet)|
|optee|(not yet)|

For more information, see [GitLab CI job configuration file](./gitlab-ci.yml).

## keystone

```sh
git clone --recursive http://192.168.100.100/vc707/ta-ref.git
cd ta-ref
docker run -it --rm -v $(pwd):/home/keystone/ta-ref vc707/test:ta_ref_keystone_devel
```

For build && test:

```sh
source keystone.sh
make build
make test
```

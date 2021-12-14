# Overview of ta-ref

@image html images/ta-ref_overview_flow.png
@image latex images/ta-ref_overview_flow.png width=\textwidth

## Features

### What we did on RISC-V
- We designed the GP internal API library to be portable.
  - Keystone SDK is utilized because of runtime "Eyrie".
  - The library is ported to Intel SGX as well as RISC-V Keystone.

- Implementation Challenge
  - The combination of GP internal API and cipher suite is big.
    - We pick up some important GP internal APIs.
  - Some APIs depend on CPU architecture.
    - We separate APIs into CPU architecture dependent / independent.
  - Integrate GP TEE Internal API to Keystone SDK.
    - Keystone SDK includes EDL (Enclave Definition Language) named “keedger”.
    - Keedger creates the code for OCALL (request from TEE to REE) to check the pointer and boundary.

###Separate GP TEE Internal API

- CPU architecture dependent
  - Random Generator, Time, Secure Storage, Transient Object(TEE_GenerateKey)

- CPU architecture independent(Crypto)
  - Transient Object(exclude TEE_GenerateKey), Crypto Common, Authenticated Encryption, Symmetric/Asymmetric Cipher, Message Digest

@image html images/separate-gp-tee-internal-api.png
@image latex images/separate-gp-tee-internal-api.png width=\textwidth

## Diagram

### Dependency of category

@image html images/ta-ref-overview.png
@image latex images/ta-ref-overview.png width=\textwidth


### TEEP Architechture

@image html images/TEEP-Architechture.png
@image latex images/TEEP-Architechture.png width=\textwidth

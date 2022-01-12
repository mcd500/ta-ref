# Overview of TA-Ref

## Features of TA-Ref
  - Provides Portable API and SDK among Intel SGX, ARM TrustZone-A and RISC-V Keystone 
  - Provides portability for source codes of Trusted Applications among SGX, TrustZone and Keystone
  - Provides subset of Global Platform API

### Hardware Features of TA-Ref on TEE

@image html images/Hardware_Features_for_TA-Ref.png
@image latex images/Hardware_Features_for_TA-Ref.png width=\textwidth

## Components of TA-Ref

### TA-Ref Components on Keystone

@image html images/Ta-ref_components_on_Keystone.png
@image latex images/Ta-ref_components_on_Keystone.png width=\textwidth

### TA-Ref Components on OP-TEE

@image html images/Ta-ref_components_on_Optee.png
@image latex images/Ta-ref_components_on_Optee.png width=\textwidth

### TA-Ref Components on SGX

@image html images/Ta-ref_components_on_SGX.png
@image latex images/Ta-ref_components_on_SGX.png width=\textwidth


## What we did on RISC-V
  - We designed the GP internal API library to be portable.
  - Keystone SDK is utilized because of runtime "Eyrie".
  - The library is ported to Intel SGX as well as RISC-V Keystone.

### Challenges faced during Implementation 
  - The combination of GP internal API and cipher suite is big.
    - To reduce the size, We pick up some important GP internal APIs.
  - Some APIs depend on CPU architecture.
    - We separate APIs into CPU architecture dependent / independent.
  - Integrate GP TEE Internal API to Keystone SDK.
    - Keystone SDK includes EDL (Enclave Definition Language) named “keedger”.
    - Keedger creates the code for OCALL (request from TEE to REE) to check the pointer and boundary.

### Selected GP TEE Internal API's for testing

- CPU architecture dependent
  - Random Generator, Time, Secure Storage, Transient Object(TEE_GenerateKey)

- CPU architecture independent(Crypto)
  - Transient Object(exclude TEE_GenerateKey), Crypto Common, Authenticated Encryption, Symmetric/Asymmetric Cipher, Message Digest

Following shows the table of CPU Dependent and Independent API's with its functions.

@image html images/separate-gp-tee-internal-api.png
@image latex images/separate-gp-tee-internal-api.png width=\textwidth

## Diagram

### Dependency of category

@image html images/dependency_category.png
@image latex images/dependency_category.png width=\textwidth


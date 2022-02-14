# Overview of Trusted Application Reference (TA-Ref)

## Features of TA-Ref
  - Provides Portable API and SDK among Intel SGX, ARM TrustZone-A and RISC-V Keystone 
  - Provides portability for source codes of Trusted Applications among SGX, TrustZone and Keystone
  - Provides subset of Global Platform API

### Assumption of hardware features of TA-Ref on TEE

@image html docs/images/Hardware_Features_for_TA-Ref.png
@image latex docs/images/Hardware_Features_for_TA-Ref.png width=\textwidth

The Secure Time Lock is the date and time clock hardware peripheral which updates monotonically provided separately from regular clock peripheral so the user application and OS on REE could not change the date and/or time. Many certificates of CA, license keys of purchased serial code, hardware enablement keys such as increasing the battery size of the electric cars are bound to the date. The easiest way for end users or attackers phishing the CAs and web sites, using the software and enabling the optional hardware feature without the payment is to change the value of the clock. The concrete date and time is especially important for the telemetry data.

The Secure Storage will be saving the cryptographic keys, Trusted Application binaries, personalization data, telemetry data, and etc, which are security sensitive files must not be tampered by any applications on the REE side. The size of the storage is typically in the order of megabytes to fulfill the required files.

The Random Generator is another requirement of the hardware ensuring the security level of the system. Quality of the random value is very important for having a good security level on many cryptographic algorithms used inside TEE. It is recommended to have an equivalent level of SP 800-90B and FIPS 140-3.

The Cryptographic Hardware accelerators are not strictly mandatory hardware features, however, it is essential to have them to be usable devices to prevent or expose the very slow usability.

## Components of TA-Ref

### TA-Ref Components on Keystone

@image html docs/images/Ta-ref_components_on_Keystone.png
@image latex docs/images/Ta-ref_components_on_Keystone.png width=\textwidth

TA-Ref provides a portable TEE programming environment over the Keystone project on RISC-V RV64GC CPU. Each TA in the Trusted Aria is protected with Physical memory protection (PMP) which is enabled by RISC-V hardware.

- Keystone project
  * https://keystone-enclave.org/

### TA-Ref Components on OP-TEE

@image html docs/images/Ta-ref_components_on_Optee.png
@image latex docs/images/Ta-ref_components_on_Optee.png width=\textwidth

It is on OP-TEE and highly utilizing the programming environment provided by TA-Ref to simplify developing of Trusted Applications to be able to build and function on other CPUs with the single source code.

### TA-Ref Components on SGX

@image html docs/images/Ta-ref_components_on_SGX.png
@image latex docs/images/Ta-ref_components_on_SGX.png width=\textwidth

The diagram shows implementation of TA-Ref and Trusted Applications on SGX. Unlike ARM Cortex-A or RISC-V, the TEE security level is implemented vertically in the user space. The TA-Ref provides the same programming environment of ARM Cortex-A or RISC-V on Intel with the capability subset of Global Platform TEE Internal APIs.

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

@image html docs/images/separate-gp-tee-internal-api.png
@image latex docs/images/separate-gp-tee-internal-api.png width=\textwidth

## Dependency of category

@image html docs/images/dependency_category.png
@image latex docs/images/dependency_category.png width=\textwidth


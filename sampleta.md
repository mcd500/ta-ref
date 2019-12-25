## Writing a sample trusted application on RISC-V

The purpose of this article is to describe an example trusted application with [Keystone enclave](https://keystone-enclave.org/) which is a research TEE on RISC-V and discuss how typical functions should be implemented in the trusted application.

Our example includes some typical functions like:

* get random number
* get times
* make message digest
* protected files
* encryption/decryption with symmetric key
* sign/verify with asymmetric keys

Generally, some of these functions may be implemented with the help of the hardware like random number generator, real time clock, secure storage and some special cryptographic accelerators which can be accessed with enclaves to avoid security issues.
In our case, there is no such special hardware and we use the functions given by the rich os via OCALL. As a result, some functions are restrictive compared with the hardware supports and should be carefully handled.

## Trusted application API

There are several exhaustive trusted application APIs. [GlobalPlatform API](https://globalplatform.org/document_types/api/) is the well-known API for mainly ARM TrustZone.
[Intel SGX sdk](https://software.intel.com/en-us/sgx/sdk) gives their API for Intel CPU with Software Guard Extensions.
[Open Enclave SDK](https://openenclave.io/sdk/) defines TEE API which is independent to the base TEEs.
[Google Asylo](https://asylo.dev/) has another portable TEE API.

The implementation of GlobalPlatform API on TrustZone is based on trusted os called OPTEE os. SGX and Keystone uses very thin runtimes instead of full os.

(Annotation: Keystone uses a thin runtime "Eyrie" as its standard runtime, though it can be replaced with more rich one, for example, SeL4. See https://github.com/keystone-enclave/keystone-seL4)

This design choice effects how trusted computation should be done and their attack surfaces, though both will play essentially same role.
Usually trusted os has rich set of the cryptographic and other functions and has exclusive access rights to security critical resources including special hardware. On the other hand, thin runtimes give some basic functions only. In the latter case, cryptographic functions are processed with enclave itself, not with trusted os calls.
Generally, the attack surface of cryptographic computation is in the trusted os on the former system and in the enclave on the latter.

It affects also the number of context switches required when some operation is done, though this won't be a problem if the functions given by trusted os are relatively heavy.

## OCALL

There are several forms of communication between normal applications and enclaves. OCALL which means 'out call' is a typical one with function call like interface that an enclaves can call some normal world function. Some systems have functions with inverse direction. SGX's normal application can call an enclave function with ECALL. [Fig.1](http://192.168.100.100/vc707/junkyard/blob/master/ocall-howto.pdf) shows how OCALL works in Keystone with a simple example.

There are two glue codes both in the application and the enclave.
Each glue code wraps edge/boundary system calls and serializes/deserializes arguments/results. These wrapper codes should be crafted very carefully because they are put at attack surfaces. See [A Tale of Two Worlds: Assessing the Vulnerability of Enclave Shielding Runtimes](https://people.cs.kuleuven.be/~jo.vanbulck/ccs19-tale.pdf).

These wrapper codes have rather fixed pattern and usually generated with some tools called edger. SGX has edger8r as its edger tool and Open Enclave uses oeedger8r which is a modified version of edger8r. Recently, Keystone gives keyedge as its edger. All edgers generate glue codes from annotated function. edger8r/oeedger defines Enclave Definition Language (EDL) for this purpose. Keyedge uses simple annotations ignored with normal compilation.


```
  [edger8r syntax in .edle]

    untrusted {
        unsigned int ocall_print_string([in, string] const char *str);
    };


  [keyedge syntax in .h]
  
    unsigned int ocall_print_string(keyedge_str const char* str);

```


Keyedge uses the LLVM infrastructure to analyze syntax and the flatcc to serialize/deserialize data. edger8r/oeedger8r is written by ocalm and has its own analyzer/serializer/deserializer. edger8r is relatively easy to port for other system. oeedger8r is an example and we also have ported it to Keystone and used it internally before keyedge is released. All edgers generate wrapper codes which sanitize arguments and results, though there maybe yet unknown issues as "A Tale of..." caveats.

## Emulation of secure storage

Our example enclave gives minimal interfaces which are common for OPTEE, SGX and Keystone. Supported cryptographic operations are 256bit AES CBC/GCM, SHA3 digest and ed25519 sign/verify only. OPTEE gives these functions with their API. For Keystone and SGX, we use cryptographic libraries, though SGX prepares some of these functions in SDK.

OPTEE gives random number and time functions with its standard API. SGX defines these functions in the standard library, though the secure time function can work only on Windows system at the moment. Keystone has no secure random and time functions yet.

The one problematic API is the secure storage API. OPTEE assumes the existence of real secure storage supported by hardware. We use encrypted normal files to emulate the secure storage for SGX and Keystone because we couldn't assume such hardware for them. The file contents are encrypted with AES CBC. The key should be persistent and unpredictable. We construct a key from the data given by report system call function. This scheme gives many restrictions and some issues on API.

* It can't be opened with the append mode. If you want to append something to the object, you have to read all content and write the appended one.
* Read/write is permitted only when the data size is a multiple of 16.
* Open with RW mode isn't supported. Storage(persistent object) should be opened with write-only mode or read-only mode.

The key and initial vector (iv) cause other implementation issue.  The ideal key and initial vector are hard to get in the usual Keystone environment.  We use attestation report as the last resort.  SGX has sgx_get_key function which is essentially EGETKEY/EREPORT wrapper and use it for file encryption.  Keystone/SGX report is enclave/system invariant which depends on some given data.  With using objectID (file name) as the given data, it returns an enclave/system/objectID invariant.  We deduce the key and the initial vector from this invariant.
We use the signature part of the report as key and the iv is got as a digest of the report.  It means that the iv correlates with the key.  This will reduce the endurance against the brute force, though the iv changes with the enclave and objectID.
Those keys add another constraints on Persistent objects. 

* An object can be accessed with only one enclave.
* Changes of system could make all persistent objects obsolete.

Changes of BIOS (sgx) or SM (keystone) will give the different signature even for same enclave.

The rollback attack is the another issue. There is no mechanism to avoid rollback attack with this scheme. The user must update ObjectID or some data in the enclave like version id explicitly for the new contents. This mitigates rollback attack because key/iv are ObjectID invariant. 

## ???performance measurement???


@page SecureStoragePage Secure Storage Functions

Core Functions, Secure Storage Functions.\n

Pseudo code of how to use Secure Storage.\n
These could be implemented using ocall on Keystone.\n
I prefer this feature is implemented in runtime in S-Mode for less overhead rather than switching to host os every time.\n
\n
Almost identical to open(), clone(), read(), write() in POSIX API.

```
--- write file start ---
     TEE_ObjectHandle o;
     char buf[bufsize];

     TEE_CreatePersistantObject(&o, filename, namelen, WO);

     /* fill the date in buffer */

     TEE_WriteObjectData(o, buf, bufsize);

     TEE_CloseObject(o);
--- write file end ---
```
```
--- read file start ---
     TEE_ObjectHandle o;
     char buf[bufsize];

     TEE_OpenPersistantObject(&o, filename, namelen, RO);

     TEE_ReadObjectData(o, buf, bufsize);

     /* use the date in buffer */

     TEE_CloseObject(o);
--- read file end ---
```

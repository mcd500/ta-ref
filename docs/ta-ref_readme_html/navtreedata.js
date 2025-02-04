/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "TA-REF", "index.html", [
    [ "Overview of Trusted Application Reference (TA-Ref)", "index.html", [
      [ "Features of TA-Ref", "index.html#autotoc_md1", [
        [ "Assumption of hardware features of TA-Ref on TEE", "index.html#autotoc_md2", null ]
      ] ],
      [ "Components of TA-Ref", "index.html#autotoc_md3", [
        [ "TA-Ref Components on Keystone", "index.html#autotoc_md4", null ],
        [ "TA-Ref Components on OP-TEE", "index.html#autotoc_md5", null ],
        [ "TA-Ref Components on SGX", "index.html#autotoc_md6", null ]
      ] ],
      [ "What we did on RISC-V", "index.html#autotoc_md7", [
        [ "Challenges faced during Implementation", "index.html#autotoc_md8", null ],
        [ "Selected GP TEE Internal API's for testing", "index.html#autotoc_md9", null ]
      ] ],
      [ "Dependency of category", "index.html#autotoc_md10", null ]
    ] ],
    [ "API comparison with full set of GP API", "a00053.html", [
      [ "GP API", "a00053.html#autotoc_md12", null ]
    ] ],
    [ "How to run sample TA programs on TA-Ref", "a00054.html", [
      [ "Run samples for Keystone", "a00054.html#autotoc_md14", null ],
      [ "Run samples for OP-TEE", "a00054.html#autotoc_md15", null ],
      [ "Run samples for Intel SGX", "a00054.html#autotoc_md16", null ]
    ] ],
    [ "How to write your first 'Hello World' TA Program", "a00055.html", [
      [ "Writing 'Hello World' TA for Keystone", "a00055.html#autotoc_md18", null ],
      [ "Writing 'Hello World' TA for OP-TEE", "a00055.html#autotoc_md19", null ],
      [ "Writing 'Hello World' TA for Intel SGX", "a00055.html#autotoc_md20", null ]
    ] ],
    [ "Tutorials of using GP API's in TEE", "a00056.html", [
      [ "Time Functions", "a00056.html#autotoc_md22", null ],
      [ "Random Functions", "a00056.html#autotoc_md23", null ],
      [ "Hash Functions", "a00056.html#autotoc_md24", null ],
      [ "Symmetric Crypto AES-GCM Functions", "a00056.html#autotoc_md25", null ],
      [ "Asymmetric Crypto Functions", "a00056.html#autotoc_md26", null ],
      [ "Open, Read, Write, Close On Secure Storage", "a00056.html#autotoc_md27", null ],
      [ "API Error Codes and its values", "a00056.html#autotoc_md28", null ]
    ] ],
    [ "Preparation and building TA-Ref with docker", "a00057.html", [
      [ "Preparation", "a00057.html#autotoc_md30", [
        [ "Installing Docker", "a00057.html#autotoc_md31", null ],
        [ "Executing Docker without sudo", "a00057.html#autotoc_md32", null ],
        [ "Create a docker network tamproto", "a00057.html#autotoc_md33", null ]
      ] ],
      [ "Docker images details", "a00057.html#autotoc_md34", null ],
      [ "Building TA-Ref with Docker", "a00057.html#autotoc_md35", [
        [ "Building TA-Ref for Keystone with docker", "a00057.html#autotoc_md36", null ],
        [ "Building TA-Ref for OP-TEE with docker", "a00057.html#autotoc_md37", null ],
        [ "Building TA-Ref for Intel SGX with docker", "a00057.html#autotoc_md38", null ]
      ] ]
    ] ],
    [ "Preparation before building TA-Ref without Docker", "a00058.html", [
      [ "Keystone(RISC-V Unleased)", "a00058.html#autotoc_md40", [
        [ "Required Packages", "a00058.html#autotoc_md41", null ],
        [ "Download RISC-V toolchain and Keystone SDK", "a00058.html#autotoc_md42", null ],
        [ "Run Keystone examples", "a00058.html#autotoc_md43", null ]
      ] ],
      [ "OP-TEE (ARM64 Raspberry Pi 3 Model B)", "a00058.html#autotoc_md44", [
        [ "Required Packages", "a00058.html#autotoc_md45", null ],
        [ "Download and build OP-TEE Toolchains 3.10.0", "a00058.html#autotoc_md46", null ],
        [ "Download OP-TEE 3.10.0", "a00058.html#autotoc_md47", null ],
        [ "Build OP-TEE 3.10.0", "a00058.html#autotoc_md48", null ],
        [ "Run OP-TEE Examples", "a00058.html#autotoc_md49", [
          [ "Launching QEMU Console", "a00058.html#autotoc_md50", null ],
          [ "Run hello world example", "a00058.html#autotoc_md51", null ]
        ] ]
      ] ],
      [ "SGX (Intel NUC)", "a00058.html#autotoc_md52", [
        [ "System Requirements", "a00058.html#autotoc_md53", [
          [ "List of machines which are confirmed to work", "a00058.html#autotoc_md54", null ],
          [ "BIOS Versions which are failed or scucceeded in IAS Test", "a00058.html#autotoc_md55", null ],
          [ "BIOS Settings", "a00058.html#autotoc_md56", null ]
        ] ],
        [ "Required Packages", "a00058.html#autotoc_md57", null ],
        [ "Build SGX", "a00058.html#autotoc_md58", [
          [ "Build and install linux-sgx", "a00058.html#autotoc_md59", null ],
          [ "Build and Install SGX Driver", "a00058.html#autotoc_md60", null ]
        ] ],
        [ "Run sgx-ra-sample", "a00058.html#autotoc_md61", [
          [ "Build sgx-ra-sample", "a00058.html#autotoc_md62", null ],
          [ "Prepare for IAS Test", "a00058.html#autotoc_md63", null ],
          [ "Run IAS Test", "a00058.html#autotoc_md64", null ],
          [ "Possible wget Error", "a00058.html#autotoc_md65", null ],
          [ "BIOS Updating", "a00058.html#autotoc_md66", null ],
          [ "Run LocalAttestation", "a00058.html#autotoc_md67", null ]
        ] ]
      ] ],
      [ "Doxygen", "a00058.html#autotoc_md68", [
        [ "Required Packages", "a00058.html#autotoc_md69", null ],
        [ "Build and Install Doxygen", "a00058.html#autotoc_md70", null ]
      ] ],
      [ "Customizing MbedTLS Configuration file", "a00058.html#autotoc_md71", [
        [ "What can be customized?", "a00058.html#autotoc_md72", null ],
        [ "mbedtls configuration file (config.h)", "a00058.html#autotoc_md73", [
          [ "Optee mbetls config file", "a00058.html#autotoc_md74", null ],
          [ "TA-Ref mbetls config file", "a00058.html#autotoc_md75", null ],
          [ "TEEP-Device mbetls config file", "a00058.html#autotoc_md76", null ]
        ] ],
        [ "Supplement Investigation information", "a00058.html#autotoc_md77", null ]
      ] ]
    ] ],
    [ "Building TA-Ref without Docker", "a00059.html", [
      [ "TA-Ref with Keystone", "a00059.html#autotoc_md79", [
        [ "Cloning and building dependent sources", "a00059.html#autotoc_md80", null ],
        [ "Clone the ta-ref source", "a00059.html#autotoc_md81", null ],
        [ "Build the ta-ref source for test_hello and test_gp", "a00059.html#autotoc_md82", null ]
      ] ],
      [ "TA-Ref with OP-TEE", "a00059.html#autotoc_md83", [
        [ "Clone the ta-ref source", "a00059.html#autotoc_md84", null ],
        [ "Build the ta-ref source for test_hello and test_gp", "a00059.html#autotoc_md85", null ]
      ] ],
      [ "TA-Ref with SGX", "a00059.html#autotoc_md86", [
        [ "Clone the ta-ref source", "a00059.html#autotoc_md87", null ],
        [ "Build the ta-ref source for test_hello and test_gp", "a00059.html#autotoc_md88", null ],
        [ "Check TA-Ref by running test_gp, test_hello, simulation mode on any pc", "a00059.html#autotoc_md89", [
          [ "test_hello", "a00059.html#autotoc_md90", null ],
          [ "test_gp", "a00059.html#autotoc_md91", null ]
        ] ]
      ] ],
      [ "Generating ta-ref.pdf with Doxygen", "a00059.html#autotoc_md92", [
        [ "Cloning source and building docs", "a00059.html#autotoc_md93", null ]
      ] ]
    ] ],
    [ "Running on Development Boards", "a00060.html", [
      [ "Keystone, Unleased", "a00060.html#autotoc_md95", [
        [ "Preparation of rootfs on SD Card", "a00060.html#autotoc_md96", [
          [ "Create SD-card partition manually", "a00060.html#autotoc_md97", null ],
          [ "Write boot and rootfs files into SD-card", "a00060.html#autotoc_md98", null ]
        ] ],
        [ "Copying binaries of test_hello and test_gp", "a00060.html#autotoc_md99", null ],
        [ "Check test_hello and test_gp on Unleased", "a00060.html#autotoc_md100", null ]
      ] ],
      [ "OP-TEE, Raspberry PI 3", "a00060.html#autotoc_md101", [
        [ "Preparation of rootfs on SD Card", "a00060.html#autotoc_md102", null ],
        [ "Copying binaries of test_hello and test_gp to rootfs partition", "a00060.html#autotoc_md103", null ],
        [ "Check test_hello and test_gp", "a00060.html#autotoc_md104", null ]
      ] ],
      [ "SGX, NUC", "a00060.html#autotoc_md105", [
        [ "Copying binaries of test_hello and test_gp to NUC machine", "a00060.html#autotoc_md106", null ],
        [ "Check test_hello and test_gp", "a00060.html#autotoc_md107", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"a00053.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';
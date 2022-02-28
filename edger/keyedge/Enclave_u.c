/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 National Institute of Advanced Industrial Science
 *                           and Technology (AIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
// generated empty file
#include "ocalls_host.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>


/**
 * ocall_print_string() - Prints the str value as a string format.
 * 
 * This function just prints the str as a string and returns string length.
 * 
 * @param  str    The string value for print.
 * 
 * @return str    It return string length
 */
unsigned int ocall_print_string(const char* str){
  printf("%s",str);
  return strlen(str);
}

/**
 * ocall_open_file() - It just calls open function as a return.
 * 
 * This function just calls the file open function as an return.
 * 
 * @param  fname    The character type argument of file name    
 * @param  flags    The integer type argument of flag values  
 * @param  perm     Indicates permissions of new file.
 *  
 * @return open     It returns with function called file open().
 */
int ocall_open_file(const char* fname, int flags, int perm)
{
    return open(fname, flags, perm);
}

/**
 * ocall_close_file() - It just calls close function as a return.
 * 
 * This function just calls the file close function as an return.
 * 
 * @param fdesc    The file descriptor.  
 * 
 * @return         It returns with function called close(fdesc).
 */
int ocall_close_file(int fdesc)
{
    return close(fdesc);
}

/**
 * ocall_write_file256 () - It just calls write function as a return
 * 
 * This function just calls the file write function as an return. Its just 
 * write to the file which are specified in the paramter.
 * 
 * @param  fdesc    The file descriptor of where to write the output.
 * @param  buf      A pointer to a buffer which will be written to the file.
 * @param  len      The number of bytes to write.
 * 
 * @return write    It returns with function called write().
 */
int ocall_write_file256(int fdesc, const char *buf,  unsigned int len)
{
    return write(fdesc, buf, len);
}

/**
 * ocall_unlink() - It just calls unlink function as a return.
 * 
 * This function deletes a name from the filesystem.
 * 
 * @param  path      The file path.  
 * 
 * @return unlink    It returns with function called unlink().
 */
int ocall_unlink(keyedge_str const char *path)
{
    return unlink(path);
}

/**
 * ocall_fstat_size() - It is to return size with help of calling fstat 
 * function.
 *
 * This function firstly declares structure and storing the value of fstat 
 * function into the integer variable ret, After that checking the variable with 
 * if condition returns ret.
 * 
 * @param  fd            The file descriptor
 * 
 * @return ret           Returns the value of fstat function
 * @return st.st_size    Returns size value if success else error occured.
 */
int ocall_fstat_size(int fd)
{
    struct stat st;
    int ret = fstat(fd, &st);
    if (ret < 0) {
        return ret;
    }
    return st.st_size;
}

/**
 * ocall_read_file256() -It is for read the file and it return ret with help 
 * of calling read function.
 * 
 * The ocall_read_file256 function firstly checks with if conditon the count is 
 * greater than 256 means it will assign count as 256 and initialzing the 
 * variable Calling the read function using the given parameters storing the 
 * read value into the newly declared variable and returns the variable.
 * 
 * @param  fdesc    The file descriptor.
 * @param  count    The length of the buffer.
 * 
 * @return ret      It will return read value if success else error occured.
 */
ob256_t ocall_read_file256(int fdesc, unsigned int count)
{
  if (count > 256) {
    count = 256;
  }
  ob256_t ret;
  int rtn = read(fdesc, ret.b, count);
  ret.ret = rtn;
  return ret;
}

/**
 * ocall_ree_time() - It is to get time of the day and return ret 
 * (time structure) with help of calling gettimeofday function.
 * 
 * This function firtly declares the timeval and timezone 
 * structure to store the time after that calling the gettimeofday 
 * function storing the obtained value in the rtn, Inside the if 
 * app_Verbose log it will print the gettimeofday with seconds.
 * 
 * @return ret    It will return ree time if success else error occured.
 */
ree_time_t ocall_ree_time(void)
{
  struct timeval tv;
  struct timezone tz;
  ree_time_t ret;
  int rtn = gettimeofday(&tv, &tz);
#ifdef APP_VERBOSE
  printf("@[SE] gettimeofday %d sec %d usec -> %d\n",tv.tv_sec,tv.tv_usec,rtn);
#endif
  ret.seconds = tv.tv_sec;
  ret.millis = tv.tv_usec / 1000;
  return ret;
}

/**
 * ocall_getrandom16() - It is to print the random buffer using the 
 * getrandom function.
 *  
 * This function declared the varible with specified data type, and calling 
 * the getrandom function storing the obtained value into variable, Inside the 
 * if app_Verbose log it will print the getrandom buf values.
 *
 * @param  flags    The list of flag values.
 * 
 * @return ret      It will return random value if success else error occured.
 */
ob16_t ocall_getrandom16(unsigned int flags)
{
  ob16_t ret;
  ssize_t rtn = getrandom(ret.b, 16, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",ret.b,16,flags,rtn);
#endif
  ret.ret = rtn;
  return ret;
}

/**
 * ocall_getrandom196() - It is to print the random buffer using the 
 * getrandom function.
 *  
 * This function declared the varible with specified data type, and calling 
 * the getrandom function storing the obtained value into variable, Inside the 
 * if app_Verbose log it will print the getrandom buf values.
 *
 * @param  flags    The list of flag values.
 * 
 * @return ret      It will return random value if success else error occured.
 */
ob196_t ocall_getrandom196(unsigned int flags)
{
  ob196_t ret;
  ssize_t rtn = getrandom(ret.b, 196, flags);
#ifdef APP_VERBOSE
  printf("@[SE] getrandom buf %x len %d flags %d -> %d\n",ret.b,196,flags,rtn);
#endif
  ret.ret = rtn;
  return ret;
}

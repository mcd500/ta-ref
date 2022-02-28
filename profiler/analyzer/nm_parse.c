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
#include <stdio.h>
#include <string.h>
#include "nm_parse.h"

#define BUF_SIZE 512
#define POOL_SIZE 30000
#define MAX_ADDR 0xFFFFFFFF
static struct nm_info nm_pool[POOL_SIZE];
static int idx = 0;
    
/**     
 * create_htable() - Creates the hash table which stores data in an 
 * associative manner.
 * 
 * This function returns the hash table where the data is stored in 
 * an array format.
 * 
 * @return list		Updated structure list returns if success, else error 
 *			occured.
 */
static struct list* create_htable(void) {
    struct list* table = (struct list*)malloc(sizeof(struct list)*HASH_SIZE);
    int i;
    for(i = 0; i < HASH_SIZE; i++) {
        table[i].next = &table[i];
        table[i].nm = NULL;
    }
    return table;
}

/**
 * get_key() - Returns the address of the hash key.
 * 
 * This function it returns the modulo operator of address and hash size of 
 * the pointer.
 * 
 * @param addr				Address of the key value.
 *
 * @return Address of the hash key	If success, else error occured.
 */
static size_t get_key(unsigned long addr) {
    return addr % HASH_SIZE;
}

/**
 * get_func_name() - Returns the function name by assigning elements to it.
 * 
 * This function returns  func_name if the element of address is equal 
 * to address of the get_key else returns NULL.
 * 
 * @param table			It's an object of struct list. 
 * @param addr			Address to find the key value.
 *
 * @return String length	If success, else error occured.                            
 */
const char* get_func_name(struct list *table, unsigned long addr) {
    size_t idx = get_key(addr);
    struct list *head = &table[idx];
    struct list *elem;
    for(elem = head->next; elem != head; elem = elem->next) {
        if(elem->addr == addr) {
            return elem->nm->func_name;
        }
    }
    return NULL;
}

/**
 * insert_nm() - Inserts the element into the list.
 * 
 * This function is to insert the element inside the list.
 *
 * @param table		It's an object of struct list. 
 * @param addr		Address of the key value.
 * @param nm		Name of the information of struct nm_info
 */
static void insert_nm(struct list *table, unsigned long addr, struct nm_info* nm) {
    size_t idx = get_key(addr);
    struct list *head = &table[idx];
    struct list *elem = (struct list*)malloc(sizeof(struct list));
    elem->addr = addr;
    elem->nm = nm;
    elem->next = head->next;
    head->next = elem;
}

/**
 * parse_nm() - Returns the table of the list structure.
 * 
 * This function opens the file and checks if the file is empty or not. 
 * If the file is not empty then it reads a line from the file pointer(fp) and
 * stores it into the line. Function name copies to the network pool, and then  
 * inserts the network monitor.
 * 
 * @param fname				File name.
 *
 * @return Updated structure list	If success, else error occured.                
 */
struct list* parse_nm(const char *fname) {
    struct list *table = create_htable();
    char line[BUF_SIZE];
    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        perror("file open");
        return NULL;
    }
    while(fgets(line, BUF_SIZE, fp) != NULL) {
        unsigned long addr;
        char type;
        char func_name[256];
        sscanf(line, "%16lx %c %s", &addr, &type, func_name);
        if(addr >= MAX_ADDR) {
            fprintf(stderr, "too big addr 0x%16lx, %s\n", addr, func_name);
            return NULL;
        }
        nm_pool[idx].type = type;
        strncpy(nm_pool[idx].func_name, func_name, 256);
        insert_nm(table, addr, &nm_pool[idx]);
        idx++;
    }
    return table;
}

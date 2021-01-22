#include <stdio.h>
#include <string.h>
#include "nm_parse.h"

#define BUF_SIZE 512
#define POOL_SIZE 30000
#define MAX_ADDR 0xFFFFFFFF
static struct nm_info nm_pool[POOL_SIZE];
static int idx = 0;
    
/**     
 * create_htable() - Create Hash table is a data structure is stored data in an 
 * dictionaries. 
 * 
 * This function used for data is stored in an array format.
 * 
 * @return list    Updated structure list value returns if success, else error 
 *                 occured.
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
 * get_key() - Get a single key, and try to pick apart function key codes.
 * 
 * get_key function used the hash size and address of the pointer.
 * 
 * @param  addr                       Address of to find the key value.
 *
 * @return Address of the hash key    If success, else error occured.
 */
static size_t get_key(unsigned long addr) {
    return addr % HASH_SIZE;
}

/**
 * get_func_name() - Get function name.
 * 
 * This function find the function name using assign the value of
 * elements.
 * 
 * @param  table             Its a object of struct list. 
 * @param  addr              Address of to find the key value.
 *
 * @return String length     If success, else error occured.                            
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
 * insert_nm() - Insert function name.
 * 
 * This function used for insert the value into list, and all the new value
 * assign in structure of list.
 * 
 * @param table    Its a object of struct list. 
 * @param addr     Address of to find the key value.
 * @param nm       Name of the information of struct nm_info
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
 * parse_nm() - Parse the name of the function.
 * 
 * This function firslty open the file and check the file empty or not, if file
 * not empty then file content print and insert the name of the function.
 * 
 * @param  fname                     File name.
 *
 * @return Updated structure list    If success, else error occured.                
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

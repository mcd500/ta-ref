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
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "profiler_data.h"
#include "stack.h"
#include "analyzer.h"
#include "nm_parse.h"

#define BUF_MAX 65536
// #define CAPTION "+ perf result"
#define COLS "id,idx,start_core_id,end_core_id,depth,addr,funcname,start[clocks],end,duration"
#define FORMAT "%03d,%03ld,%d,%d,%ld,0x%08lx,%s,%ld,%ld,%ld\n"

/**
 * main() - Opens the log file, reads and performs the print operation.
 * 
 * This function opens the log file and read the data inside the log file.
 * for_loop starts to print the column one by one and hence it shows the 
 * complete log details.
 * 
 * @param argc		Argument Count is int and stores number of command-line
 *			arguments passed by the user including the name of the
 *			program.
 * @param argv		Argument Vector is array of character pointers listing all
 *			the arguments.
 *
 * @return 0		If success, else error occured.
 */
int main(int argc, char *argv[]) {
    static char buf[BUF_MAX];
    struct __profiler_header* header;
    struct __profiler_data * data;
    uint64_t count;
    uint64_t i;
    struct list *table;
    if(argc<3) return 0;
    int fd = open(argv[1], O_RDONLY, (mode_t)0600);
    if(fd < 0) {
        perror("file couldn't open");
        return 0;
    }
    if(read(fd, buf, BUF_MAX) < 0) {
        perror("logfile cannot read");
        return 0;
    }
    close(fd);

    if(!(table = parse_nm(argv[2]))) {
        fprintf(stderr, "parse_nm failed\n");
        return 0;
    }

    header = (struct __profiler_header*)buf;
    data = ((struct __profiler_data *)(header + 1));
    count = header->idx;

    unsigned long baseaddr;
    // printf(CAPTION "\n");
    for(i = 0; i < count; i++, data++) {
        struct result res;
        __profiler_nsec_t nsec = data->nsec;
        if(i == 0) {
            baseaddr = (unsigned long)data->callee;
            // printf("baseaddr: 0x%016lx\n", baseaddr);
            printf(COLS "\n");
            res.start = nsec;
            res.end = 0;
            continue;
        }
        switch(data->direction) {
            case CALL:
                res.callee = data->callee;
                res.start_hartid = data->hartid;
                res.start = nsec; res.end = 0;
                res.idx = i;
                push(res);
                break;
            case RET:
                if(is_empty()) return 0;
                res = pop();
                res.end = nsec;
                res.end_hartid = data->hartid;
                __profiler_nsec_t duration = res.end - res.start;
                unsigned long addr = (unsigned long)res.callee - (unsigned long)baseaddr;
                printf(FORMAT, i, res.idx, res.start_hartid, res.end_hartid, res.depth, addr, get_func_name(table, addr), res.start, res.end, duration);
                break;
            default:
                fprintf(stderr, "direction is something wrong!: %d, data->direction\n");
                return 0;
        }
    }
    return 0;
}

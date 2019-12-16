#!/usr/bin/python3

with open("/tmp/tee.log", "r") as f:
    lines = f.readlines()

output_enable=0

for line in lines:
    if "ecall_ta_main() end" in line:
	    output_enable = 0
    if output_enable:
	    print(line, end="")
    if "ecall_ta_main() start" in line:
	    output_enable = 1

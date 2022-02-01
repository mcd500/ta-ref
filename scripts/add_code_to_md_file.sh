startTag=("/*MD_UPDATE_REE_COMMENT_START*/" "/*MD_UPDATE_REE_CODE_START*/" "/*MD_UPDATE_TEE_COMMENT_START*/" "/*MD_UPDATE_TEE_CODE_START*/" "/*MD_UPDATE_TEE_RANDOM_COMMENT_START*/" "/*MD_UPDATE_TEE_RANDOM_CODE_START*/")
endTag=("/*MD_UPDATE_REE_COMMENT_END*/" "/*MD_UPDATE_REE_CODE_END*/" "/*MD_UPDATE_TEE_COMMENT_END*/" "/*MD_UPDATE_TEE_CODE_END*/" "/*MD_UPDATE_TEE_RANDOM_COMMENT_END*/" "/*MD_UPDATE_TEE_RANDOM_CODE_END*/")
len=$((${#startTag[@]}-1)) #length for the array - 1

for (( count = 0; count <= $len; count++ )) 
do

    input_line_start="$(cat ../samples/secure_system/Enclave.c | grep -Fn ${startTag[count]} | cut -f1 -d:)" #reads line number from time.c with the start of code
    input_line_start=$((${input_line_start}+1)) #eliminating the start tag 
   
    input_line_end="$(cat ../samples/secure_system/Enclave.c | grep -Fn ${endTag[count]} | cut -f1 -d:)" #reads line number from time.c with the end of code
    input_line_end=$((${input_line_end}-1)) #eliminating the end tag

    output_line_start="$(cat ../docs/aist_supported_apis.md | grep -Fn ${startTag[count]}  | cut -f1 -d:)" #finding the line number to paste the output.

    sed -i "$output_line_start"r<(sed $input_line_start,$input_line_end'!d' ../samples/secure_system/Enclave.c) ../docs/aist_supported_apis.md
    
done
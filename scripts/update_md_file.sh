# Output MD file name to which the source code and comments are pasted
DOCS_SOURCE_PATH="docs"
MD_FILE="aist_supported_apis.md"
OUTPUT_MD_FILE="${DOCS_SOURCE_PATH}/${MD_FILE}"

# Variables declartions for Input source code 
SAMPLE_SOURCE_PATH="samples"
ENCLAVE_C="Enclave.c"
SECURE_SYSTEM="secure_system"
ASYMMETRIC_KEY="asymmetric_key"
HELLO_TA="hello-ta"
MESSAGE_DIGEST="message_digest"
SYMMETRIC_KEY="symmetric_key"

# Input source code files from where the code and comments extracted
SECURE_SYSTEM_INPUT_FILE="${SAMPLE_SOURCE_PATH}/${SECURE_SYSTEM}/${ENCLAVE_C}"
ASYMMETRIC_KEY_INPUT_FILE="${SAMPLE_SOURCE_PATH}/${ASYMMETRIC_KEY}/${ENCLAVE_C}"
SYMMETRIC_KEY_INPUT_FILE="${SAMPLE_SOURCE_PATH}/${SYMMETRIC_KEY}/${ENCLAVE_C}"
MESSAGE_DIGEST_INPUT_FILE="${SAMPLE_SOURCE_PATH}/${MESSAGE_DIGEST}/${ENCLAVE_C}"


# Start tag of the source code and MD file
startTag=(
    "/*START_REE_TIME_COMMENT_MD_UPD*/"
    "/*START_REE_TIME_SOURCE_MD_UPD*/"
    "/*START_TEE_TIME_COMMENT_MD_UPD*/"
    "/*START_TEE_TIME_SOURCE_MD_UPD*/"
    "/*START_TEE_RANDOM_COMMENT_MD_UPD*/"
    "/*START_TEE_RANDOM_SOURCE_MD_UPD*/")

# End tag of the source code
endTag=(
    "/*END_REE_TIME_COMMENT_MD_UPD*/"
    "/*END_REE_TIME_SOURCE_MD_UPD*/"
    "/*END_TEE_TIME_COMMENT_MD_UPD*/"
    "/*END_TEE_TIME_SOURCE_MD_UPD*/"
    "/*END_TEE_RANDOM_COMMENT_MD_UPD*/"
    "/*END_TEE_RANDOM_SOURCE_MD_UPD*/")


# Compute the total length of tags to be iterated
len=$((${#startTag[@]}-1))

# Iterate for the number of tags present
for (( count = 0; count <= $len; count++ )) 
do
    # Read the input start line number from source file
    input_line_start="$(cat ${SECURE_SYSTEM_INPUT_FILE}| grep -Fn ${startTag[count]} | cut -f1 -d:)" 
    # Skip the line number of the start tag
    input_line_start=$((${input_line_start}+1)) 

    # Read the input end line number from source file
    input_line_end="$(cat ${SECURE_SYSTEM_INPUT_FILE}| grep -Fn ${endTag[count]} | cut -f1 -d:)" 
    # Skip the line number of end tag
    input_line_end=$((${input_line_end}-1))
    
    # Find the line number to insert the souce code / comments from source file to md file.
    output_line_start="$(cat ${OUTPUT_MD_FILE} | grep -Fn ${startTag[count]}  | cut -f1 -d:)" 

    # Delete the start tag comment in the md file
    sed -i "$output_line_start"d ${OUTPUT_MD_FILE}
    # Insert the extracted text into the md file
    sed -i "$output_line_start"r<(sed $input_line_start,$input_line_end'!d' ${SECURE_SYSTEM_INPUT_FILE}) ${OUTPUT_MD_FILE}
    
done
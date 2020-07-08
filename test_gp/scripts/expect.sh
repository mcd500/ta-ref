if [ "${ANALYZE}" = "ON" ]; then
${CURDIR}/../${TEE}/scripts/analyze.expect
else
${CURDIR}/../${TEE}/scripts/demo.expect
fi

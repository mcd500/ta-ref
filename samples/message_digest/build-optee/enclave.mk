BINARY = a6f77c1e-96fe-4a0e-9e74-262582a4c8f1
CROSS_COMPILE = aarch64-linux-gnu-
CPPFLAGS = \
	-I../../../../ta-ref/build/include \
	-I../../../../ta-ref/build/include/api \
	-I../../../../ta-ref/build/include/gp \
	-Dtee_printf=printf

-include $(TA_DEV_DIR)/mk/ta_dev_kit.mk

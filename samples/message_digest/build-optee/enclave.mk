BINARY = 8d82573a-926d-4754-9353-32dc29997f74
CROSS_COMPILE = aarch64-linux-gnu-
CPPFLAGS = \
	-I../../../../ta-ref/build/include \
	-I../../../../ta-ref/build/include/api \
	-I../../../../ta-ref/build/include/gp \
	-Dtee_printf=printf

-include $(TA_DEV_DIR)/mk/ta_dev_kit.mk

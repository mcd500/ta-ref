.PHONY: all clean

all: image

image: ship

ship:
	install -m 0755 $(BINARY) $(OPTEE_DIR)/out-br/target/usr/bin

clean:

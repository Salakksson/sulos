CC16=/usr/bin/watcom/binl/wcc
LD16=/usr/bin/watcom/binl/wlink

SRC_DIR=src
BUILD_DIR=build
FILES_DIR=fs

TARGET=pushos

.PHONY: all $(TARGET) kernel bootloader clean always

all: $(TARGET)

#
# Floppy image
#
$(TARGET): $(BUILD_DIR)/$(TARGET).img

$(BUILD_DIR)/$(TARGET).img: bootloader kernel
	@dd if=/dev/zero of=$(BUILD_DIR)/$(TARGET).img bs=512 count=2880
	@mkfs.fat -F 12 -n "DISK" $(BUILD_DIR)/$(TARGET).img
	@dd if=$(BUILD_DIR)/bin/boot.bin of=$(BUILD_DIR)/$(TARGET).img conv=notrunc
	@mcopy -i $(BUILD_DIR)/$(TARGET).img $(BUILD_DIR)/bin/main.bin "::main.bin"
	@mcopy -i $(BUILD_DIR)/$(TARGET).img $(BUILD_DIR)/bin/kernel.bin "::kernel.bin"
	@mcopy -s -i $(BUILD_DIR)/$(TARGET).img $(FILES_DIR)/* ::/


#
# Bootloader
#
bootloader: boot main

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: always
	@$(MAKE) -s -C $(SRC_DIR)/bootloader/boot BUILD_DIR="$(abspath $(BUILD_DIR)/bin)"

main: $(BUILD_DIR)/main.bin

$(BUILD_DIR)/main.bin: always
	@$(MAKE) -s -C $(SRC_DIR)/bootloader/main BUILD_DIR="$(abspath $(BUILD_DIR)/bin)"

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	@$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR="$(abspath $(BUILD_DIR)/bin)"

#
# Always
#
always:
	@mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
#	$(MAKE) -C $(SRC_DIR)/bootloader/boot BUILD_DIR="$(abspath $(BUILD_DIR))" clean
#	$(MAKE) -C $(SRC_DIR)/bootloader/main BUILD_DIR="$(abspath $(BUILD_DIR))" clean
#	$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR="$(abspath $(BUIL:D_DIR))" clean
	@rm -f $(BUILD_DIR)/*


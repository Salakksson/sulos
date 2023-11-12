CC16=/usr/bin/watcom/binl/wcc
LD16=/usr/bin/watcom/binl/wlink

SRC_DIR=src
BUILD_DIR=build

.PHONY: all floppy_image kernel bootloader clean always

all: floppy_image

#
# Floppy image
#
floppy_image: $(BUILD_DIR)/main_floppy.img

$(BUILD_DIR)/main_floppy.img: bootloader kernel
	@dd if=/dev/zero of=$(BUILD_DIR)/main_floppy.img bs=512 count=2880
	@mkfs.fat -F 12 -n "NBOS" $(BUILD_DIR)/main_floppy.img
	@dd if=$(BUILD_DIR)/boot.bin of=$(BUILD_DIR)/main_floppy.img conv=notrunc
	@mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/main.bin "::main.bin"
	@mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/kernel.bin "::kernel.bin"
	@mcopy -i $(BUILD_DIR)/main_floppy.img $(BUILD_DIR)/test.txt "::test.txt"

#
# Bootloader
#
bootloader: boot main

boot: $(BUILD_DIR)/boot.bin

$(BUILD_DIR)/boot.bin: always
	@$(MAKE) -C $(SRC_DIR)/bootloader/boot BUILD_DIR="$(abspath $(BUILD_DIR))"

main: $(BUILD_DIR)/main.bin

$(BUILD_DIR)/main.bin: always
	@$(MAKE) -C $(SRC_DIR)/bootloader/main BUILD_DIR="$(abspath $(BUILD_DIR))"

#
# Kernel
#
kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: always
	@$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR="$(abspath $(BUILD_DIR))"

#
# Always
#
always:
	@mkdir -p $(BUILD_DIR)

#
# Clean
#
clean:
	#$(MAKE) -C $(SRC_DIR)/bootloader/boot BUILD_DIR="$(abspath $(BUILD_DIR))" clean
	#$(MAKE) -C $(SRC_DIR)/bootloader/main BUILD_DIR="$(abspath $(BUILD_DIR))" clean
	#$(MAKE) -C $(SRC_DIR)/kernel BUILD_DIR="$(abspath $(BUIL:D_DIR))" clean
	@rm -rf $(BUILD_DIR)/*

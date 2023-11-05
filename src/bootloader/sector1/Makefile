BUILD_DIR?=build
ASM?=nasm

.PHONY: all clean

all: sector1

sector1: $(BUILD_DIR)/sector1.bin

$(BUILD_DIR)/sector1.bin:
	@$(ASM) boot.asm -f bin -o "$(BUILD_DIR)/sector1.bin"

clean:
	@rm -f "$(BUILD_DIR)/sector1.bin"

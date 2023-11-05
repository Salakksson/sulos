BUILD_DIR?=build
ASM?=nasm
ASMFLAGS?=-f obj
CC16?=/usr/bin/watcom/binl/wcc
CFLAGS16?=-4 -d3 -s -wx -ms -zl -zq # -oneatxzh
LD16?=/usr/bin/watcom/binl/wlink

SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/main/c/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/main/asm/%.obj, $(SOURCES_ASM))

.PHONY: all main clean always

all: main

main: $(BUILD_DIR)/main.bin

$(BUILD_DIR)/main.bin: $(OBJECTS_ASM) $(OBJECTS_C)
	@$(LD16) NAME $(BUILD_DIR)/main.bin FILE \{ $(OBJECTS_ASM) $(OBJECTS_C) \} OPTION MAP=$(BUILD_DIR)/main.map @linker.lnk

$(BUILD_DIR)/main/c/%.obj: %.c always
	@$(CC16) $(CFLAGS16) -fo="$@" $<

$(BUILD_DIR)/main/asm/%.obj: %.asm always
	@$(ASM) $(ASMFLAGS) -o "$@" $<

always:
	@mkdir -p $(BUILD_DIR)/main/c
	@mkdir -p $(BUILD_DIR)/main/asm

clean:
	@rm -f "$(BUILD_DIR)/main.bin"

ASM=nasm

SRC_DIR = Assemply
BUILD = build

$(BUILD)/test_floppy.img: $(BUILD)/test.bin
	cp $(BUILD)/test.bin $(BUILD)/test_floppy.img
	truncate -s 1440k $(BUILD)/test_floppy.img

$(BUILD)/test.bin: $(SRC_DIR)/test.asm
	$(ASM) $(SRC_DIR)/test.asm -f bin -o $(BUILD)/test.bin

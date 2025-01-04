XLEN ?= 32
CROSS   = riscv$(XLEN)-unknown-elf-
CC      = $(CROSS)gcc
CPP     = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
AR      = $(CROSS)ar
DEBUG ?= 1
# BASE_ADDRESS ?= 0x80000000

DESTDIR ?= /usr/local/

ifeq ($(XLEN), 64)
    MARCH = rv64ima_zicsr_zifencei
    MABI = lp64
	STACK_SIZE = 600
else
    MARCH = rv32ima_zicsr_zifencei
    MABI = ilp32
	STACK_SIZE = 300
endif

BUILD_DIR = build

CFLAGS  = -march=$(MARCH) -mabi=$(MABI) -mcmodel=medany \
	-D__riscv_float_abi_soft \
	-I include/ \
	-Wall \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fno-builtin-printf

ifeq ($(DEBUG), 1)
    CFLAGS += -Og -ggdb3
else
    CFLAGS += -Os
endif

SRCS = src/riscv.c
HEADERS = typetag.h riscv.h

OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%$(XLEN).o) $(ASMS:src/%.S=$(BUILD_DIR)/%$(XLEN).o)
DEPS = $(SRCS:src/%.c=$(BUILD_DIR)/%$(XLEN).d) $(ASMS:src/%.S=$(BUILD_DIR)/%$(XLEN).d)

TARGET = libtypetag.a
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(BUILD_DIR)/%$(XLEN).o: src/%.c 
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%$(XLEN).o: src/%.S 
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: $(BUILD_DIR)/$(TARGET)
	@mkdir -p "$(DESTDIR)/include"
	cp -r include/* "$(DESTDIR)/include"
	@mkdir -p "$(DESTDIR)/lib"
	cp $(BUILD_DIR)/$(TARGET) "$(DESTDIR)/lib"

-include $(DEPS)

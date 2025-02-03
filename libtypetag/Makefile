.PHONY: target

ARCH ?= 
DEBUG ?= 1

ifeq ($(ARCH), riscv)
	TARGET_NAME = libtypetag-riscv
	DESTDIR ?= /opt/x-tools/riscv32-unknown-elf/
	BUILD_DIR ?= build-riscv
	TARGETS = $(TARGET_NAME).a

	XLEN ?= 32
	CROSS   = riscv$(XLEN)-unknown-elf-
	CC      = $(CROSS)gcc
	CPP     = $(CROSS)cpp
	OBJCOPY = $(CROSS)objcopy
	AR      = $(CROSS)ar
	# BASE_ADDRESS ?= 0x80000000

	ifeq ($(XLEN), 64)
		MARCH = rv64ima_zicsr_zifencei
		MABI = lp64
		STACK_SIZE = 600
	else
		MARCH = rv32ima_zicsr_zifencei
		MABI = ilp32
		STACK_SIZE = 300
	endif

	CFLAGS += -march=$(MARCH) -mabi=$(MABI) -mcmodel=medany \
		-D__riscv_float_abi_soft \
		-fmessage-length=0 \
		-ffunction-sections \
		-fdata-sections \
		-fno-builtin-printf

	SRCS = src/control_riscv.c src/typetag.c
else
	TARGET_NAME = libtypetag
	DESTDIR ?= /usr/local/
	BUILD_DIR ?= build-x86
	TARGETS = $(TARGET_NAME).so $(TARGET_NAME).a

	CC      = gcc
	CPP     = cpp
	OBJCOPY = objcopy
	AR 	    = ar
	LDFLAGS =

	SRCS = src/typetag.c
endif

CFLAGS += -I include/ -Wall

ifeq ($(DEBUG), 1)
    CFLAGS += -Og -ggdb3
endif

OBJS = $(SRCS:src/%.c=$(BUILD_DIR)/%$(XLEN).o) $(ASMS:src/%.S=$(BUILD_DIR)/%$(XLEN).o)
DEPS = $(SRCS:src/%.c=$(BUILD_DIR)/%$(XLEN).d) $(ASMS:src/%.S=$(BUILD_DIR)/%$(XLEN).d)

BUILD_TARGETS = $(addprefix $(BUILD_DIR)/, $(TARGETS))
target: $(BUILD_TARGETS) 

$(BUILD_DIR)/$(TARGET_NAME).a: $(OBJS)
	$(AR) rcs $@ $^

$(BUILD_DIR)/$(TARGET_NAME).so: $(OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(BUILD_DIR)/%$(XLEN).o: src/%.c 
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%$(XLEN).o: src/%.S 
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: target
	@mkdir -p "$(DESTDIR)/include"
	cp -r include/* "$(DESTDIR)/include"
	@mkdir -p "$(DESTDIR)/lib"
	cp $(BUILD_TARGETS) "$(DESTDIR)/lib"

-include $(DEPS)

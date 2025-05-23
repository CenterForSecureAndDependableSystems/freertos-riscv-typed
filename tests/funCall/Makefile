XLEN ?= 32
CROSS   = riscv$(XLEN)-unknown-elf-
CC      = $(CROSS)gcc
CPP     = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar
DEBUG ?= 0
BASE_ADDRESS ?= 0x80000000

ifeq ($(XLEN), 64)
    MARCH = rv64ima_zicsr_zifencei
    MABI = lp64
	STACK_SIZE = 600
else
    MARCH = rv32ima_zicsr_zifencei
    MABI = ilp32
	STACK_SIZE = 300
endif

BUILD_DIR       = build
PROJECT_DIR     = /home/researcher/project
RTOS_SOURCE_DIR = $(PROJECT_DIR)/FreeRTOS/FreeRTOS/Source

CPPFLAGS = \
	-D__riscv_float_abi_soft \
	-DportasmHANDLE_INTERRUPT=handle_trap \
	-I . -I ../Common/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V \
	-I $(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions
CFLAGS  = -march=$(MARCH) -mabi=$(MABI) -mcmodel=medany \
	-Wall \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-fno-builtin-printf
LDFLAGS = -nostartfiles \
	-march=$(MARCH) -mabi=$(MABI) -mcmodel=medany \
	-Xlinker --gc-sections \
	-Xlinker --defsym=__stack_size=$(STACK_SIZE)

ifeq ($(DEBUG), 1)
    CFLAGS += -Og -ggdb3
else
    CFLAGS += -Os
endif

SRCS = main.c riscv-virt.c htif.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c  \
	$(RTOS_SOURCE_DIR)/tasks.c  \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/port.c

ASMS = start.S \
	$(RTOS_SOURCE_DIR)/portable/GCC/RISC-V/portASM.S

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%$(XLEN).o) $(ASMS:%.S=$(BUILD_DIR)/%$(XLEN).o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%$(XLEN).d) $(ASMS:%.S=$(BUILD_DIR)/%$(XLEN).d)

$(BUILD_DIR)/RTOSDemo$(XLEN).axf: $(OBJS) $(BUILD_DIR)/fake_rom$(BASE_ADDRESS).lds Makefile
	$(CC) $(LDFLAGS) $(OBJS) -T$(BUILD_DIR)/fake_rom$(BASE_ADDRESS).lds -o $@

$(BUILD_DIR)/%$(XLEN).o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%$(XLEN).o: %.S Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

# Run lds through the C preprocessor, to replace BASE_ADDRESS with the actual
# value. It might be simpler to use sed instead.
$(BUILD_DIR)/%$(BASE_ADDRESS).lds: fake_rom.lds Makefile
	$(CPP) $(CPPFLAGS) -DBASE_ADDRESS=$(BASE_ADDRESS) $< | grep -v '^#' > $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

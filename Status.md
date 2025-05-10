# Project Status

# libtypetag
`libtypetag` is a C library which provides utilities for reading and modifying
tags (`typetag_t`). Currently these are just single bytes, and the functions
interpret the bits as tag state.

The structure of the tags are partially described in [UIdaho Tagging Model.docx]("UIdaho Tagging Model.docx"), briefly:

```
  7       7 6     5 4      2 1      0
 ┌─────────┬───────┬────────┬────────┐
 │multibyte│  ref  │  type  │  size  │
 └─────────┴───────┴────────┴────────┘
```
- Size (2 bits): Size of object in bytes. One of {1 `(00)`, 2 `(01)`, 4 `(10)`, 8 `(11)`}.
- Type (3 bits): Object type
	- Special = 0x0
	- Code (Executable) = 0x1...
	- Collection
	- Bit Vector (Raw)
	- Unsigned int
	- Signed int
	- Floating point
	- Reference: (NOTE: Currently used as a return address tag in the implementation)
- Ref (2 bits): Not exactly sure what these do or how they should be implemented.
	- None = 0x0
	- Basic (Pointer?)
	- Collection Size
	- Collection Reference
- Multibyte (1 bit): Whether this tag is part of a multibyte object.

### Control Functions
Several functions for controlling the tagging
features are provided in the `control.h` header.
These functions use custom no-op instructions which are interpreted by the modified SPIKE simulator. In a final non-testing implementation, many of these would likely be disabled.

- `tt_set_checks(int enabled)`: Enable or disable tag checking (0 to disable, 1 to enable). 
- `tt_set_prop(int enabled)`: Enable or disable tag propagation (0 to disable, 1 to enable). 
- `tt_get_tag(char* ptr)`: Get the `typetag_t` associated with the byte at `ptr`. The `char*` typing is used to address a single byte.
- `tt_set_tag(char* ptr, typetag_t tag)`: Sets the tag associated with `ptr` to `tag`.

### libtypetag Usage
This library can be built and installed to both the system environment and the
RISCV x-tools toolchain. The Docker image should automatically build and install
both with the `install.sh` script.

If you need to build the library externally, you can specify the `ARCH` variable
when using `make`
to build for a RISC-V x-tools environment:
```bash
# Install to /usr/local 
make install
# Install to /opt/x-tools/riscv32-unknown-elf
# using the riscv32-unknown-elf- xtools toolchain
make install ARCH=riscv
```

The default system version is only used when building SPIKE, RISC-V binaries
should link to the riscv version:

```make
CROSS_ROOT = /opt/x-tools/riscv32-unknown-elf/
LIBS = -L $(CROSS_ROOT)/lib -l:libtypetag-riscv.a
```

# Tests
The [basicTag](tests/basicTag) project is where I've been testing new features,
additional tests should use it as a template as it includes a more stripped-down
Makefile which still uses the correct x-tools toolchain. Other tests are
included which demonstrate vulnerabilities. Tests can generally be built with
`make`, the `.axf` file produced can be run with `spike`.

# SPIKE (riscv-isa-sim-typed)
Some features that have been implemented:
- Definition of tag memory regions, which are only accessible by instruction implementations through the `MMU.tag_load<T>()` and `MMU.tag_store<T>()` functions (For debugging, only writes are restricted, so debuggers can read tag memory. This bypass can be found in `mmu_t::translate`).
- Tag propagation for basic load and store instructions `sw, lw, sb, lb`, etc.
- Tag processing controls via pseudoinstructions (such as `slti`, see libtypetag).
- Loading tag memory from ELF files through custom sections.
- Extra tag trap exceptions for tag checking violations defined in [trap.h](riscv-isa-sim-typed/riscv/trap.h), with IDs defined at [typetag/include/exception.h](libtypetag/include/typetag/exception.h). Currently, these are:
	- `TT_EXP_INT_OVERFLOW`
	- `TT_EXP_INT_DIV_ZERO`
	- `TT_EXP_INVALID_CALL_TAG`
	- `TT_EXP_INVALID_INDIRECT_TAG`
	- `TT_EXP_INVALID_RETURN_TAG`

	Only `TT_EXP_INVALID_RETURN_TAG` is implemented at the moment, the others are included as part of the current spec.

- Tag exception controls in `processor_t`:
	```cpp
	bool get_tag_checking_enabled();
	bool get_tag_propagation_enabled();
	void set_tag_checking(bool enabled);
	void set_tag_propagation(bool enabled);
	TrapMode get_tag_trap_mode(tagexception_t e);
	void set_tag_trap_mode(tagexception_t e, TrapMode mode);
	```	
	Where `TrapMode` is an enum with the options:
	```cpp
	TRAP_DISABLED = 0x0, // No action on tag violation
	TRAP_ENABLED = 0x1, // Trigger hardware fault on violation
	TRAP_WARN = 0x2 // Emit debug message on violation, no other action
	``` 
	For debug messages to be emitted, `TYPE_TAGGING_DEBUG` must be defined (currently defined in [decode_macros_tt.h](riscv-isa-sim-typed/riscv/decode_macros_tt.h)).

- Return address tagging on `jal, jalr` instructions (only for direct jumps), 
which throw an `TT_EXP_INVALID_RETURN_TAG` fault if the tag on the return address has been tampered with.
- Reworked some parts of ELF file loading to provide enough information for tag region creation.

## Building SPIKE
SPIKE uses the [autoconf](https://www.gnu.org/software/autoconf/) suite of build
tools for building. A `configure` script is already provided, so you shouldn't
need to run `autoconf` generally. The general process for building is:
```bash
cd riscv-isa-sim-typed
# If you need to recreate the configure script:
# autoconf 

cd build
../configure
make -j$(nproc)

# If you want to use the spike-run32 command, make sure to reinstall
sudo make install
```

If you really want, the `configure` script includes options for
enabling/disabling type tagging and specifying the libtypetag directories,
though I haven't tested these super well. You shouldn't need to touch these,
tagging is enabled by default:

```bash
# Output of ./configure --help
  --enable-type-tagging   Enable experimental type tagging
  --with-typetag-libdir=LIB_DIR
                          Force given directory for libtypetag.
  --with-typetag-includedir=INCLUDE_DIR
                          Force given directory for libtypetag.
```

A VSCode [tasks.json](.vscode/tasks.json) file is also provided with tasks for
building and reinstalling spike into the container (Ctrl+Shift+B). Installing
requires you to input a password into the task terminal that pops up.

## SPIKE Usage
To run RISC-V binaries through spike, you can use the script `spike-run32`:

```bash
spike-run32 build/basicTag32.axf

# Or, do it manually:
spike \
	-p1 \
	--isa RV32IMA \
	-m0x80000000:0x10000000 \
	--tag-mem 0x90000000:0x10000000:0x80000000 \
	--rbb-port 9824 \
	build/basicTag32.axf
```

Note the flags: 
``` bash
# Defines a memory region, base:size
-m0x80000000:0x10000000

# Defines a tag memory region, base(of the tag region):size:mapped base(the base of the normal memory region)
--tag-mem 0x90000000:0x10000000:0x80000000 \
```

`--tag-mem` is described more in the project [README.md].

### Debugging

For debugging RISC-V binaries running in spike, you need to run `openocd`, which
you can attach gdb through. A config file is provided at [spike-openocd.cfg],
which is roughly the same as the ones found in the example directories
(`spike-1.cfg`). The script [scripts/openocd-gdb-attach] starts openocd in the
background and attaches gdb afterward, writing openocd's output to
`openocd.log`. You can also run them separately:

```bash
# Terminal 1
spike-run32 <path to binary>
```

```bash
# Terminal 2
openocd-gdb-attach <path to binary>

# Or manually ----------
# Terminal 2
openocd -f /home/researcher/project/spike-openocd.cfg

# Terminal 3
riscv32-unknown-elf-gdb \
	-ex 'target extended-remote localhost:3333' \
	-ex 'layout split' \
	<path to binary>
```

For debugging SPIKE itself, you can directly run it through (or attach) gdb. If
using VSCode, this `launch.json` configuration works (also provided in the repo):

```json
{
	"version": "0.2.0",
	"configurations": [
		{
			"name": "(gdb) Launch",
			"type": "cppdbg",
			"request": "launch",
			"program": "/home/researcher/project/riscv-isa-sim-typed/build/spike",
			"args": [
				"--halted", 
				"-p1", "--isa", "RV32IMA", 
				"-m0x80000000:0x10000000", "--rbb-port", "9824", 
				"--tag-mem", "0x90000000:0x10000000:0x80000000",
				"${input:path}"
			],
			"stopAtEntry": false,
			"cwd": "${fileDirname}",
			"environment": [],
			"externalConsole": false,
			"MIMode": "gdb",
			"setupCommands": [
				{
					"description": "Enable pretty-printing for gdb",
					"text": "-enable-pretty-printing",
					"ignoreFailures": true
				},
				{
					"description": "Set Disassembly Flavor to Intel",
					"text": "-gdb-set disassembly-flavor intel",
					"ignoreFailures": true
				}
			]
		}
	],
	"inputs": [
		{
			"id": "path",
			"type": "promptString",
			"description": "Binary path",
			"default": "/home/researcher/project/tests/basicTag/build/basicTag32.axf"
		}
	]
}
```

## Tag Regions / ELF Loading
Tag regions are mapped on top of normal memory regions either through user defined regions (using the `--tag-mem` flag), or
when loading ELF files with the custom section type. ELF defined
tag regions are loaded after user regions, and will override those region definitions. 

For example, if SPIKE is started with `--tag-mem 9000:1000:8000`, a tag region
at `(tag)[9000, 10000]` mapped onto `(mem)[8000, 9000]` will be created. Note
the `-m` flag is needed for the normal memory (`-m8000:1000`), but not for the
tag region. If an ELF file is loaded which maps `(tag)[8100, 8200]` to
`(mem)[8200, 8300]`, the original `mem` region will be split and resized, and
the new regions will be created between them:

```
Before Loading:
Name    Location       Size
---------------------------
A.mem   [8000, 9000]   1000
A.tag   [9000, 10000]  1000

After Loading:
Name    Location       Size
---------------------------
A.mem   [8000, 8100]   100
B.mem   [8100, 8200]   100
B.tag   [8200, 8300]   100
C.mem   [8300, 9000]   700
A.tag   [9000, 9100]   100  # Note the 'dead' memory between [9100,9300]
C.tag   [9300, 10000]  700  
```
(The names here are only for demonstration, tag regions don't store names
currently).

Note the 'dead' region at `[9100,9300]`. This is wasteful but fine, as the
memory previously being covered by this area (`B`) is now being covered by
`B.tag` (or is taken up by `B.tag`, which doesn't need tag coverage).

Also note that only regular 'mapped' memory regions can be overridden with new
mappings, as overriding tag regions would mean the associated mapped regions
would lose their tag memory. Check
[tag_regions.cc](riscv-isa-sim-typed/riscv/tag_regions.cc) for more details.

### ELF Loading
Tag regions can be loaded from section/segment pairs in an ELF file. ELF loading
occurs in [htif.cc](riscv-isa-sim-typed/fesvr/htif.cc) and
[elfloader.cc](riscv-isa-sim-typed/fesvr/elfloader.cc). Loading starts in
`htif_t::start()`, segments are actually parsed in `load_elf()` (from
elfloader.cc).  Once program loading is complete (tag memory sections get loaded
into memory like any other loadable section, with a segment definition), 
`htif_t::start()` calls `tag_regions_t::find_elf_tag_regions` to identify the
tag sections and map the appropriate regions.

> TODO: Currently I have SPIKE checking sections to determine tag regions,
> may want to consider if marking segments instead is more appropriate.

Tag sections use the `sh_type` `0x700000000`, which is the start of the
processor specific custom section type range (Quick reference for section headers [here](https://gist.github.com/x0nu11byt3/bcb35c3de461e5fb66173071a2379779#section-headers-shdr)). When `tag_regions_t::find_elf_tag_regions`
finds a section of this kind, it grabs its `sh_link` field and uses it as an index
into the list of all sections in the ELF file. The virtual address of the specified
section is used as the mapped base of the tag region. So:

```
[Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
...
[ 5] .data             PROGBITS        80080000 005000 000550 00  WA  0   0  8
...
[24] .data_tag         NULL            800831e0 03d000 000550 00      5   0 4096
# readelf reads the type as NULL, because its unidentified, sh_link is 0x700000000.
# Note the link (Lk) field is 5.
...
```

> TODO: This system limits the way tag regions can be assigned, we may want to
> allow arbitrary region placement rather than requiring a mapped section in the future.

The tool [elf-inject-cpp](tools/elf-inject-cpp) is available for testing ELF loading,
allowing you to inject a new section containing custom data. See the
[README](tools/elf-inject-cpp/README.md) file there for details.

## Other SPIKE Notes

### Useful File List
Generally, new type tagging content has been separated into
separate files, though changes have been made in some normal SPIKE source. I've made an effort to guard significant changes with `#ifdef TYPE_TAGGING_ENABLED`, which can be toggled during
build configuration (see [#Building SPIKE]).

- [insns/](riscv-isa-sim-typed/riscv/insns): Directory for instructions. These
files are included by `insn_template.cc`, which gets preprocessed at build time
to
replace all instances of `NAME` with each instruction name.
Note these template functions provide the parameters `processor_t* p, insn_t
insn, reg_t pc`, which are used by the macros defined in `decode_macros.h`.

- [decode_macros.h](riscv-isa-sim-typed/riscv/decode_macros.h): Macros used
within many instruction headers, including things like `MMU`, `READ_REG`, `RD`,
etc.
- [processor.h](riscv-isa-sim-typed/riscv/processor.h): Defines `processor_t`,
which contains tag state controls like `get_tag_checking_enabled()`.
- [mmu.h](riscv-isa-sim-typed/riscv/mmu.h): Defines `mmu_t`, which contains
special `tag_store()` and `tag_load()` functions.
- [config.h](riscv-isa-sim-typed/config.h.in) Contains the
`TYPE_TAGGING_ENABLED` definition (set by the `configure` script). You'll need
to include `config.h` if you want to use that definition.
- [spike.cc](riscv-isa-sim-typed/spike_main/spike.cc) Main function for spike, contains parsing and setup for the `--tag-mem` flag.

**Type Tagging Files**:
- [decode_macros_tt.h](riscv-isa-sim-typed/riscv/decode_macros_tt.h): Macros for instructions specific to type tagging. Includes `READ_REG_TAG`, `IF_TAG_PROPAGATION`, etc. 
- [tag_regions.h](riscv-isa-sim-typed/riscv/tag_regions.h): Object for managing tag/memory mappings.

# Future Work

- Consider improvements to the tag structure:
	- Prioritize object size (to prevent spatial violations) 
		- Check whether offsets to references fall within object bounds
		- Consider requiring that field accesses use direct pointers rather than offsets to base pointers for tag applications (See RV-CURE[^1])
	- Prioritize reference validity (to prevent temporal violations)
	- Typing may still be useful to prevent misinterpretation (i.e. when casting to and from generic void*).
	- May need to consider extra metadata table for storing object bounds and validity mechanisms.
	- Generally check out RV-CURE[^1], which has nice protections sans typing.
- Add propagation/checking for more instructions
- Work out collection/multibyte object implementation?


[^1]: Y. Kim, A. Kar, J. Lee, J. Lee, and H. Kim, “RV-CURE: A RISC-V Capability Architecture for Full Memory Safety,” Aug. 05, 2023, arXiv: arXiv:2308.02945. doi: 10.48550/arXiv.2308.02945.

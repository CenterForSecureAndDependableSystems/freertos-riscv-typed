# elf-inject-cpp

## Building
Build with CMake:
```bash
cmake -B build -S . -G Ninja
cmake --build build
```

## Usage
This tool injects the contents of a binary file into the target ELF, meaning you'll
need to create a binary file of an appropriate size for testing. You can do this
with `dd`:

```bash
# Create a file <file> with <size> bytes. Filling it with 0x1 allows us
# to more easily differentiate between uninitialized memory and the test data.
tr '\0' '\1' < /dev/zero | dd of=<file> bs=1 count=<size>
```

Note that the size of the file must match (or be larger than) the size of the
section you're linking to. A smaller section will work, but will not contain data.

```bash
SIZE=$(readelf -S | grep '\s.data\s' | awk '{print $7}')
```

Then, inject the new section. The tag section type will be applied to it, and a
loadable segment will also be created:

```bash
./build/elf-inject myELF.axf out.axf .data_tag .data data_tag_bytes.bin
```

## Notes
LIEF, the library which this tool uses, has a bug which corrupts the debuginfo 
of RISC-V ELF files when inserting sections. The submodule included here is
a patched version, though it's technically a band-aid fix which moves the
segment table to the back of the file, rather than fixing the (as of yet unidentified)
bug which actually causes the corruption.
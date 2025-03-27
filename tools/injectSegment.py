# NOTE: May be an issue, but I haven't run into any breakage yet: https://github.com/lief-project/LIEF/issues/1175

import lief
import sys
import argparse

parser = argparse.ArgumentParser(prog="elfInject")
parser.add_argument("input_elf")
parser.add_argument("output_elf")
parser.add_argument("-t", "--add-tag-section", 
    action="append",
    nargs=3,
    metavar=("name", "linked_name", "content"),
    help="Provide a new section name, the name of the section to link the tags to, and a path to a binary file containing the contents of the new section."
)

args = parser.parse_args()

target = lief.parse(args.input_elf)

section_map = {}
for i, sec in enumerate(target.sections):
   section_map[sec.name] = i

for section_args in args.add_tag_section:
    name, link_name, content_path = section_args

    # Find region past all already mapped regions
    vaddr = 0
    for section in target.sections:
        i = section.virtual_address + section.size
        if i > vaddr:
            vaddr = i

    section           = lief.ELF.Section()
    section.name      = name
    section.type      = lief.ELF.Section.TYPE.MIPS_AUXSYM
    # section.type      = lief.ELF.Section.TYPE.from_value(0x70000001)
    section.alignment = 8
    section.link      = section_map[link_name] 

    with open(content_path, "rb") as file:
        section.content = bytearray(file.read())

    section.size = len(section.content)
    if vaddr != 0:
        section.virtual_address = vaddr

    section = target.add(section, True)

    print("Injected section:")
    print(f"    File offset: 0x{section.file_offset:02x}")
    print(f"    Alignment:   0x{section.alignment:02x}")
    print(f"    Type:        {section.type}")
    print(f"    Flags:       {section.flags}")
    print(f"    Segments:")
    for seg in section.segments:
        print(f"        {seg}")

target.write(args.output_elf)

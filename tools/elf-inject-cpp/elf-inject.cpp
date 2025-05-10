#include "LIEF/ELF/Section.hpp"
#include <LIEF/ELF/Binary.hpp>
#include <LIEF/ELF/Parser.hpp>

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <map>

using namespace LIEF::ELF;

size_t first_available_vaddr(Binary const& elf) {
	uint64_t vaddr = 0;
	for(auto const& s : elf.segments()) {
		vaddr = std::max(vaddr, s.virtual_address() + s.virtual_size());
	}

	return vaddr;
}

int main(int argc, char** argv) {
	if(argc < 5) {
		printf("Usage: elf-inject <input_elf> <output> <section_name> <linked_section_name> <content_file>\n");
		exit(1);
	}

	std::string input_elf(argv[1]);
	std::string output_elf(argv[2]);
	std::string section_name(argv[3]);
	std::string linked_name(argv[4]);
	std::string content_path(argv[5]);

	auto elf = Parser::parse(input_elf);
	if(!elf) {
		fprintf(stderr, "Failed to parse elf file\n");
		return 1;
	}
	
	std::map<std::string, size_t> section_indicies;
	size_t i = 0;
	for(auto const& section : elf->sections()) {
		section_indicies.emplace(section.name(), i);
		i += 1;
	}

	Section section;
	section.name(section_name);
	section.type(static_cast<Section::TYPE>(0x700000000));
	section.alignment(0x1000);

	// Add the associated section's index into the 'link' field of the section.
	// Spike will use this to determine the base memory address on which to 
	// map our tag region.
	if(section_indicies.find(linked_name) != section_indicies.end())
		section.link(section_indicies.at(linked_name));

	std::vector<uint8_t> data;
	std::ifstream content_file(content_path);
	std::copy(
		std::istream_iterator<uint8_t>(content_file),
		std::istream_iterator<uint8_t>(), 
		std::back_inserter(data)
	);
	section.content(data);
	section.size(data.size());

	// Relocate the segment table to the end of the binary before calculating
	// the virtual address, as LIEF's automatic method likes to load the
	// new phdr table on top of our custom segments. 
	//elf->relocate_phdr_table(LIEF::ELF::Binary::BINARY_END);
	// NOTE: LIEF only reserves space for 10 user segments, so more than that
	// may cause the phdr table to leak over into another segment.
	section.virtual_address(first_available_vaddr(*elf));

	Section* new_section = elf->add(section, true);
	elf->write(output_elf);

	printf("Injected section:\n");
    printf("    File offset: 0x%#lx\n", new_section->file_offset());
    printf("    Alignment:   0x%#lx\n", new_section->alignment());
    printf("    Type:        0x%#lx\n", (unsigned long)new_section->type());
    printf("    Flags:       0x%#lx\n", new_section->flags());
    printf("    Segments:\n");
	for(auto const& seg : new_section->segments()) {
		printf("        File offset: 0x%#lx\n", seg.file_offset());
		printf("        Virt addr: 0x%#lx\n", seg.virtual_address());
		printf("        Virt size: 0x%#lx\n", seg.virtual_size());
		printf("       ---\n");
	}
    // for(auto const& seg : elf->segments())
    //     printf("        \n");
}

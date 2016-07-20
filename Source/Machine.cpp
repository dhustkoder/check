#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Utix/BaseTraits.h>
#include <Utix/ScopeExit.h>
#include "Instructions.h"
#include "Machine.h"

namespace gbx {




Machine* CreateMachine() {	

	auto* const mach = static_cast<Machine*>(malloc(sizeof(Machine)));

	if(!mach) {
		perror("can't allocate memory for Machine");
		return nullptr;
	}

	auto mach_cleanup = utix::MakeScopeExitIf([=]() noexcept {
			free(mach);
	});

	if(!mach->memory.Initialize())
		return nullptr;


	mach_cleanup.Cancel();
	return mach;
}




void DestroyMachine(Machine* const mach) {
	mach->memory.Dispose();
	free(mach);
}





bool Machine::LoadRom(const char* const rom_file_name) {

	FILE* const rom_file = fopen(rom_file_name, "r");

	if(!rom_file) {
		perror("Could not open rom file");
		return false;
	}

	const auto file_cleanup = utix::MakeScopeExit([=]() noexcept {
		fclose(rom_file);
	});	

	fseek(rom_file, 0, SEEK_END);
	const auto rom_size = static_cast<size_t>(ftell(rom_file));
	fseek(rom_file, 0, SEEK_SET);
	
	if(rom_size > MAX_CARTRIDGE_SIZE) {
		fprintf(stderr, "\'%s\' size is too big\n", rom_file_name);
		return false;
	}

	fread(this->memory.Data(), sizeof(uint8_t), rom_size, rom_file);

	if(ferror(rom_file)) {
		perror("error while reading from rom");
		return false;
	}

	this->m_rom_size = rom_size;
	cpu.SetPC(CARTRIDGE_ENTRY_POINT);
	cpu.SetSP(0xFFFE);
	cpu.SetAF(0x01B0);
	cpu.SetBC(0x0013);
	cpu.SetDE(0x00D8);
	cpu.SetHL(0x014D);

	return true;
}




bool Machine::StepMachine() {
	// fetch Opcode and execute instruction
	// uint8_t variable can't overflow main_instruction array
	const uint16_t pc = this->cpu.GetPC();
	printf("%X: ", pc);
	if(pc < CHAR_DATA_OFFSET) {
		const auto op = memory.ReadU8(pc);
		cpu.SetOP(op);
		main_instructions[op](this);
	} 
	else {
		fprintf(stderr, "PC overflows program memory\n");
		return false;
	}

	return true;
}
























}

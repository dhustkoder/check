#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <Utix/ScopeExit.h>
#include "Machine.h"


int Debug(gbx::Machine* const machine);

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "usage: %s <rom>\n", argv[0]);
		return EXIT_FAILURE;
	}

	gbx::Machine* const machine = gbx::CreateMachine();
	
	if(!machine)
		return EXIT_FAILURE;

	const auto machine_cleanup = utix::MakeScopeExit([=] {
		gbx::DestroyMachine(machine);
	});

	if(!machine->LoadRom(argv[1]))
		return EXIT_FAILURE;



	return Debug(machine);
}









int Debug(gbx::Machine* const machine) {
	printf("Select an option:\n"              \
	       "q - quit\n"                       \
	       "s - 1 step\n"                     \
	       "c - step until sig or pc break\n" \
	       "b - set pc break\n"               \
	       "r - print registers\n");

	char opt = 0;
	unsigned int pc_wanted = 0;
	
	while(true) {
		scanf(" %c", &opt);
		
		switch(opt)
		{
		
		case 's':
			if(!machine->Step()) 
				return EXIT_FAILURE;
			break;
			
		case 'c':
			while(machine->cpu.GetPC() != pc_wanted) {
				if(!machine->Step()) 
					return EXIT_FAILURE;
			}
			break;
		
		case 'b':
			printf("selected desired pc value (hex): ");
			scanf(" %x", &pc_wanted);
			printf("selected: 0x%x\n", pc_wanted);
			if(pc_wanted > gbx::HOME_MAX_OFFSET)
				fprintf(stderr, "warning pc_wanted oveflows HOME memory offsets\n");
			break;

		case 'r': 
			machine->cpu.ShowRegisters();
			break;

		case 'q':
			goto break_while;
		
		default:
			fprintf(stderr, "error, not opt \'%c\'\n", opt);
			break;
		}

	}

break_while:

	return EXIT_SUCCESS;
}






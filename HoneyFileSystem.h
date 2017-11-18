#include <iostream>
#include <stdlib.h>

#include "boot_record.h"
#include "directory_entry.h"
#include "fat_table.h"

class HoneyFileSystem{
	public:
		
		void zeroFilledFormat(FILE* file);
		void fastFormat(FILE* file);
		
};

void HoneyFileSystem::zeroFilledFormat( FILE* file ){
	
	
}

void HoneyFileSystem::fastFormat( FILE* file ){
	boot_record br;
	fat_table ft(br.numero_setores_fat, br.bytes_per_sector);
	directory_entry de;

	rewind(file);
	br.WritetoFile(file );
	ft.WritetoFile( file, br.bytes_per_sector);

	for( int i = 0; i < br.numero_entradas_diretorio; i++ ){
		de.WritetoFile(file);
	}
	
}

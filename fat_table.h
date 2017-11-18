#ifndef HONEY_FAT_TABLE
#define HONEY_FAT_TABLE

#include <stdio.h>
#include <stdlib.h>

class fat_table{
	public:
		unsigned int numero_setores;
	    unsigned short* tabela_FAT;
        
		fat_table( unsigned int numero_setores, unsigned short bytes_per_sector = 512 );
		void WritetoFile( FILE* file, unsigned short bytes_per_sector );
};

fat_table::fat_table( unsigned int numero_setores, unsigned short bytes_per_sector ){
	
	this->numero_setores = numero_setores;
	tabela_FAT = (unsigned short*) calloc( numero_setores*bytes_per_sector, sizeof(short) );
	
	//tabela_FAT[0] = 0xfff8;
	//tabela_FAT[1] = 0xfff8;
	
}

void fat_table::WritetoFile( FILE* file, unsigned short bytes_per_sector ){
	fwrite( this->tabela_FAT, sizeof(short), this->numero_setores*bytes_per_sector, file );
}

#endif
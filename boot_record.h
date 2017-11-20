#ifndef HONEY_BOOT_RECORD
#define HONEY_BOOT_RECORD

#include <stdio.h>

class __attribute__((__packed__)) boot_record{
	public:
		unsigned short bytes_per_sector;
		unsigned short numero_setores_por_cluster;
		unsigned short numero_setores_reservados;
		unsigned short numero_fat;
		unsigned short numero_entradas_diretorio;
		unsigned short numero_total_setores;
		unsigned short numero_setores_fat;
		unsigned short keep_power_of_two;

		boot_record( unsigned short bytes_per_sector = 512, unsigned short numero_setores_por_cluster = 128,
						unsigned short numero_setores_reservados = 1, unsigned short numero_fat = 1,
						unsigned short numero_entradas_diretorio = 2048, unsigned short numero_total_setores = 65535,
						unsigned short numero_setores_fat = 128){	
							
			this->bytes_per_sector = bytes_per_sector;
			this->numero_setores_por_cluster = numero_setores_por_cluster;
			this->numero_setores_reservados = numero_setores_reservados;
			this->numero_entradas_diretorio = numero_entradas_diretorio;
			this->numero_total_setores = numero_total_setores;
			this->numero_setores_fat = numero_setores_fat;
			this->numero_fat = numero_fat;
			this->keep_power_of_two = 0x3432;
		}

		void WritetoFile( FILE* file );

};

void boot_record::WritetoFile( FILE* file ){
	fwrite( this, 16, 1, file );
}

#endif
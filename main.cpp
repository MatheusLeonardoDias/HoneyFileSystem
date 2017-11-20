#include <iostream>
//~ #include <string.h>
//~ #include <stdlib.h>
using namespace std;

#include "boot_record.h"
#include "directory_entry.h"
#include "fat_table.h"
#include "HoneyFileSystem.h"

int main( int argc, char *argv[] ){
	
	
	
	/* Tratamento dos argumentos */
	//~ for(int i = 1; i <= argc; i++ ){
		//~ printf("%s\n", argv[i] );
		//~ if( strcmp(argv[i], "-total_setores" ) ){
			//~ i++;
			//~ char *p;
			//~ total_setores = strtol( argv[i], &p, 10);
		//~ }
	//~ }
	
	
	FILE* pendrive_ptr = fopen( "fatpreenchida.img", "wb+" );
	if( pendrive_ptr == NULL ){
		printf( "Deu Ruim\n");
		return -1;
	}
	
	HoneyFileSystem hfs;

	hfs.fastFormat( pendrive_ptr );
	
	FILE* input_file = fopen( "input.txt", "rb+" );
	if( input_file == NULL ){
		printf( "Deu Ruim mas foi quase\n" );
		return -1;
	}
	
	hfs.allocateFile( pendrive_ptr, 0, "seramds", "oii", input_file );

	fclose( input_file );

	FILE* output_file = fopen( "output.txt", "wb+" );
	if( output_file == NULL ){
		printf( "Deu Ruim mas foi quase\n" );
		return -1;
	}

	hfs.transferFile( pendrive_ptr, 0, 1, output_file );
	
	fclose(output_file);
	
	hfs.createSubDirectory( pendrive_ptr, 0, "novo" );

	string param = "/novo/agrvai.txt";

	printf("%d\n", hfs.findFolderCluster( pendrive_ptr, "novo", 0) );
	
	pair< unsigned short, unsigned short> ans = hfs.findFileCluster( pendrive_ptr, "seramds", "oii", 0);	
	printf("%d\n", ans.second );

	fclose(pendrive_ptr);
	
	return 0;
}

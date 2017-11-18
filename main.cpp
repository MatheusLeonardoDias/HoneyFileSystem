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
	}else{
		//~ while( !feof(pendrive_ptr) ){
			//~ int line = 1;
			//~ fread( &line, sizeof(int), 1, pendrive_ptr );
			//~ printf( "%d ", line );
			//~ fwrite( &br, sizeof(br), 1, pendrive_ptr );
	
			
		//~ }
		//~ fclose( pendrive_ptr );
		
		HoneyFileSystem hfs;

		hfs.fastFormat( pendrive_ptr );
		printf("uhul\n");
		
		
	}
		
	fclose(pendrive_ptr);
	
	return 0;
}

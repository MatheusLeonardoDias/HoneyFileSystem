#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "boot_record.h"
#include "directory_entry.h"
#include "fat_table.h"

class HoneyFileSystem{
	public:
		boot_record* br;
		fat_table* ft;
		unsigned int first_data_cluster;

		HoneyFileSystem();
		void zeroFilledFormat(FILE* file);
		void fastFormat(FILE* file);
		void allocateFile(FILE* file, unsigned short folder_cluster, string name, string extension, FILE* input);
		void transferFile(FILE* file, unsigned short file_cluster, unsigned short file_cluster_offset, FILE* output );
		unsigned short seekFreeCluster();
		void createSubDirectory( FILE* file, unsigned short folder_cluster, string name );
		int findFolderCluster( FILE* file, string name, unsigned short entry_cluster );
		pair<int,int> findFileCluster( FILE* file, string name, string extension, unsigned short entry_cluster );
		void listDirectory( FILE* file, unsigned short cluster );
		void treeListDirectory( FILE* file, unsigned short cluster, int offset );

};

HoneyFileSystem::HoneyFileSystem(){
	this->br = new boot_record;
	this->ft = new fat_table(br->numero_setores_fat, br->bytes_per_sector);
	this->first_data_cluster = ( ( br->numero_setores_fat * br->bytes_per_sector ) * 2 ) + 16;
}

void HoneyFileSystem::zeroFilledFormat( FILE* file ){
	boot_record br_format;
	fat_table ft_format(br_format.numero_setores_fat, br_format.bytes_per_sector);
	directory_entry de_format;

	rewind(file);
	br_format.WritetoFile(file );
	ft_format.WritetoFile( file, br_format.bytes_per_sector);
	
	// Para o primeiro setor do cluster
	de_format.name_file[0] = '.';
	de_format.attribute_file = 0x10;
	de_format.first_cluster = 0x0000;
	de_format.WritetoFile( file );
	for(int i = 32; i < br_format.bytes_per_sector; i+=32 ){
		directory_entry clean_entry;
		clean_entry.WritetoFile(file);
	}

	// Demais setores do primeiro cluster
	for( int sector = 1; sector < br_format.numero_setores_por_cluster; sector++ ){
		for(int i = 0; i < br_format.bytes_per_sector; i+=32 ){
			directory_entry clean_entry;
			clean_entry.WritetoFile(file);
		}
	}

	// Demais Clusters 
	for( int cluster = 1; cluster < (br_format.numero_total_setores*br_format.numero_setores_por_cluster); cluster++ ){
		printf( "Writing Cluster %d\n", cluster );
		for( int sector = 0; sector < br_format.numero_setores_por_cluster; sector++ ){
			for(int i = 0; i < br_format.bytes_per_sector; i+=32 ){
				directory_entry clean_entry;
				clean_entry.WritetoFile(file);
			}
		}
	}

	printf("Disk has been formated with the Zero Filled method!\n");
}

void HoneyFileSystem::fastFormat( FILE* file ){
	boot_record br_format;
	fat_table ft_format(br_format.numero_setores_fat, br_format.bytes_per_sector);
	directory_entry de_format;

	rewind(file);
	br_format.WritetoFile(file );
	ft_format.WritetoFile( file, br_format.bytes_per_sector);
	
	// Para o primeiro setor do cluster
	de_format.name_file[0] = '.';
	de_format.attribute_file = 0x10;
	de_format.first_cluster = 0x0000;
	de_format.WritetoFile( file );
	for(int i = 32; i < br_format.bytes_per_sector; i+=32 ){
		directory_entry clean_entry;
		clean_entry.WritetoFile(file);
	}

	// Demais setores do primeiro cluster
	for( int sector = 1; sector < br_format.numero_setores_por_cluster; sector++ ){
		for(int i = 0; i < br_format.bytes_per_sector; i+=32 ){
			directory_entry clean_entry;
			clean_entry.WritetoFile(file);
		}
	}

	printf("Disk has been formated with the Fast Format method!\n");
}

void HoneyFileSystem::allocateFile( FILE* file, unsigned short folder_cluster, string name, string extension, FILE* input ){
	
	if( name.size() > 12 || extension.size() > 3 ){
		printf("Name exceds the maximum permited by Honey File System. It will be limited to the first 12 caracters!\n" );
	}
	/* Marca na tabela FAT */
	fseek( file, 16, SEEK_SET );
	fread( this->ft->tabela_FAT, sizeof(short), 65536, file ); 
	unsigned short cluster = this->seekFreeCluster();
	fseek(input, 0L, SEEK_END);
	unsigned long input_size = ftell(input);
	this->ft->tabela_FAT[cluster] = 0xFFFF;
	/* */

	/* Cria a entrada de diretorio */
	directory_entry de;
	for( int i = 0; i < name.size() && i < 12; i++ ){
		de.name_file[i] = name[i];
	}
	for( int i = 0; i < extension.size() && i < 3; i++ ){
		de.extension[i] = extension[i];
	}
	de.first_cluster = cluster;
	de.attribute_file = 0x20;
	de.size_file = input_size;
	/*
		Include information of Date and Time
	*/

	fseek( file, this->first_data_cluster+(folder_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	unsigned char sector = -1;
	unsigned char sector_offset = -1;
	int find = 0;
	
	for( int actual_sector = 0; !find && actual_sector < this->br->numero_setores_por_cluster; actual_sector++ ){
		for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
			unsigned char buffer;
			fread( &buffer, 1, 1, file );
			fseek( file, 31, SEEK_CUR );
			if( buffer == 0x00 || buffer == 0xE5 ){
				sector = actual_sector;
				sector_offset = offset;
				find = 1;
				break;
			}
		}
	}

	if( !find ){
		printf( "File cannot be created, directory reach the maximum entrys!\n" );
		exit(-1);
	}

	fseek( file, this->first_data_cluster+(folder_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	fseek( file, (sector*this->br->bytes_per_sector)+sector_offset, SEEK_CUR);
	de.WritetoFile(file);
	/* */
	
	/* Grava os dados no cluster reservado */
	rewind(input);
	fseek( file, this->first_data_cluster+(cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	for( unsigned long i = 0; i < (unsigned long) (ceil((double)input_size/(double)(this->br->bytes_per_sector*this->br->numero_setores_por_cluster))); i++ ){
		
		while( !feof( input ) ){
			unsigned char buffer;
			fread( &buffer, 1, 1, input );
			fwrite( &buffer, 1, 1, file );
		}

		if( feof( input ) )
			break;

		unsigned short new_cluster = this->seekFreeCluster();
		this->ft->tabela_FAT[cluster] = new_cluster;
		this->ft->tabela_FAT[new_cluster] = 0xFFFF;
		cluster = new_cluster;

		fseek( file, this->first_data_cluster+(cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	}

	fseek(file, 16, SEEK_SET );
	this->ft->WritetoFile( file, this->br->bytes_per_sector );

}

unsigned short HoneyFileSystem::seekFreeCluster(){
	unsigned short cluster = -1;
	
	for( int offset = 0; offset < this->ft->numero_setores; offset++ ){
		unsigned short entry = this->ft->tabela_FAT[offset];
		if( entry == 0x0000 ){
			cluster = offset;
			break;
		}
	}

	if( cluster == -1 ){
		printf( "Disk is full!/n" );
		exit(-1);
	}

	return cluster;

}

void HoneyFileSystem::transferFile(FILE* file, unsigned short file_cluster, unsigned short file_cluster_offset, FILE* output){
	unsigned char* buffer;
	fseek( file, 16, SEEK_SET );
	fread( this->ft->tabela_FAT, sizeof(short), 65536, file );

	/* Le a directory entry */
	fseek( file, this->first_data_cluster+(file_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	fseek( file, file_cluster_offset, SEEK_CUR );
	directory_entry de;

	fread(de.name_file, 1, 12, file);
	fread(de.extension, 1, 3, file);
	fread(&de.attribute_file, sizeof(char), 1, file);
	fread(&de.creation_time, sizeof(short), 1, file );
	fread(&de.creation_date, sizeof(short), 1, file );
	fread(&de.last_acess_date, sizeof(short), 1, file );
	fread(&de.last_modification_time, sizeof(short), 1, file );
	fread(&de.last_modification_date, sizeof(short), 1, file );
	fread(&de.first_cluster, sizeof(short), 1, file );
	fread(&de.size_file, sizeof(int), 1, file );
	/* */

	fseek( file, this->first_data_cluster+(de.first_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET);
	int qtd_cluster = ceil((double)de.size_file/(double)(this->br->bytes_per_sector*this->br->numero_setores_por_cluster) );
	while( qtd_cluster > 0 ){
		int buffer_size = min( (int)de.size_file, (int)(this->br->bytes_per_sector*this->br->numero_setores_por_cluster) );
		buffer = (unsigned char*) calloc( buffer_size, sizeof(char) );
		de.size_file -= buffer_size;
		
		fread( buffer, buffer_size, 1, file );
		fwrite( buffer, buffer_size, 1, output );

		if(this->ft->tabela_FAT[de.first_cluster] == 0xFFFF)
			break;
	}
}


void HoneyFileSystem::createSubDirectory( FILE* file, unsigned short folder_cluster, string name ){
	if( name.size() > 12 ){
		printf("Name exceds the maximum permited by Honey File System. It will be limited to the first 12 caracters!\n" );
	}
	
	/* Marca na tabela FAT */
	fseek( file, 16, SEEK_SET );
	fread( this->ft->tabela_FAT, sizeof(short), 65536, file ); 
	
	unsigned short cluster = this->seekFreeCluster();
	this->ft->tabela_FAT[cluster] = 0xFFFF;
	/* */
	
	/* Cria a entrada de diretorio */
	directory_entry de;
	for( int i = 0; i < name.size() && i < 12; i++ ){
		de.name_file[i] = name[i];
	}
	de.first_cluster = cluster;
	de.attribute_file = 0x10;
	de.size_file = 0;
	/*
	Include information of Date and Time
	*/

	fseek( file, this->first_data_cluster+(folder_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	unsigned char sector = -1;
	unsigned char sector_offset = -1;
	int find = 0;
	
	for( int actual_sector = 0; !find && actual_sector < this->br->numero_setores_por_cluster; actual_sector++ ){
		for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
			unsigned char buffer;
			fread( &buffer, 1, 1, file );
			fseek( file, 31, SEEK_CUR );
			if( buffer == 0x00 || buffer == 0xE5 ){
				sector = actual_sector;
				sector_offset = offset;
				find = 1;
				break;
			}
		}
	}
	
	fseek( file, this->first_data_cluster+(folder_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	fseek( file, (sector*this->br->bytes_per_sector)+sector_offset, SEEK_CUR);
	de.WritetoFile(file);
	/* */
	
	/* Grava os dados no cluster reservado */
	fseek( file, this->first_data_cluster+(cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	de.name_file = (unsigned char*) calloc( 12, sizeof(char) );
	de.name_file[0] = '.';
	de.attribute_file = 0x10;
	de.first_cluster = cluster;
	de.WritetoFile(file);
	
	de.name_file = (unsigned char*) calloc( 12, sizeof(char) );
	de.name_file[0] = '.';
	de.name_file[1] = '.';
	de.attribute_file = 0x10;
	de.first_cluster = folder_cluster;
	de.WritetoFile(file);
	
	for(int i = 64; i < this->br->bytes_per_sector; i+=32 ){
		directory_entry clean_entry;
		clean_entry.WritetoFile(file);
	}

	for( int sector = 1; sector < this->br->numero_setores_por_cluster; sector++ ){
		for(int i = 0; i < this->br->bytes_per_sector; i+=32 ){
			directory_entry clean_entry;
			clean_entry.WritetoFile(file);
		}
	}


	fseek(file, 16, SEEK_SET );
	this->ft->WritetoFile( file, this->br->bytes_per_sector );
}

int HoneyFileSystem::findFolderCluster( FILE* file, string name, unsigned short entry_cluster ){
	if( name.size() > 12 ){
		printf("Name exceds the maximum permited by Honey File System. It will be limited to the first 12 caracters!\n" );
	}

	fseek( file, this->first_data_cluster+(entry_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
		directory_entry de;

		fread(de.name_file, 1, 12, file);
		fread(de.extension, 1, 3, file);
		fread(&de.attribute_file, sizeof(char), 1, file);
		fread(&de.creation_time, sizeof(short), 1, file );
		fread(&de.creation_date, sizeof(short), 1, file );
		fread(&de.last_acess_date, sizeof(short), 1, file );
		fread(&de.last_modification_time, sizeof(short), 1, file );
		fread(&de.last_modification_date, sizeof(short), 1, file );
		fread(&de.first_cluster, sizeof(short), 1, file );
		fread(&de.size_file, sizeof(int), 1, file );

		int flag = 0, i = 0;
		for( i = 0; i < name.size() && i < 12; i++ ){
			if( de.name_file[i] != name[i] ){
				flag = 1;
				break;
			}
		}

		if( !flag && ( i == 12 || de.name_file[i] == 0) && de.attribute_file == 0x10)
			return de.first_cluster;

	}

	//printf("Folder not found!\n");
	return -1;

}

pair<int,int> HoneyFileSystem::findFileCluster( FILE* file, string name, string extension, unsigned short entry_cluster ){
	if( name.size() > 12 ){
		printf("Name exceds the maximum permited by Honey File System. It will be limited to the first 12 caracters!\n" );
	}

	fseek( file, this->first_data_cluster+(entry_cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
		directory_entry de;

		fread(de.name_file, 1, 12, file);
		fread(de.extension, 1, 3, file);
		fread(&de.attribute_file, sizeof(char), 1, file);
		fread(&de.creation_time, sizeof(short), 1, file );
		fread(&de.creation_date, sizeof(short), 1, file );
		fread(&de.last_acess_date, sizeof(short), 1, file );
		fread(&de.last_modification_time, sizeof(short), 1, file );
		fread(&de.last_modification_date, sizeof(short), 1, file );
		fread(&de.first_cluster, sizeof(short), 1, file );
		fread(&de.size_file, sizeof(int), 1, file );
		
		pair< int, int> ans;
		int flag = 0, i = 0;
		for( i = 0; i < name.size() && i < 12; i++ ){
			if( de.name_file[i] != name[i] ){
				flag = 1;
				break;
			}
		}

		if( !flag && ( i == 12 || de.name_file[i] == 0) && de.attribute_file == 0x20){
			ans.first = entry_cluster;
			ans.second = offset;
		}else{
			continue;
		}

		flag = 0, i = 0;
		for( i = 0; i < extension.size() && i < 3; i++ ){
			if( de.extension[i] != extension[i] ){
				flag = 1;
				break;
			}
		}

		if( !flag && ( i == 3 || de.extension[i] == 0) && de.attribute_file == 0x20){
			return ans;
		}else{
			continue;
		}


	}

	printf("File not found!\n");
	return pair< int, int>( -1, -1); 
	exit(-1);

}

void HoneyFileSystem::listDirectory( FILE* file, unsigned short cluster ){
	fseek( file, this->first_data_cluster+(cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	int cont = 0;
	for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
		directory_entry de;

		fread(de.name_file, 1, 12, file);
		fread(de.extension, 1, 3, file);
		fread(&de.attribute_file, sizeof(char), 1, file);
		fread(&de.creation_time, sizeof(short), 1, file );
		fread(&de.creation_date, sizeof(short), 1, file );
		fread(&de.last_acess_date, sizeof(short), 1, file );
		fread(&de.last_modification_time, sizeof(short), 1, file );
		fread(&de.last_modification_date, sizeof(short), 1, file );
		fread(&de.first_cluster, sizeof(short), 1, file );
		fread(&de.size_file, sizeof(int), 1, file );

		if( de.name_file[0] == 0 ){
			printf("\n");
			return;
		}else if( de.name_file[0] == 0xE5 ){
			continue;
		}

		if( de.attribute_file == 0x10 ){
			printf("\x1b[32m %17s \x1b[0m", de.name_file );
		}else{
			printf("\x1b[34m %17s \x1b[0m", strcat( strcat( (char*) de.name_file, "."), (char*) de.extension ) );
		}	

		cont++;
		if( cont == 3 ){
			printf("\n");
			cont = 0;
		}
	}

	printf("\n");
}

void HoneyFileSystem::treeListDirectory( FILE* file, unsigned short cluster, int desloc ){
	fseek( file, this->first_data_cluster+(cluster*(this->br->bytes_per_sector*this->br->numero_setores_por_cluster)), SEEK_SET );
	int cont = 0;
	for( int offset = 0; offset < this->br->bytes_per_sector; offset+=32 ){
		directory_entry de;

		fread(de.name_file, 1, 12, file);
		fread(de.extension, 1, 3, file);
		fread(&de.attribute_file, sizeof(char), 1, file);
		fread(&de.creation_time, sizeof(short), 1, file );
		fread(&de.creation_date, sizeof(short), 1, file );
		fread(&de.last_acess_date, sizeof(short), 1, file );
		fread(&de.last_modification_time, sizeof(short), 1, file );
		fread(&de.last_modification_date, sizeof(short), 1, file );
		fread(&de.first_cluster, sizeof(short), 1, file );
		fread(&de.size_file, sizeof(int), 1, file );

		if( de.name_file[0] == 0 ){
			printf("\n");
			return;
		}else if( de.name_file[0] == 0xE5 || de.name_file[0] == '.' ){
			continue;
		}

		//printf("%d", desloc);
		if( de.attribute_file == 0x10 ){
			
			for( int i = 0; i < desloc; i++ )
				printf(" -> ");

			printf("\x1b[32m %s \x1b[0m\n", de.name_file );
			this->treeListDirectory( file, de.first_cluster, desloc+1);
		}else{
			for( int i = 0; i < desloc; i++ )
				printf(" -> ");

			printf("\x1b[34m %s \x1b[0m\n", strcat( strcat( (char*) de.name_file, "."), (char*) de.extension ) );
		}

	}

	//printf("\n");
}
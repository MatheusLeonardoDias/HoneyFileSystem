#ifndef HONEY_DIRECTORY_ENTRY
#define HONEY_DIRECTORY_ENTRY

#include <stdio.h>

class __attribute__((__packed__)) directory_entry{
	public:
		unsigned char* name_file;
		unsigned char* extension;
		unsigned char attribute_file;
		unsigned short creation_time;
		unsigned short creation_date;
		unsigned short last_acess_date;
		unsigned short last_modification_time;
		unsigned short last_modification_date;
		unsigned short first_cluster;
		unsigned int size_file;
		
		directory_entry(){

			this->name_file = (unsigned char*) calloc( 12, sizeof(char) );
			this->extension = (unsigned char*) calloc( 3, sizeof(char) );
			this->attribute_file = 0;
			this->creation_time = 0;
			this->creation_date = 0;
			this->last_acess_date = 0;
			this->last_modification_time = 0;
			this->last_modification_date = 0;
			this->first_cluster = 0;
			this->size_file = 0;
		}
		
		void WritetoFile( FILE* file );
};

void directory_entry::WritetoFile( FILE* file ){
	fwrite(this->name_file, 1, 12, file);
	fwrite(this->extension, 1, 3, file);
	fwrite(&this->attribute_file, sizeof(char), 1, file);
	fwrite(&this->creation_time, sizeof(short), 1, file );
	fwrite(&this->creation_date, sizeof(short), 1, file );
	fwrite(&this->last_acess_date, sizeof(short), 1, file );
	fwrite(&this->last_modification_time, sizeof(short), 1, file );
	fwrite(&this->last_modification_date, sizeof(short), 1, file );
	fwrite(&this->first_cluster, sizeof(short), 1, file );
	fwrite(&this->size_file, sizeof(int), 1, file );
}

#endif
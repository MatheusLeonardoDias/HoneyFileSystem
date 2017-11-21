#include <iostream>
#include <string.h>
//~ #include <string.h>
//~ #include <stdlib.h>
using namespace std;

#include "boot_record.h"
#include "directory_entry.h"
#include "fat_table.h"
#include "HoneyFileSystem.h"

void printErrorMessage( ){
	printf( "Wrong arguments!" );
}

int main( int argc, char *argv[] ){
	HoneyFileSystem hfs;
	FILE* pendrive_ptr;

	char* pendrive_name;
	if(argc > 1){
		pendrive_name = argv[1];
		pendrive_ptr = fopen( pendrive_name, "r+" );
		if( pendrive_ptr == NULL ){
			printf( "Deu Ruim\n");
			return -1;
		}
		rewind(pendrive_ptr);
	}else{
		printErrorMessage();
		exit(-1);
	}

	char* operation;	
	if( argc > 2 ){
		operation = argv[2];
		if( !strcmp( operation, "fastformat" ) ){
			pendrive_ptr = fopen( pendrive_name, "w+" );
			if( pendrive_ptr == NULL ){
				printf( "Deu Ruim\n");
				return -1;
			}

			hfs.fastFormat( pendrive_ptr );
		}else if( !strcmp( operation, "secureformat" ) ){
			pendrive_ptr = fopen( pendrive_name, "w+" );
			if( pendrive_ptr == NULL ){
				printf( "Deu Ruim\n");
				return -1;
			}

			hfs.zeroFilledFormat( pendrive_ptr );			
		}else if( !strcmp( operation, "ls" ) ){
			if( argc > 3 ){
				string file_name, file_ext;
				unsigned short cluster = -1; 
				char *token, *str, *tofree;
				
				tofree = str = strdup(argv[3]);  // We own str's memory now.
				while ((token = strsep(&str, "/"))){
					if( !strcmp( token, "" ) ){
						cluster = 0;
					}else{
						if( strstr( token, "." ) != NULL ){
							printErrorMessage();
							exit(-1);
						}else{
							int new_cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							if( new_cluster == -1 ){
								printf( "Directory not found!\n" );
								return -1;
							}else{
								cluster = new_cluster;
							}
							
						}
					}
				
				}
				
				hfs.listDirectory( pendrive_ptr, cluster );
				free(tofree);
			}else {
				printErrorMessage();
				return -1;
			}
		
		}else if( !strcmp( operation, "tree" ) ){
			if( argc > 3 ){
				string file_name, file_ext;
				unsigned short cluster = -1; 
				char *token, *str, *tofree;
				
				tofree = str = strdup(argv[3]);  // We own str's memory now.
				while ((token = strsep(&str, "/"))){
					if( !strcmp( token, "" ) ){
						cluster = 0;
					}else{
						if( strstr( token, "." ) != NULL ){
							printErrorMessage();
							exit(-1);
						}else{
							int new_cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							if( new_cluster == -1 ){
								printf( "Directory not found!\n" );
								return -1;
							}else{
								cluster = new_cluster;
							}
							
						}
					}
				
				}
				
				hfs.treeListDirectory( pendrive_ptr, cluster, 0 );
				free(tofree);
			}else {
				printErrorMessage();
				return -1;
			}
		}else if( !strcmp( operation, "createfile" ) ){
			if( argc > 4 ){
				string file_name, file_ext;
				unsigned short cluster = -1; 
				char *token, *str, *tofree;
				
				pair<int,int> cluster_data;

				tofree = str = strdup(argv[3]);  // We own str's memory now.
				while ((token = strsep(&str, "/"))){
					if( !strcmp( token, "" ) ){
						cluster = 0;
					}else{
						if( strstr( token, "." ) != NULL ){
							// const char *my_str_literal = "/novo/teste.txt";
							char *name = NULL, *extension = NULL;
							char *local_token, *local_str, *local_tofree;
							
							local_tofree = local_str = strdup(token);  // We own str's memory now.
							while ((local_token = strsep(&local_str, "."))){
								if( name == NULL ){
									name = local_token;
								}else{
									extension = local_token;									
									break;
								}
							}
							
							FILE* input = fopen( argv[4], "r+" );
							if( input == NULL ){
								printf( "Deu Ruim\n");
								return -1;
							}
							
							hfs.allocateFile(pendrive_ptr, cluster, string(name), string(extension), input );
							
							printf( "File created succesfully\n" );
							free(local_tofree);
							break;
						}else{
							int new_cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							if( new_cluster == -1 ){
								hfs.createSubDirectory( pendrive_ptr, cluster, string(token) );
								cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							}else{
								cluster = new_cluster;
							}
							
						}
					}
				
				}
			
				free(tofree);

			}else{
				printErrorMessage();
				exit(-1);
			}		
		}else if( !strcmp( operation, "createfolder" ) ){
			if( argc > 3 ){
				string file_name, file_ext;
				unsigned short cluster = -1; 
				char *token, *str, *tofree;
				
				tofree = str = strdup(argv[3]);  // We own str's memory now.
				while ((token = strsep(&str, "/"))){
					if( !strcmp( token, "" ) ){
						cluster = 0;
					}else{
						if( strstr( token, "." ) != NULL ){
							printErrorMessage();
							exit(-1);
						}else{
							int new_cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							if( new_cluster == -1 ){
								hfs.createSubDirectory( pendrive_ptr, cluster, string(token) );
								cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							}else{
								cluster = new_cluster;
							}
							
						}
					}
				
				}
			
				free(tofree);
			}else {
				printErrorMessage();
				return -1;
			}

		}else if( !strcmp( operation, "transferfile" ) ){
			if( argc > 4 ){
				string file_name, file_ext;
				unsigned short cluster = -1; 
				char *token, *str, *tofree;
				
				pair<int,int> cluster_data;

				tofree = str = strdup(argv[3]);  // We own str's memory now.
				while ((token = strsep(&str, "/"))){
					if( !strcmp( token, "" ) ){
						cluster = 0;
					}else{
						if( strstr( token, "." ) != NULL ){
							// const char *my_str_literal = "/novo/teste.txt";
							char *name = NULL, *extension = NULL;
							char *local_token, *local_str, *local_tofree;
							
							local_tofree = local_str = strdup(token);  // We own str's memory now.
							while ((local_token = strsep(&local_str, "."))){
								if( name == NULL ){
									name = local_token;
								}else{
									extension = local_token;									
									break;
								}
							}
							
							FILE* output = fopen( argv[4], "w+" );
							if( output == NULL ){
								printf( "Deu Ruim\n");
								return -1;
							}
							
							cluster_data = hfs.findFileCluster( pendrive_ptr, string(name), string(extension), cluster );
							hfs.transferFile(pendrive_ptr, cluster_data.first, cluster_data.second, output );
							
							fclose(output);
							printf( "File transfer succesfully\n" );
							free(local_tofree);
							break;
						}else{
							int new_cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							if( new_cluster == -1 ){
								hfs.createSubDirectory( pendrive_ptr, cluster, string(token) );
								cluster = hfs.findFolderCluster(pendrive_ptr, string(token), cluster );
							}else{
								cluster = new_cluster;
							}
							
						}
					}
				
				}
			
				free(tofree);

			}else{
				printErrorMessage();
				exit(-1);
			}

		}else if( !strcmp( operation, "deletefolder" ) ){
			
		}else if( !strcmp( operation, "deletefile" ) ){
			
		}

	}
	
	return 0;
}

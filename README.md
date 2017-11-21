# **HoneyFileSystem**

The Honey File System is a FAT16 based file system with academic purpose.

## Usage

  **Compile:**
```    
 g++ main.cpp -o main
```    
  
  **FastFormat:** Format the disk just writing the boot_record and the zero-filled FAT, and creates a root directory
``` 
    ./main [device or path to img file] fastformat
```
  **SecureFormat:** Format the disk writing the boot_record and the zero-filled FAT, and write zeros on all the clusters of the disk. This method could take a long while.
``` 
    ./main [device or path to img file] secureformat
```    
  **LS:** Show the list of files and directorys in the path specified, in columns format.
``` 
    ./main [device or path to img file] ls [Path in HoneyFileSystem]
```    
  **Tree:** Show all the tree of files and directorys under the Path specified.
``` 
    ./main [device or path to img file] tree [Path in HoneyFileSystem]
```
  **CreateFile:** Create a new file in the Honey File System based on file passed by input.
``` 
    ./main [device or path to img file] createfile [Path in HoneyFileSystem] [Path of input file(Host File System)]
```  
  **CreateFolder:** This method creates a new folder on the path specified. If some folder in the path doesn`t exist, creates the hole path of folders, allocating one-by-one in the respective free clusters.
``` 
    ./main [device or path to img file] createfolder [Path in HoneyFileSystem]
```
  **TransferFile:** Reads a file from the Honey File System and tranfer to the output file specified.
``` 
    ./main [device or path to img file] transferfile [Path in HoneyFileSystem] [Path of input file(Host File System)]
```
  **DeleteFile:** NOT IMPLEMENTED YET 
``` 
    ./main [device or path to img file] deletefile
```  
  **DeleteFolder:** NOT IMPLEMENTED YET
``` 
    ./main [device or path to img file] deletefolder
```

## Examples:
  
```
    ./main fat_example.img fastformat
    ./main fat_example.img createfolder /newfolder/anothersubdir
    ./main fat_example.img createfile /newfolder/anothersubdir/teste.txt input.txt
    ./main fat_example.img transferfile /newfolder/anothersubdir/teste.txt output.txt
    ./main fat_example.img tree /

```

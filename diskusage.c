#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ftw.h>
#define PATH_MAX_SIZE 4096
#define FILES_ARR_START_SIZE 10
typedef struct {
	char path[PATH_MAX_SIZE];
	size_t size;
}FileEntry;

FileEntry *files;
size_t current_index = 0;
size_t total_num = 0;

int function(const char* path, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
	if(current_index >= total_num){
		total_num *= 2;
		files = realloc(files, total_num*sizeof(FileEntry));
		if(files == NULL){
			perror("Error reallocating files");
			exit(-1);
		}
	}

	files[current_index].size = sb->st_size;
	strcpy(files[current_index].path, path);
	current_index++;
	return 0;
}

void init_file(){
	files = (FileEntry*)calloc(FILES_ARR_START_SIZE, sizeof(FileEntry));
	if(files == NULL){
		perror("FIles array could not be allocated!!");
		exit(-1);
	}
	total_num = FILES_ARR_START_SIZE;
}

void print_files(){
	int maxSize = find_max_filesize();
	
	for(size_t i = 0; i < current_index; ++i){
		printf("%ld kB (%ld)\t%s\n", files[i].size/1024, files[i].size, files[i].path);
		int num_blocks =20 * log10((float) files[i].size) /log10((float) maxSize);
		for(int j = 0; j < num_blocks; j++) printf("██");
		printf("\n");
		
	}
}

int cmp(void* first, void* second){
	FileEntry* a = (FileEntry*) first;
	FileEntry* b = (FileEntry*) second;

	return b->size - a->size;
}

int find_max_filesize(){
	size_t max = 0;
	for(size_t i = 0; i < current_index; ++i){
		if(files[i].size > max) 
			max = files[i].size;
	}
	return max;
}

int main(int argc, const char *argv[]){
	if(argc != 2) {
		printf("Usage: %s <path>\n", argv[0]);
		exit(-1);
	}

	init_file();
	nftw(argv[1], function, 5, 0);
	qsort(files, current_index, sizeof(FileEntry),&cmp);
	print_files();
}

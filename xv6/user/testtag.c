
#include "types.h"
#include "user.h"

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200

int
main(int argc, char *argv[]){
	int fd = open("ls", O_RDWR);
	tagFile(fd, "type", "history", 7);
	tagFile(fd, "author", "shangjiang", 10);

	char* key1 = "type";
  	char buffer1[18];
  	int res = getFileTag(fd, key1, buffer1, 18);

  	if(res <= 18){
   		printf(1, "got the tag: %s: %s\n", key1, buffer1); 
                                       
 	} else{
   		printf(1, "buffer too small.\n");
 	}

 	char* key2 = "author";
 	char buffer2[18];
 	res = getFileTag(fd, key2, buffer2, 18);

	if(res == -1){
		printf(1, "getFileTag: res is -1,something is wrong"); 
	}else if(res <= 18){
   		printf(1, "got the tag: %s: %s\n", key2, buffer2); 
                                       
 	} else{
   		printf(1, "buffer too small.\n");
 	}

 	res = removeFileTag(fd, key2);
 	if(res == -1){
		printf(1, "removeFileTag: res is -1,something is wrong"); 
	}else{
		printf(1, "removed the file tag");
	}

	char buffer3[18];

	res = getFileTag(fd, key2, buffer3, 18);

	if(res != -1){
		printf(1, "getFileTag: res is not -1,something is wrong"); 
	}else{
		printf(1, "getFileTag: tag is removed, cannot get the tag"); 
	}


 	close(fd);
 	exit();


}
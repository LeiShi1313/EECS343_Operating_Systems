
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
	close(fd);

	fd = open("ls", O_RDONLY);

	char* key = "type";
  char buffer[18];
  int res = getFileTag(fd, key, buffer, 18);

  if(res <= 18){
   	printf(1, "%s: %s\n", key, buffer); 
                                       
 	} else{
   	printf(1, "buffer too small.\n");
 	}

 	close(fd);
 	exit();


}
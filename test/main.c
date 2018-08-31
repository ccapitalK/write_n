#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>

char buf[256];
const char* str =  "Long string\n";

int main() {
    int fd = open("/dev/write6", O_WRONLY);
    if(fd < 0) {
        printf("Failed to open file\n");
        return 1;
    }
    int n = write(fd, str, strlen(str));
    printf("%d\n", n);
    return 0;
}

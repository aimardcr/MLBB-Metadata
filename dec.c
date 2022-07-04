#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void decrypt(uint8_t *buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] ^= 0xC3;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <input> <output>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    
    lseek(fd, 0, SEEK_END);
    int size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);

    uint8_t *data = malloc(size);
    read(fd, data, size);
    close(fd);

    if (*(int *)(data) == 0xA28C4102) {
        *(int *)(data) = 0xFAB11BAF;
        decrypt(data + 0x4, 268);
        
        for (int i = 0x0; i < 0x30; i += 0x8) {
            decrypt(data + *(int *)(data + i + 0x8), *(int *)(data + i + 0xC));
        }
        fd = open(argv[2], O_CREAT | O_WRONLY);
        write(fd, data, size);
        close(fd);
    } else {
        printf("Invalid header.\n");
    }
    free(data);
    return 0;
}

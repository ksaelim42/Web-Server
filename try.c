// #include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// #include <fcntl.h>
// # define	a 20
// # define	b 020
// # define	c 0x200
// # define	d 0x400

// int	main(void) {
// 	FILE	*file = fopen("./file.txt", "r");
// 	// int	fd = open ("./file.txt", O_RDONLY);
// 	printf("fd : %d\n", fileno(file));
// 	char	buffer[20];
// 	int	size = fread(buffer, 19, sizeof(buffer) - 2, file);
// 	buffer[19] = '\0';
// 	printf("size: %d\n", size);
// 	printf("%s\n", buffer);
// 	// printf("base-10: %d\n", a);
// 	// printf("base-8:  %d\n", b);
// 	// printf("200: %d\n", c);
// 	// printf("400:  %d\n", d);

// }
#include <stdio.h>

// int main() {
//     FILE *file;
//     char buffer[100];

//     // Open a file in binary read mode
//     file = fopen("file.txt", "r");

//     if (file == NULL) {
//         perror("Error opening file");
//         return 1;
//     }

//     // Read 10 elements of 10 bytes each from the file into the buffer
//     size_t elements_read = fread(buffer, sizeof(char), 10, file);

//     if (elements_read > 0) {
//         // Process the data in the buffer
//         printf("Read %zu elements: %s\n", elements_read, buffer);
//     } else {
//         perror("Error reading from file");
//     }


// 	int	fd = fileno(file);
// 	char	newbuf[20];
// 	read(fd, newbuf, 5);
// 	printf("read for sys call : %s\n", newbuf);

//     // Close the file
//     fclose(file);

//     return 0;
// }

// strtod : pEnd is pointer to the after of the last number value
#define SIZE 200
int	main(void) {
	char	*myarray[SIZE];
	size_t	a = 1024 * 1024 * 1024;
	a *= 8;
	for (int j = 0; j < SIZE; j++) {
		for (int i = 0; i < SIZE; i++) {
			myarray[i] = malloc(a);
			myarray[i][0] = 'A' + i;
		}
		usleep(50000);
	}
//	for (int i = 0; i < SIZE; i++) {
//		printf("%c: %p\n", myarray[i][0], &myarray[i][0]);
//		usleep(100000);
//	}
	printf("Success allocate memory\n");
	sleep(5);
}

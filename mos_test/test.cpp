#include <sys/select.h>
#include <stdio.h>

#define STDIN_FILENO 0

int main() {
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
	
	if (result == -1) {
		perror("select");
	} else if (result > 0) {
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			printf("Data is available for reading on stdin.\n");
			// Read data from stdin
		}
		// Check other sets for write and exceptional conditions if needed
	} else {
		printf("Timeout occurred.\n");
	}

	return 0;
}
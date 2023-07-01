#include <stdio.h>
#include "system.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <path-to-rom>\n", argv[0]);
		return 1;
	}

    return 0;
}


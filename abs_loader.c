#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    FILE *fp1;
    int i, j, hexaddr;
    char line[50], addr[10];

    fp1 = fopen("inp.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    while (fscanf(fp1, "%s", line) != EOF) {
        if (line[0] == 'T') {
            for (i = 1, j = 0; i <= 6; i++, j++)
                addr[j] = line[i];
            addr[j] = '\0';

            hexaddr = strtol(addr, NULL, 16); // Convert to hex number

            i = 9;
            while (line[i] != '\0') {
                printf("%x \t %c%c\n", hexaddr, line[i], line[i + 1]);
                hexaddr++;
                i += 2;
            }
        }
    }
    fclose(fp1);
    return 0; // End of the program
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    FILE *fp1;
    int i, j, hexaddr, start_hexaddr;
    char line[50], addr[10];

    // Open the input file
    fp1 = fopen("inp.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE; // Exit if file cannot be opened
    }

    printf("Enter the actual starting address: ");
    scanf("%x", &start_hexaddr);

    // Read each line from the file
    while (fscanf(fp1, "%s", line) != EOF) {
        if (line[0] == 'T') {
            for (i = 1, j = 0; i < 7; i++, j++)
                addr[j] = line[i];
            addr[j] = '\0';

            hexaddr = strtol(addr, NULL, 16); // Convert address from string to hex

            i = 9; // Start reading the data part of the line
            while (line[i] != '\0') {
                printf("%x \t %c%c\n", hexaddr + start_hexaddr, line[i], line[i + 1]);
                hexaddr += 1; // Increment address for the next byte
                i += 2; // Move to the next pair of characters
            }
        }
    }

    fclose(fp1); // Close the file
    return 0; // End of the program
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fpOpTab, *fpInput, *fpIntermediate, *fpSymtab, *fpLength;
int opTabSize = 0, symTabSize = 0, locctr = 0, startAddress = 0, totalSize = 0;

struct OPTAB {
    char label[30];
    char opcode[30];
} OT[50];

struct SYMTAB {
    char label[30];
    int addr;
} ST[50];

void readOpTab() {
    while (fscanf(fpOpTab, "%s %s", OT[opTabSize].label, OT[opTabSize].opcode) != EOF) {
        opTabSize++;
    }
}

int inOPTAB(char *opcode) {
    for (int i = 0; i < opTabSize; i++) {
        if (strcmp(OT[i].label, opcode) == 0) return 1;
    }
    return 0;
}

void readLine(char *label, char *opcode, char *operand) {
    char line[100];
    label[0] = opcode[0] = operand[0] = '\0';

    if (fgets(line, sizeof(line), fpInput) == NULL) {
        strcpy(opcode, "END");
        return;
    }

    line[strcspn(line, "\n")] = '\0';

    int count = sscanf(line, "%s %s %s", label, opcode, operand);

    if (count == 1) {
        strcpy(opcode, label);
        strcpy(label, "");
        strcpy(operand, "");
    } else if (count == 2) {
        strcpy(operand, opcode);
        strcpy(opcode, label);
        strcpy(label, "");
    }
}

int main() {
    char label[30], opcode[30], operand[30];

    fpOpTab = fopen("optab.txt", "r");
    fpInput = fopen("input.txt", "r");
    fpIntermediate = fopen("inter.txt", "w");
    fpSymtab = fopen("symtab.txt", "w");
    fpLength = fopen("length.txt", "w");

    if (!fpOpTab || !fpInput) {
        printf("Error: Missing input files.\n");
        return 1;
    }

    readOpTab();
    readLine(label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        startAddress = (int)strtol(operand, NULL, 16);
        locctr = startAddress;
        fprintf(fpIntermediate, "%x\t%s\t%s\t%s\n", locctr, label, opcode, operand);
        readLine(label, opcode, operand);
    } else {
        locctr = 0;
    }

    while (strcmp(opcode, "END") != 0) {
        for (int i = 0; i < symTabSize; i++) {
            if (strcmp(ST[i].label, label) == 0) {
                printf("Error: Duplicate label found -> %s\n", label);
                exit(0);
            }
        }

        if (strcmp(label, "") != 0) {
            strcpy(ST[symTabSize].label, label);
            ST[symTabSize].addr = locctr;
            symTabSize++;
        }

        fprintf(fpIntermediate, "%x\t%s\t%s\t%s\n", locctr, label, opcode, operand);

        if (inOPTAB(opcode)) {
            locctr += 3;
            totalSize += 3;
        } else if (strcmp(opcode, "WORD") == 0) {
            locctr += 3;
            totalSize += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            locctr += 3 * (atoi(operand));
        } else if (strcmp(opcode, "RESB") == 0) {
            locctr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            int len;
            if (operand[0] == 'C' || operand[0] == 'c') {
                len = strlen(operand) - 3;
            } else {
                len = (strlen(operand) - 3) / 2;
            }
            locctr += len;
            totalSize += len;
        }

        readLine(label, opcode, operand);
    }

    fprintf(fpIntermediate, "%x\t%s\t%s\t%s\n", locctr, label, opcode, operand);

    for (int i = 0; i < symTabSize; i++) {
        fprintf(fpSymtab, "%s\t%x\n", ST[i].label, ST[i].addr);
    }

    fprintf(fpLength, "%x\n%x", locctr - startAddress, totalSize);

    fclose(fpIntermediate);
    fclose(fpOpTab);
    fclose(fpInput);
    fclose(fpLength);
    fclose(fpSymtab);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function: Convert an integer to 2-digit hexadecimal string
void int_to_hex(int value, char *buffer) {
    sprintf(buffer, "%02X", value);
}

int main() {
    // Basic variables
    char addrStr[10], label[10], opcode[10], operand[10];
    char symbol[10], tempHex[10];
    char ch;

    int startAddr = 0, diff = 0, address = 0, prevAddr = 0;
    int len = 0, actualLen = 0, finalAddr = 0, symbolAddr = 0;

    // Mnemonics and corresponding opcode values
    char mnemonic[15][15] = {"LDA", "MUL", "STA", "LDCH", "STCH"};
    char machineCode[15][15] = {"00", "20", "0C", "53", "57"};

    // File pointers
    FILE *asmList, *symTab, *interFile, *objFile;

    // Open all required files
    asmList = fopen("assmlist.txt", "w");
    symTab  = fopen("symtab.txt", "r");
    interFile = fopen("intermed.txt", "r");
    objFile = fopen("objcode.txt", "w");

    if (!asmList || !symTab || !interFile || !objFile) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Find the final address from intermediate file
    fscanf(interFile, "%s%s%s", label, opcode, operand);
    while (strcmp(opcode, "END") != 0) {
        prevAddr = address;
        fscanf(interFile, "%x%s%s%s", &address, label, opcode, operand);
    }
    finalAddr = address;
    fclose(interFile);

    // Reopen intermediate file to start second pass
    interFile = fopen("intermed.txt", "r");
    fscanf(interFile, "%s%s%s", label, opcode, operand);

    // START directive
    if (strcmp(opcode, "START") == 0) {
        fprintf(asmList, "\t%s\t%s\t%s\n", label, opcode, operand);
        fprintf(objFile, "H^%s^00%s^00%x\n", label, operand, finalAddr);

        fscanf(interFile, "%x%s%s%s", &address, label, opcode, operand);
        startAddr = address;
        diff = prevAddr - startAddr;

        fprintf(objFile, "T^%02X^%02X", address, diff);
    }

    // Processing all lines until END
    while (strcmp(opcode, "END") != 0) {

        // ----- CASE 1: BYTE -----
        if (strcmp(opcode, "BYTE") == 0) {
            fprintf(asmList, "%x\t%s\t%s\t%s\t", address, label, opcode, operand);

            len = strlen(operand);
            actualLen = len - 3;  // Exclude C' and '

            fprintf(objFile, "^");
            for (int i = 2; i < actualLen + 2; i++) {
                int_to_hex((unsigned char)operand[i], tempHex);
                fprintf(asmList, "%s", tempHex);
                fprintf(objFile, "%s", tempHex);
            }
            fprintf(asmList, "\n");
        }

        // ----- CASE 2: WORD -----
        else if (strcmp(opcode, "WORD") == 0) {
            sprintf(addrStr, "%06d", atoi(operand));
            fprintf(asmList, "%x\t%s\t%s\t%s\t%s\n", address, label, opcode, operand, addrStr);
            fprintf(objFile, "^%s", addrStr);
        }

        // ----- CASE 3: RESB / RESW -----
        else if (strcmp(opcode, "RESB") == 0 || strcmp(opcode, "RESW") == 0) {
            fprintf(asmList, "%x\t%s\t%s\t%s\n", address, label, opcode, operand);
        }

        // ----- CASE 4: Mnemonic (Instruction) -----
        else {
            int j = 0;
            while (j < 15 && strcmp(opcode, mnemonic[j]) != 0)
                j++;

            // Special case: operand COPY
            if (strcmp(operand, "COPY") == 0) {
                fprintf(asmList, "%x\t%s\t%s\t%s\t%s0000\n",
                        address, label, opcode, operand, machineCode[j]);
            }
            // Normal instruction with symbol lookup
            else {
                rewind(symTab);
                while (fscanf(symTab, "%s%x", symbol, &symbolAddr) != EOF) {
                    if (strcmp(operand, symbol) == 0)
                        break;
                }
                fprintf(asmList, "%x\t%s\t%s\t%s\t%s%x\n",
                        address, label, opcode, operand, machineCode[j], symbolAddr);
                fprintf(objFile, "^%s%x", machineCode[j], symbolAddr);
            }
        }

        // Read next line
        fscanf(interFile, "%x%s%s%s", &address, label, opcode, operand);
    }

    // Write END record
    fprintf(asmList, "%x\t%s\t%s\t%s\n", address, label, opcode, operand);
    fprintf(objFile, "\nE^%02X", startAddr);

    printf("\n✅ Intermediate file -> Object code generated.\n");

    fclose(asmList);
    fclose(symTab);
    fclose(interFile);
    fclose(objFile);

    // --------- Display Section ---------
    printf("\nIntermediate file:\n");
    interFile = fopen("intermed.txt", "r");
    while ((ch = fgetc(interFile)) != EOF) putchar(ch);
    fclose(interFile);

    printf("\n\nSymbol Table:\n");
    symTab = fopen("symtab.txt", "r");
    while ((ch = fgetc(symTab)) != EOF) putchar(ch);
    fclose(symTab);

    printf("\n\nAssembler Listing:\n");
    asmList = fopen("assmlist.txt", "r");
    while ((ch = fgetc(asmList)) != EOF) putchar(ch);
    fclose(asmList);

    printf("\n\nObject Code:\n");
    objFile = fopen("objcode.txt", "r");
    while ((ch = fgetc(objFile)) != EOF) putchar(ch);
    fclose(objFile);

    return 0;
}
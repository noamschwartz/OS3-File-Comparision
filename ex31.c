//Noam Schwartz
//200160042
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int checkIfIdentical (FILE* file1, FILE* file2){
    int identical = 1;
    //get total number of bytes in the file.
    fseek(file1, 0, SEEK_END);
    long int fileOneIdent = ftell(file1);
    fseek(file2, 0, SEEK_END);
    long int fileTwoIdent = ftell(file1);
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);
    if (fileOneIdent != fileTwoIdent){
        return 0;
    }
    else{
        while ( !feof(file1)){
            if (fgetc(file1) != fgetc(file2)){
                identical = 0;
                break;
            }
        }
        if (!identical){
            return 0;
        }
        else{
            return 1;
        }
    }
}
FILE* copyFIleNoSpace(FILE* f, char* filename) {
    FILE *noSpacesFile;
    int buff;
    noSpacesFile = fopen(filename, "w+");
    if (noSpacesFile == NULL) {

        return NULL;
    }
    while ((buff = fgetc(f)) != EOF) {
        if ((buff != ' ') && (buff != '\n')) {
            fputc(buff, noSpacesFile);
        }
    }
    return noSpacesFile;
}


int checkIfSimiler(FILE* f1, FILE* f2) {
    int buffer1 = 0;
    int buffer2 = 0;
    int isSimilar = 1;
    rewind(f1);
    rewind(f2);
    while (1) {
        buffer1 = fgetc(f1);
        buffer2 = fgetc(f2);
        if ((buffer1 == EOF) || (buffer2 == EOF)) {
            break;
        }
        if ((buffer1 != buffer2) && ((buffer1 + 32) != buffer2) && (buffer1 != (buffer2 + 32))) {
            isSimilar = 0;
            break;
        }
    }
    if (((buffer1 == EOF) && (buffer2 != EOF)) || ((buffer1 != EOF) && (buffer2 == EOF))) {
        isSimilar = 0;
    }
    return isSimilar;
}




int main(int argc, char* argv[]) {
    char* file1 = argv[1];
    char* file2 = argv[2];
    FILE *fileOne;
    FILE *fileTwo;
    FILE *temp1;
    FILE *temp2;
    int dofilesExists = 1;
    //open file1
    fileOne = fopen(file1, "r");
    //check if file exists
    if (fileOne == NULL){
        dofilesExists = 0;
    }
    //open file 2
    fileTwo = fopen(file2, "r");
    //check if file exists
    if (fileTwo == NULL){
        fclose(fileOne);
        dofilesExists = 0;

    }

    if (dofilesExists){
        if (checkIfIdentical(fileOne, fileTwo)) {
            fclose(fileOne);
            fclose(fileTwo);
            return 1;
        }
        rewind(fileOne);
        rewind(fileTwo);
        temp1 = copyFIleNoSpace(fileOne, "noSpacesFile1.txt");
        temp2 = copyFIleNoSpace(fileTwo, "noSpacesFile2.txt");
        //rewind(temp1);
        //rewind(temp2);
        if (checkIfSimiler(temp1, temp2)) {
            fclose(fileOne);
            fclose(fileTwo);
            remove("noSpacesFile1.txt");
            remove("noSpacesFile2.txt");
            return 3;
        }
        else{
            fclose(fileOne);
            fclose(fileTwo);
            return 2;
        }
    }
    else {
        return 0;
    }
}



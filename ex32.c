//
// Noam Schwartz
// 200160042
//

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
/*
 * This is the struct that contains the students name and grade.
 */
typedef struct student {
    int grade;
    char student_name[150];
}student;
/*
 * This function gets the firectory path and creates an array of students.
 */
student* createArrayOfStudents (char* dir) {
    //allocate initial memory for the students.
    student *students = (student*) malloc(0);
    // Pointer for directory entry
    struct dirent *de;
    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(dir);
    // opendir returns NULL if couldn't open directory
    if (dr == NULL) {
        printf("Could not open student directory");
    }
    int i = 0;
    if (dr !=NULL){
        //while there are still more directories (students).
        while (de = readdir(dr)){
            if ((strcmp(de->d_name, "..") !=0) && (strcmp(de->d_name, ".")!=0)){
                //allocate more memory for a new student
                students = (student*) realloc(students, (i+1) * sizeof(student));
                student temp;
                //update student name.
                strcpy(temp.student_name, de->d_name);
                temp.grade = 0;
                students[i] = temp;
                i++;
            }
        }
        (void) closedir(dr);
    }
    return students;
}
/*
 * This function gets the c file name and compiles it.
 */

int compile(student *students,char* fileName, FILE* inpFile){
    int compiled = 0;
    int timeOut = 40;
    char compileLine[150]= "gcc -o a.out ./";
    strcat(compileLine, fileName);
    char * finalCompileLine = compileLine;
    //compile file
    system(finalCompileLine);
    //Spawn a child to run the program.
    pid_t pid=fork();
    if (pid==0) {
        //child process
        rewind(inpFile);
        //get the args from the unput file one by one.
       dup2(fileno(inpFile), STDIN_FILENO);
       //create a new file for the output.
       freopen("output.txt", "w", stdout);
       static char *argv[]={"./a.out",NULL};
       if ((execvp(argv[0], argv) < 0)){
           //compilationError
           exit(20);
       }
    }
    else {
        // pid!=0; parent process
        int childId;
        sleep(5);
        if (!waitpid(pid, &childId, WNOHANG)){
            kill(pid,0);
            return timeOut;
        }
        compiled = 1;
        //int exitStat = WEXITSTATUS(childId);
        if (WEXITSTATUS(childId) == 20){
            compiled = 0;
        }
    }
    return compiled;
}

/*
 * This function checks if the output of the students file matches the official results.
 */
int checkOutput(char* outputPath, char* myOutput ){
    strcat(myOutput, "/output.txt");
    chdir(getenv("HOME"));
    //create the output compile line (runs ex31)
    char *argv[4]={"./comp.out", myOutput,outputPath, NULL};
    //Spawn a child to run the program.
    pid_t pid=fork();
    if (pid==0) {
        // child process
        if ((execvp(argv[0], argv) < 0)){
            printf("Error in System call\n");
        }
    }
    else {
        // pid!=0; parent process
        int child_pid;
        // wait for child to exit
        waitpid(pid,&child_pid,0);
        if (WIFEXITED(child_pid)){
            return WEXITSTATUS(child_pid);
        }
    }
    return 0;


}
/*
 * This function unpdaes the studens grade according to the outputs matching result.
 */
void updateGrade(student *students, char* outputPath, char* myOutput){
    //get the result from ex31.
    int i = checkOutput(outputPath, myOutput);
    //update students grade. (compiled programs only)
    if (i == 1){
        students->grade=100;
    }
    if(i==3){
        students->grade=80;
    }
    if(i==2){
        students->grade=60;
    }
}
/*
 * This function searches the directory for a c file.
 */
int searchInDirectory(student *students, char *dirname, FILE* inputPath, char* outputPath){
    int didCompile = 0;
    DIR *dir;
    struct dirent *dirp;
    //open directory.
    dir=opendir(dirname);
    chdir(dirname);
    //while there are still directories not scanned.
    while((dirp=readdir(dir))!=NULL){
        int length = strlen(dirp->d_name);
        //directory type.
        if(dirp->d_type==4){
            //if the directory is "." or ".." ignore it.
            if(strcmp(dirp->d_name, ".")==0 || strcmp(dirp->d_name, "..")==0){
                continue;
            }
            //search inner directories with recursion.
            searchInDirectory(students,dirp->d_name, inputPath, outputPath);
        }
        //check if the spesific file is a c file.
        else if (strncmp(dirp->d_name + length - 2, ".c", 4) == 0){
            //c file found. compile it.
            didCompile = compile(students,dirp->d_name, inputPath);
            //no c file found.
            if (didCompile == 0) {
                students->grade=20;
                closedir(dir);
                return 1;
            }
            //timeout.
            if (didCompile ==40){
                students->grade=40;
            }
            //c file compiles succesfully.
            if (didCompile == 1){
                updateGrade(students, outputPath, dirname);
            }
            closedir(dir);
            return didCompile;
        }
    }
    chdir("..");
    closedir(dir);
    return didCompile;
}


/*
 * This function searches for a c file in the directory.
 */
int findCFile(student *students, char* dirPath, FILE* inputPath, char* outputPath){
    int isThereCFile = 0;
    char path[15000];
    strcpy(path, dirPath);
    //add file name to the path.
    strcat(path, "/");
    strcat(path, students->student_name);
    //check if the file exists.
    int doesExist = searchInDirectory(students, path, inputPath, outputPath);
    //no c file.
    if (doesExist ==0){
        //no c file in student directory
        printf("%s: %d\n", students->student_name, students->grade);
        students->grade = 0;
        return isThereCFile;
    }
    //file was found
    isThereCFile = 1;
    printf("%s: %d\n", students->student_name, students->grade);
    return isThereCFile;
}
/*
 * This function counts the number of students.
 */
int getNumberOfStudents(char* dirPath) {
    int numOfStudents = 0;
    DIR *d;
    struct dirent *dir;
    //open main directory
    d = opendir(dirPath);
    if (d) {
        //run through the directory and get the number of students.
        while ((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0){
                continue;
            }
            numOfStudents++;
        }
        closedir(d);
    }
    return numOfStudents;
}
/*
 * This function creates the results.csv file.
 */
void createCSVFile(student* students, char* directoryPath){
    int i = 0;
    //create the file in the home directory.
    chdir(getenv("HOME"));
    int numOfStudents = getNumberOfStudents(directoryPath);
    //create the file.
    FILE* results = fopen("results.csv", "w");
    if (results == NULL){
        printf("Error creating csv file.");
        return;
    }
    //run through all of the students and assien their reason to the grade.
    for (i ; i< numOfStudents; i++){
        student stud = students[i];
        char* reason = malloc(30);
        switch (stud.grade){
            case 0:
                strcpy(reason, "NO_C_FILE");
                break;
            case 20:
                strcpy(reason, "COMPILATION_ERROR");
                break;
            case 40:
                strcpy(reason, "TIMEOUT");
                break;
            case 60:
                strcpy(reason, "BAD_OUTPUT");
                break;
            case 80:
                strcpy(reason, "SIMILAR_OUTPUT");
                break;
            case 100:
                strcpy(reason, "GOOD_JOB");
                break;
        }
        //write the information to the file.
        fprintf(results, "%s,%d,%s\n", stud.student_name, stud.grade, reason);
        free(reason);
    }
    fclose(results);
}
/*
 * This is the main function of the program. It gets students assignments and grades them.
 */
int main(int argc, char* argv[]){
    int noCFile = 0;
    char* configPath = argv[1];
    FILE * configFile = fopen(configPath, "r");
    if (configFile == NULL){
        printf("Cannot open configuration file");
        return 0;
    }
    //break the three lines of the configurations file.
    char lineOne [150],lineTwo [150],lineThree [150];
    fgets(lineOne, sizeof(lineOne), configFile);
    //directory file path
    char* finalDirPath = lineOne;
    strtok(finalDirPath, "\n");
    fgets(lineTwo, sizeof(lineTwo), configFile);
    //input file path.
    char *finalInputFile = lineTwo;
    strtok(finalInputFile, "\n");
    fgets(lineThree, sizeof(lineThree), configFile);
    //output file path.
    char *finalOutputFile = lineThree;
    strtok(finalOutputFile, "\n");
    fclose(configFile);
    //create an array of the students.
    int i = 0;
    student* students = createArrayOfStudents(finalDirPath);
    int numOfStudents = getNumberOfStudents(finalDirPath);
    FILE * inpFile = fopen(finalInputFile, "r");
    while(i<numOfStudents){
        chdir(getenv("HOME"));
        noCFile = findCFile(&students[i], finalDirPath, inpFile, finalOutputFile);
        i++;
    }
    //create the csv file.
    createCSVFile(students, finalDirPath);
    free(students);
    fclose(inpFile);
    return 0;
}

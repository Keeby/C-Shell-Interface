   /*
   * Jaspar Deeb
   *
   * Basic shell. Acts as a simple command line interpretter for UNIX.
   * Handles metacharacters "<", ">", "|", "&", "#", "|&". See getword.c.
   *
   * Return values:
   * Does not return any specific values.
   *
   * '<' Will assume input data. '>' will assume output data.
   *
   * 
   *
   *
   */
   
   #include "p2.h"
   int terminator = 0;
   char newargv[STORAGE * 3]; // This *should* be a string array. Unfortunately, it's a character array which gets converted later. I've made it STORAGE *3 as a quick fix.
   int pointer = 0; /*not really a pointer */
   int num = 0;
   int redirection = 0;
   int counter = 0;
   int hasAmpersand = -1;
   int hasPipe = -1;
   int hasPipeAmpersand = -1;
   char* globalStringIn = NULL;
   char* globalStringOut = NULL;
   int firstVisit = 0;
   int pipeNum = 0;
   int pipeIndex[10];
   int pipeAndIndex[10];
   int argYes = 0;
   int mvFflag = 0;
   
   void sighandle() { /*Function for the signal handler. */
   }
   
   int main(int argc, const char* argv[]) {
     int execvpTest = 0;
     char* cdPointer;
     char* mvPointer;
     char* mvPointer2;
     pid_t forkid, second;
     globalStringIn = malloc(STORAGE);
     globalStringOut = malloc(STORAGE);
     setpgid(0,0);
     (void)signal(SIGTERM, sighandle);
     if(argc > 2) {
       fprintf(stderr, "Error: too many arguments.\n");
       _exit(9);
     }
     else if(argc == 2) {
       int fileTester = open(argv[1], O_RDONLY);
       argYes = 1;
       if(fileTester > 0) {
         dup2(fileTester, STDIN);
         close(fileTester);
       }
       else {
       perror("Error");
         _exit(9);
       }
     }
     for(;;) {
       int cdMv = 0;
       int i = 0;
       int z = 0;
       int x = 0;
       char* stringArray[MAXITEM];
       hasPipe = -1;
       newargv[0] = 0;
       globalStringIn = malloc(STORAGE);
       globalStringOut = malloc(STORAGE);
       memset(newargv, 0, 255);
       pointer = 0;
       terminator = 0;
       counter = 0;
       redirection = 0;
       hasPipeAmpersand = -1;
       hasAmpersand = -1;
       memset(pipeIndex, 0, 10);
       memset(pipeAndIndex, 0, 10);
       pipeNum = 0;
       mvFflag = 0;
       if(argYes != 1)
         printf("p2: ");
       parse();
       if(newargv[0] == EOF) {
           break;
         }
       
       /**********************cd scection*************************/
       
       if(strcmp(newargv, "cd") == 0) {
        cdMv = 1;
        cdPointer = newargv + 3;
        if(cdPointer[0] == 0) {
          chdir(getenv("HOME")); /*goes to home in the buf*/
        }
        i = 0;
        while(cdPointer[i] != 0) {
          i++;
        }
        if(cdPointer[i+1] != 0) {
          fprintf(stderr, "Error: Too many arguments.\n");
        }
        else if(cdPointer[0] != 0){
          chdir(cdPointer); /*should change the directory*/
        }
      }
      /***********************mv section**************************/
     
      if(strcmp(newargv, "MV") == 0) {
        char* directName1;
        char* buffer;
        cdMv = 1;
        mvPointer = newargv + 3;
        if(mvPointer[0] == 0) {
          fprintf(stderr, "Error: MV: not enough arguments.\n");
          continue;
        }
        else {
          i = 0;
          while(mvPointer[i] != 0) {
            i++;
          }
          i++;
          mvPointer2 = mvPointer + i;
          i = 0;
          if(mvPointer2[i] == 0) { /*same code as above, optimize later I suppose*/
            fprintf(stderr, "Error: only one argument passed.\n");
            continue;
          }
          else {
            while(mvPointer2[i] != 0) {
            i++;
            }
            if(mvPointer2[i+1] != 0) {
                fprintf(stderr, "Error: Too many arguments.\n");
                continue;
            }
            /*From below, we'll look at different cases of a 2 word mv.*/
            else if(mvPointer2[0] != 0) {
              int fileTester = open(mvPointer, O_RDONLY);
              int fileTester2 = open(mvPointer2, O_RDONLY);
              DIR* dirTester = opendir(mvPointer2);
              if(fileTester >= 0) {
                /* 1st file exists */
                if(fileTester2 >= 0) { //if both files exist
               
                  if(dirTester != NULL) {
                    directName1 = realpath(mvPointer, buffer); //takes the directory of the first file         
                    strcat(mvPointer2, "/"); //adds a / to the directory of the 2nd word
                    strcat(mvPointer2, mvPointer); //adds the file name to the directory
                    link(directName1, mvPointer2); //the file is now in the directory
                    unlink(directName1); //file is deleted form original directory
                  }
                  else {
                    if(mvFflag == 1) {
                      if(strcmp(mvPointer, mvPointer2) == 0) {
                        fprintf(stderr, "Error: same file\n");
                        continue;
                      }
                      unlink(mvPointer2);
                      link(mvPointer, mvPointer2);
                      unlink(mvPointer);
                    }
                    else {
                      fprintf(stderr, "Error: %s already exists.\n", mvPointer2);
                      continue;
                    }
                  }
                } //Only first file exists
                  else {
                    link(mvPointer, mvPointer2);
                    unlink(mvPointer);
                  }
                }
              }
              else {
                perror("Error");
                continue;
              }
            }
          }
        }
     
      /*****************forking to child section********************/
      while(z < counter - 1) {
        int pipeCounter = 0;
        stringArray[z] = newargv + z + x; //x is word length total, while z is the space count total.
             
        for(pipeCounter = 0; pipeCounter <= pipeNum; pipeCounter++) {       

          if(pipeIndex[pipeCounter] == z && pipeIndex[pipeCounter++] != 0) {
//nulls where the pipe is.
            stringArray[z] = NULL;
            }
        }
        x = x + (int)strlen(newargv + z + x);
        z++;
      }
      stringArray[z] = 0;
      if(newargv[0] != 0 && terminator == 1 && cdMv == 0) { /*make sure that there aren't 0 words*/
        fflush(stderr);
        fflush(stdout);
        if((forkid = fork()) == 0) { /*everything in this if statement will be the CHILD.*/
          if(redirection == 0 && hasPipe < 0) { /*We have no redirection symbols, then it will be a regular command.*/
            int execvpTest = execvp(newargv, stringArray);
            if(execvpTest < 0) {
              perror("Error");
              _exit(9);
            }
          }
          /*section of redirection*/
          else {
            if(globalStringIn[0] != '\0') { /* '<' used */
              int fileTester = open(globalStringIn, O_RDONLY);
              if(fileTester > 0) {
                dup2(fileTester, STDIN);
                close(fileTester);
              }
              else {
                perror("Error");
                _exit(9);
              }
            }
            if(globalStringOut[0] != '\0') { /*  '>' used */
              int fileTester = open(globalStringOut, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
              if(fileTester < 0) { /*any error */
                perror("Error");
                _exit(9);
              }
                dup2(fileTester, STDOUT);
                close(fileTester);
            }
          }
            if(hasPipe >= 0) { //child
              int filedesc[pipeNum * 2]; // File descriptor for each end of number of pipes.
              pipe(filedesc);
              fflush(stdout);
              fflush(stderr);
              if((second = fork()) == 0) {//grandchild
                if(pipeNum > 1) { //is there more than one pipe?
                    int i = 0;
                    for(i = 1; i < pipeNum; i++) { //start at one since we pipe at 0 at the beginning.
                      pipe(filedesc + i * 2);
                      if(fork() == 0) {
                        if(pipeNum == i + 1) { // The youngest child, the great great... grandchild
                          dup2(filedesc[pipeNum * 2 - 1], STDOUT);
                          if(pipeAndIndex[pipeNum - i - 1] == pipeIndex[pipeNum - i - 1]) {
                            dup2(filedesc[pipeNum * 2 - 1], 2);
                          }
                          close(filedesc[pipeNum * 2 - 1]);
                          close(filedesc[pipeNum * 2 - 2]);
                          execvp(stringArray[0], stringArray);
                        }
                      }
                      else { //All other middle children.
                      dup2(filedesc[i * 2], STDIN);
                      dup2(filedesc[i * 2 - 1], STDOUT);
                      if(pipeAndIndex[pipeNum - i + 1] == pipeIndex[pipeNum - i + 1]){
                        dup2(filedesc[i * 2 - 1], 2);
                      }
                      close(filedesc[i * 2 - 2]);
                      close(filedesc[i * 2 - 1]);
                      close(filedesc[i * 2]);
                      close(filedesc[i * 2 + 1]);
                      execvpTest = execvp(stringArray[pipeIndex[pipeNum - i - 1] + 1], stringArray + pipeIndex[pipeNum - i - 1] + 1);
                      //we need to start at the first pipe index, which is i- 1 (zero), since we started here with i = 1...
                      //Also, it needs to descend down since piping goes from right to left.
                      if(execvpTest < 0) {
                        perror("Error");
                        _exit(9);
                      }
                    }
                  }
                }
                else { //else for pipenum > 1, if it's just one pipe we handle it the same way as p2
                  dup2(filedesc[1], STDOUT);
                  if(pipeAndIndex[0] == pipeIndex[0])
                    dup2(filedesc[1], 2);
                  close(filedesc[0]);
                  close(filedesc[1]);
                  execvpTest = execvp(stringArray[0], stringArray);
                  if(execvpTest < 0) { 
                    perror("Error");
                    _exit(9);
                  }
                }
              }
              else { //child
                dup2(filedesc[0], STDIN);
                if(pipeAndIndex[0] == pipeIndex[0])
                    dup2(filedesc[0], 2);                 
                close(filedesc[0]);
                close(filedesc[1]);
                if(pipeNum <= 1) {       
                  execvpTest = execvp(stringArray[pipeIndex[0] + 1], stringArray + pipeIndex[0] + 1);
                }
                else {
                  execvpTest = execvp(stringArray[pipeIndex[pipeNum - 1] +1], stringArray + pipeIndex[pipeNum - 1] + 1);
                }
                  if(execvpTest < 0) {
                    perror("Error");
                    _exit(9);
                  }
              }                                           
            }
            execvpTest = execvp(newargv, stringArray);
            if(execvpTest < 0) {
              perror("Error");
              _exit(9);
            }         
        }
        else if(hasAmpersand < 0) {
          for(;;) {
            pid_t pid = wait(NULL);
            if(pid == forkid) break;
          }
        }
        else if(hasAmpersand > 0) {
          printf("%s [%d]\n", newargv, forkid);
          hasAmpersand = -1;
        }
      }
    }
   
    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n");
    exit(0);
   
  }
 
  void parse() {
    int pound = 0;
    int notAgain = 0;
    int firstMv = 0; 
    for(;;) {
      int j = 0;
      int localRedirection = 0;
      int testnum = 0; 
      int fileTester = 0;           
     
      char firstArray[STORAGE];
      num = getword(firstArray);
      if(firstMv == 1) {
        if(strcmp(firstArray, "-f") == 0) {
          mvFflag = 1;
          localRedirection = 1;
        }
        if(strcmp(firstArray, "-n") == 0) {
          localRedirection = 1;
        }
      }
      if(num == -1 || num == -2) {
        localRedirection = 1;
        if(firstArray[0] == 38) {
          terminator = 1;
          hasAmpersand = 1;
          counter++;
          break;
        }
        redirection = 1;
        if(firstArray[0] == '#') {
          pound = 1;
          if(notAgain == 0) {
            counter++;
            notAgain = 1;
          }
          if(newargv[0] != 0 && argYes == 0) { // not a comment!
            pound = 0;
            redirection = 0;
            localRedirection = 0;
            num = 1;
            counter++;
          }         
        }                 
        if(firstArray[0] == '<') {
          if(globalStringIn[0] != '\0') {
            fprintf(stderr, "Ambiguous input redirect.\n");
            break;
          }
          testnum = getword(firstArray);
          if(testnum > 0) {
            strcpy(globalStringIn, firstArray);
            fileTester = open(globalStringIn, O_RDONLY);
            if(fileTester < 0) {
              fprintf(stderr, "Error: File does not exist.\n");
              close(fileTester);
              break;
            }
          }
          else {
            fprintf(stderr, "Error: Please put a word in front '<'\n");
            break;         
          }
        }
        if(firstArray[0] == '>') {
          testnum = getword(firstArray);
          if(testnum > 0) {
            strcpy(globalStringOut, firstArray);
            fileTester = open(globalStringOut, O_RDONLY);
            if(fileTester > 0) {
              fprintf(stderr, "Error: File already exists.\n");
              close(fileTester);
              break;
            }
          }
          else {
           fprintf(stderr, "Error: Please put a word in front of '>'\n");
           break;
          }
        }
        if(firstArray[0] == '|') {
          pipeNum++;
          if(pipeNum > 10) {
            fprintf(stderr, "Error: too many pipes\n");
            pipeNum = 0;         
            break;
          }
          if(pipeNum > 1) {
           pipeIndex[pipeNum - 1] = counter++;
            if(firstArray[1] == '&') {
              pipeAndIndex[pipeNum - 1] = pipeIndex[pipeNum - 1];
            }
            localRedirection = 0;         
          }
          else {       
            hasPipe = 1;
            pipeIndex[pipeNum - 1] = counter++;   
            if(firstArray[1] == '&') {
              pipeAndIndex[pipeNum - 1] = pipeIndex[pipeNum - 1];
            }
            localRedirection = 0;
          }
        }
      }
      else if(num != -10 || num != 0) { /*counts the number of words */
        if(pound == 0)
         counter++;
      }
      if(num == -10) {
        terminator = 1;
        break; /* we'll come back in p2 */
      }
      if(num == 0) {
        terminator = 0;
        newargv[0] = EOF;
        break;
      }
      if(pound == 0) {
        if(firstMv == 1 && strcmp(firstArray, "-f") == 0) {
          mvFflag = 1;
          continue;
        }
        if(firstMv == 1 && strcmp(firstArray, "-n") == 0) { //if the n command is used after the f command, then it will be used normally (get rid of flag).
          mvFflag = 0;
          continue;
        }
        for(j = 0; j < num; j++)  {
          newargv[pointer++] = firstArray[j];
        }
      }
      if(localRedirection == 0) {
        pointer++; /*add one to account for a space */
      }
      if(strcmp(newargv, "MV") == 0) {
        firstMv = 1;
      }
    }
  }

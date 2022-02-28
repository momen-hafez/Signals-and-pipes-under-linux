#ifndef __LOCAL_H__
#define __LOCAL_H__

/* Coloring Codes */

#define RED    printf("\033[0;31m");
#define GREEN  printf("\033[0;32m");
#define YELLOW printf("\033[0;33m");
#define BLUE   printf("\033[0;34m");
#define WHITE  printf("\033[0;37m");


/* Required includes */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

enum Processes {P1 , P2 , REFEREE};
char rounds[30][10] = {"One" , "Two" , "Three" , "Four" , "Five" , "Six" , "Seven" , "Eight" , "Nine" , "Ten", "Eleven" , "Twelve"};

#endif

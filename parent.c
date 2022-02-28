
#include "local.h"

/* Global variables for the total score for each child and number of rounds */
int bigScore1   = 0;
int bigScore2   = 0;
int numOfRounds = 0;

/* Global variables to save the PIDs for the processes, P1, P2 and R */
int Pr1 , Pr2 , Re;
int cP1 , cP2 , Ref;

/* to determine if the child is P1 or P2 */
int determineProcess;

int main(){
  /* functions prototype */
  void signal_catcher_sigint (int);   // SIGINT handler
  void signal_catcher_sigquit(int);   // SIGQUIT handler
  void update_global_scores  (char*); // Update the global variables at each round
  void determine_winner      ();      // determining the winner
  void print_until_now       ();      // results until now
  
  /* 
  file descriptor for the pipes, notice that it's a 2 dimensional array, As we have three pipes 
  as the following:
  Parent <----> P1 ===> Read fd[0][0] and Write fd[0][1]
  Parent <----> P2 ===> Read fd[1][0] and Write fd[1][1]
  Parent <----> R  ===> Read fd[2][0] and Write fd[2][1]
  and each pipe has read and write, so in total we have 6 read and write for the 3 pipes 
  */
  
  int fd[3][2];
  
  /* 
  these strings are used to store the 6 values of the file descriptor to send it
  to P0, P1 and R, as the files for children is implemented in seperated files
  and we send these parameters to them using execl(...)
  */
  char sendRead1 [10];
  char sendWrite1[10];
  char sendRead2 [10];
  char sendWrite2[10];
  char sendRead3 [10];
  char sendWrite3[10];
  
  /* 
  We combine P0 and P1 in a single file because they have almost similar code, we have to distinguish between them
  so we add a new parameter to send it through execl(...) --> ChildP1 refers to P1, and ChildP2 refers to P2
  */
  char ChildP1[2];
  char ChildP2[2];

  /* strings to read messages or sent messages through the pipe, as well as the score message */
  static char message[BUFSIZ];
  static char score_result[BUFSIZ];
  
  /* 
  indication for the output color, see "local.h" to see the actual code for the colors, 
  we use #define there to make the code more readable 
  */
  BLUE; 
  printf("\n\n\n		<--------- THE GAME IS START -------->\n\n");
  WHITE;
  /* here we open the pipes and check that there's no errors */
  for(int i = 0 ; i < 3 ; i++){
    if(pipe(fd[i]) < 0){
      perror("Pipe");
      return -1;
    }
  }
  /* waiting for the signal SIGQUIT */
  if (signal(SIGQUIT, signal_catcher_sigquit) == SIG_ERR) {
    perror("handle error");
    return -2;
  }
   /* waiting for the signal SIGINT */
  if (signal(SIGINT, signal_catcher_sigint) == SIG_ERR) {
     perror("handle error");
     return -13;
  }

  printf("--> ROUND : %s --------------------------------------------\n",rounds[0]);
  YELLOW;
  /* Start forking and having children ...... */
  
  /* P1 creation .. */
  switch(Pr1 = fork()){
    case -1: // in case of any errors. 
       perror("Fork");
       return -3;
    case 0: // P1 child code
       sprintf(sendRead1 , "%d", fd[0][0]); /*fd[0][0] read between the parent and P1*/
       sprintf(sendWrite1, "%d", fd[0][1]); /*fd[0][1] write between the parent and P1*/
       /*determining that this code is for P1 not P2, P1 and P2 declared as enum in local.h*/
       sprintf(ChildP1,    "%d", P1      ); 
       /* execute P1 code, by sending the read and write descriptors for his pipe and a determiner for the child type */
       execlp("./childs", "childs", sendRead1, sendWrite1, ChildP1,(char *)0);
       /* in case of any error in execution */
       perror("Exec ");       
       return -4;
    default: // parent code
       cP1 = Pr1; // saving P1 PID
       sleep(2);    
       /* Parent ask P1 to generate 10 numbers in range [1 .. 100] and store them in child1.txt using signal SIGUSR1 */ 
       GREEN;
       printf("Hi P1, Please generate 10 numbers in range[1..100]and store them in child1.txt\n");
       YELLOW;
       kill(cP1 , SIGUSR1);
       /* P1 sends a message to the parent informing him that the file child1.txt is READY!*/
       if(read(fd[0][0] , message , BUFSIZ) != -1){
         /* Once the parent read the message, he informs that P1 is ready by sending a signal SIGINT which
         print that which Processes from P1 and P2 have been finished his file */
         determineProcess = P1;
         /* getpid() = Parent PID */
         kill(getpid() , SIGINT);
       }else{
         /* in case of any errors in reading */
         perror("read");
         return -5;
       }
   }
  sleep(1);
  /* P2 creation */
  switch(Pr2 = fork()){
    case -1:// in case of any errors in forking
       perror("Fork");
       return -6;
    case 0: // P2 code ..
       sprintf(sendRead2 , "%d", fd[1][0]); /*fd[1][0] read between the parent and P2*/
       sprintf(sendWrite2, "%d", fd[1][1]); /*fd[1][1] read between the parent and P2*/
       /*determining that this code is for P2 not P1, P1 and P2 declared as enum in local.h*/
       sprintf(ChildP2,    "%d", P2      );
       /* execute P2 code, by sending the read and write descriptors for his pipe and a determiner for the child type */
       execl("./childs", "childs", sendRead2, sendWrite2, ChildP2 , (char *)0);
       perror("Exec ");
       return -7;
    default:// The parent code .. 
       cP2 = Pr2; // Saving P2 PID
       sleep(2);
       /* Parent ask P2 to generate 10 numbers in range [1 .. 100] and store them in child2.txt using signal SIGUSR1 */ 
       GREEN;
       printf("Hi P2, Please generate 10 numbers in range[1..100]and store them in child2.txt\n");
       YELLOW;
       kill(cP2 , SIGUSR1);
       /* P2 sends a message to the parent informing him that the file child2.txt is READY!*/
       if(read(fd[1][0] , message , BUFSIZ) != -1){
          /* Once the parent read the message, he informs that P2 is ready by sending a signal SIGINT which
          print that which Processes from P1 and P2 have been finished his file */
          determineProcess = P2;
          /* getpid() = Parent PID */
          kill(getpid() ,SIGINT); 
       }else{
         // incase of any errors in reading
         perror("read");
         return -8;
       }
  } 
  sleep(1);
  /* R creation */
  switch(Re = fork()){
    case -1: // in case of any errors in creation
       perror("Fork");
       return -9;
    case 0: // R code .. 
       sprintf(sendRead3 , "%d", fd[2][0]); /*fd[2][0] read between the parent and R */
       sprintf(sendWrite3, "%d", fd[2][1]); /*fd[2][1] write between the parent and R */
       /* execute R code, by sending the read and write descriptors for his pipe */
       execl("./Referee", "Referee", sendRead3, sendWrite3 , (char *)0);
       perror("Exec ");
       return -10;
    default: // Parent code ..
       Ref = Re; // Saving R PID
       sleep(2);
       /* 
       The parent ask R to start manipulating and comparing the files after sending child1.txt-child2.txt message
       through a pipe, when the parent sent a signal SIGUSR1 to R, R will wait for the parent to write in the pipe 
       child1.txt-child2.txt message, after that, it compares the result and storing the score to return it to the parent
       */
       GREEN;
       printf("Hi R, Please take the two files and return the result to me\n");
       YELLOW;
       kill(Ref , SIGUSR1); // here, R will wait for the message to be written 
       strcpy(message , "child1.txt - child2.txt");
       // once the message is written, R will statr his manipulation .. 
       if(write(fd[2][1] , message , strlen(message)) == -1){
          // in case of any errors in writing
          perror("write");
          return -11;
        }
       sleep(2);
       /* Here the parent read the score .. score1-score2 ... from R as a string */
       if(read(fd[2][0] , score_result , BUFSIZ) == -1){
       // in case of any errors in reading
          perror("Read");
          return -12;
        }else{
	  YELLOW;
	  // printing the score of a specific round
          printf("The result of this Round according to the Referee is [ P1 - P2 : %s ]\n",score_result);
        }
        /* in this function, the parent reads the score and update the global scores of P1 and P2 */
        update_global_scores(score_result);

  } 
  /* this function prints the result in each round */
  print_until_now();
  /* The round is done, so we increased the number of rounds to start a new round */
  numOfRounds++;
  YELLOW;
  /*
  in this while loop, we will repeat the previous code, until one of the two processes
  reach the score 50, to determine the winner 
  */
  while(bigScore1 < 50 && bigScore2 < 50){
     WHITE;
     printf("--> ROUND : %s --------------------------------------------\n",rounds[numOfRounds]);
     YELLOW;
     sleep(2); 
     GREEN;
     printf("Hi P1, Please generate 10 numbers in range[1..100]and store them in child1.txt\n");
     YELLOW;   
     kill(cP1 , SIGUSR1);
     if(read(fd[0][0] , message , BUFSIZ) != -1){
         determineProcess = P1;
         kill(getpid() ,SIGINT); 
     }else{
        perror("read");
        return -14;
     }
     sleep(2);
     GREEN;
     printf("Hi P2, Please generate 10 numbers in range[1..100]and store them in child2.txt\n");
     YELLOW;
     kill(cP2 , SIGUSR1);   
     if(read(fd[1][0] , message , BUFSIZ) != -1){
        determineProcess = P2;
        kill(getpid() ,SIGINT); 
     }else{
       perror("read");
       return -15;
     } 
     sleep(2);
     GREEN;
     printf("Hi R, Please take the two files and return the result to me\n");
     YELLOW;
     kill(Ref , SIGUSR1);
     strcpy(message , "child1.txt - child2.txt");
     if(write(fd[2][1] , message , strlen(message)) == -1){
        perror("write");
        return -16;
     }
     sleep(2);
     strcpy(score_result , "");
     if(read(fd[2][0] , score_result , BUFSIZ) == -1){
        perror("write");
        return -17;
      }else{
          YELLOW;
          printf("The result of this Round according to the Referee is [ P1 - P2 : %s ]\n",score_result);
       }
      update_global_scores(score_result); 
      print_until_now();
      numOfRounds++;
  }
  WHITE;
  /* 
  call the function which determine the winner and number of required rounds
  */
  determine_winner(); 
  /*
  calling the signal SIGQUIT, to finish the game and kill the processes
  */
  kill(getpid() , SIGQUIT);  
  exit(-18);
}



/* -------------------------------------------------------------------------------
 * Handling the signal SIGINT function 
/* 
when the parent call the SIGINT means that, he recieved a message from his
children that they finished their files 
*/

void signal_catcher_sigint(int the_sig){
   if(determineProcess == P1){
      printf("Hi father, I'm P1, the numbers are written to child1.txt successfully !\n");
   }else if(determineProcess == P2){
      printf("Hi father, I'm P2, the numbers are written to child2.txt successfully !\n");
   }
}
/* -----------------------------------------------------------------------------------
/* Updating the global variables after each round */
void update_global_scores(char *result){
   /* the result is writeen as score1 - score2, we seperate it and store 
   the values of score1 and score2 to update the global variables */
   int val1 , val2;
   char num1[10] , num2[10];
   char *token;
   token = strtok(result,"-");
   strcpy(num1 , token);
   token = strtok(NULL, "-");
   strcpy(num2 , token);
   val1 = atoi(num1);
   val2 = atoi(num2);
   bigScore1 += val1;
   bigScore2 += val2;
}
/*------------------------------------------------------------------------------------
 * in this function we determine the winner by comparing the values of the global scores
*/
void determine_winner(){
   GREEN;   
   printf  ("<------------------------- The Results ------------------------->\n\n");

   if(bigScore1 >= 50 && bigScore2 >= 50){
     YELLOW;
     printf("                  < both players win! >                            \n");
     GREEN;
   }
   else if(bigScore1 >= 50){
     YELLOW;
     printf("                  < The player P1 is the winner >                   \n");
     GREEN;
   }else if(bigScore2 >= 50){
     YELLOW;
     printf("                  < The player P2 is the winner >                  \n");
     GREEN;
   }
   YELLOW;
   printf  ("                  < Number of Rounds = %d >                        \n\n",numOfRounds);
   GREEN;
   printf  ("<--------------------------------------------------------------->\n\n");
   WHITE;
}
/*------------------------------------------------------------------------------------------------
 * in this function we print the value of the gloval scores after each round
*/
void print_until_now(){
  RED;
  printf("-> The Score for P1 : %d\n",bigScore1);
  printf("-> The Score for P2 : %d\n",bigScore2);
  WHITE;
  BLUE;
  if(bigScore1 > bigScore2){
     printf("The score of P1 is Larger in Round %s\n\n\n",rounds[numOfRounds]);
  }else if(bigScore1 < bigScore2){
     printf("The score of P2 is Larger in Round %s\n\n\n",rounds[numOfRounds]);
  }else {
     printf("The score of P1 is equla to score of P2 in Round %s\n\n\n",rounds[numOfRounds]);
  }
  WHITE;
}

/* ------------------------------------------------------------------------------------------------------
 * signal catcher handler for the SIGQUIT
 * the parernt wait for his children
 * the parent used SIGKILL signal to kill his children
*/
void signal_catcher_sigquit(int the_sig){
   int wstatus1 , wstatus2 , wstatus3;
   if (kill(cP1 , SIGKILL) == -1) {
      perror("kill");
      exit(-19);
   }

   if (waitpid(cP1, &wstatus1, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(-20);
   }
   if (kill(cP2 , SIGKILL) == -1) {
      perror("kill");
      exit(-21);
   }

   if (waitpid(cP2, &wstatus2, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(-22);
   }
   if (kill(Ref , SIGKILL) == -1) {
      perror("kill");
      exit(-23);
   }

   if (waitpid(Ref, &wstatus3, WUNTRACED | WCONTINUED) == -1) {
      perror("waitpid");
      exit(-24);
   }
   RED;
   printf("\n\n * All Processes P1, P2 and R have been killed * \n\n");
   BLUE;
   printf("\n\n\n		<--------- THE GAME IS OVER -------->\n\n\n");
   exit(1);
}



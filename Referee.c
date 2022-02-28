#include "local.h"

/*
these global variables to store the values of read and write.
*/
int r;
int w;

/* score of P1 and P2 */
int score1 = 0;
int score2 = 0;

/* function prototypes */
void signal_catcher(int);          // signal catcher of the signal SIGUSR1
void compare(char * , char *);     // compare the values in the two files
void delete_files(char * , char *);// deleting the files

int main(int argc, char *argv[ ]){
   /* check the number of arguments */
   if(argc != 3){
       fprintf(stderr, "Usage: %s msq_id &\n", argv[0]);
       return 1;
   }
   /* store the read and write */
   r     = atoi(argv[1]);
   w     = atoi(argv[2]);
   /* receiving the signal */
   if ( sigset(SIGUSR1, signal_catcher) == SIG_ERR ) {
        perror("Sigset can not set SIGINT");
        exit(SIGINT);
     }
   /* 
   * The parent ask R to start manipulating and comparing the files after sending child1.txt-child2.txt message
   * through a pipe, when the parent sent a signal SIGUSR1 to R, R will wait for the parent to write in the pipe 
   * child1.txt-child2.txt message, after that, it compares the result and storing the score to return it to the parent
   * and here, R is waiting for the signal SIGUSR1 from the parent after that the parent write the message through the pipe
   * and R read this message and start manipulating the files
   */
   while(1);
 }
void signal_catcher(int the_sig){
   char score_1    [10];
   char score_2    [10];
   char final_score[10];
   char message[2 * BUFSIZ];
   char firstFile [12];
   char secondFile[12];
   /* read the messag child1.txt-child2.txt from the parent */
   if(read(r , message , 2 * BUFSIZ) == -1){
       perror("read");
       exit(2);
   }
   /* start manipulating the files */
   strcpy(final_score, "");
   char *token;
   token = strtok(message," ");
   strcpy(firstFile , token);
   token = strtok(NULL, " ");
   token = strtok(NULL, " ");
   strcpy(secondFile , token);
   /* compare the values of the files */
   compare(firstFile , secondFile);
   /* delete the files after storing the result */
   delete_files(firstFile , secondFile);
   sprintf(score_1 , "%d" , score1);
   sprintf(score_2 , "%d" , score2);
   score1 = score2 = 0;
   strcat(final_score , score_1);
   strcat(final_score , "-"   );
   strcat(final_score , score_2);
   /* sending the result through the pipe to the parent in the form of score1-score2 */
   if(write(w , final_score , strlen(final_score)) == -1){
     perror("Write");
     exit(1);
   }
 }
/* comparing the values in the files to get the result */ 
void compare(char *child1file , char *child2file){
   FILE *file_P1;
   FILE *file_P2;
   /* Opening the files */
   file_P1 = fopen(child1file , "r");
   if(file_P1 == NULL){
     perror("Open file");
   }
   file_P2 = fopen(child2file , "r");
   if(file_P2 == NULL){
     perror("Open file");
   }
   /* Comparing the values */
   for(int i = 0 ; i < 10 ; i++){
      int P1_element , P2_element;
      fscanf(file_P1, "%d", &P1_element);
      fscanf(file_P2, "%d", &P2_element);
      if(P1_element > P2_element){
        score1++;
      }else if(P1_element < P2_element){
        score2++;
      }
   }
   /* Closing the files */
   if(fclose(file_P1) == EOF){
     perror("Close file");
     exit(1);
   }
   if(fclose(file_P2) == EOF){
     perror("Close file");
     exit(1);
   }

}
/* deleting files function */
void delete_files(char *child1file , char *child2file){
  if(remove(child1file) == -1){
    perror("Remove");
    exit(1);
  }
  if(remove(child2file) == -1){
    perror("Remove");
    exit(2);
  }
  sleep(2);
  RED;
  printf("The files child1.txt and child2.txt have been deleted by R!\n");
  YELLOW;
}



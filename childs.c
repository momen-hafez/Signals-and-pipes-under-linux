#include "local.h"

/*
these global variables to store the values of read and write, and to distinguish
between P1 and P2, as they have the same file.
*/
int child;
int r;
int w;

int main(int argc, char *argv[ ]){
   /* 
   In this file we will check if the process required is P1 or P2 by send the process number as an 
   argument with the execl(..) command from the parent
   */
   void signal_catcher1(int); // handler function for P1 -> SIGUSR1
   void signal_catcher2(int); // handler function for P2 -> SIGUSR1

   char message[BUFSIZ]; /* message string to be send through the pipe */
   /* making sure that the rand() function generate different random numbers at each iteration */
   srand(getpid());
   /* check the number of sent arguments */
   if(argc != 4){
       fprintf(stderr, "Usage: %s msq_id &\n", argv[0]);
       return 20;
   }
   /* store the value of read, write and process type(P1 or P2)*/
   r     = atoi(argv[1]);
   w     = atoi(argv[2]);   
   child = atoi(argv[3]);
   /* in case of the called child is P1 */
   if(child == P1){
     if ( sigset(SIGUSR1, signal_catcher1) == SIG_ERR ) {
        perror("Sigset can not set SIGINT");
        return 21;
     }
   }else{/* in case of the called child is P2 */
     if ( sigset(SIGUSR1, signal_catcher2) == SIG_ERR ) {
        perror("Sigset can not set SIGINT");
        return 22;
     }
   } 
   while(1); 
   /* 
    * wait for the parent to ask the children to generate 10 numbers in range[1 .. 100]
    * and store them in the files child1.txt and child2.txt
   */
}
/* function for generate 10 numbers in range [1..100] and store them in child1.txt */
void signal_catcher1(int the_sig){
   int Rand10[10];  
   FILE * file;
   char message[BUFSIZ];
   /* store the numbers in array Rand10 */
   for(int i = 0 ; i < 10 ; i++){
     Rand10[i] = rand() % 100 + 1;
   }
   /* write the values to the file, each value at seperate line */
   file = fopen("child1.txt", "w");
   if(file == NULL){
     perror("File Open");
     exit (23);
   }
   for(int i = 0 ; i < 10 ; i++){
     fprintf(file, "%d\n", Rand10[i]);
   }
   fclose(file); 
   /* 
   * once the child finish writing the values to the file, he informs his parent 
   * by sending a message through the pipe 
   */
   strcpy(message, "Hi father, I'm P1, the numbers are written to child1.txt successfully !"); 
   if(write(w , message , strlen(message)) == -1){
     perror("write");
     exit(24);
   }
   sleep(2);
}
/* function for generate 10 numbers in range [1..100] and store them in child2.txt */
void signal_catcher2(int the_sig){
   int Rand10[10];
   FILE * file;
   char message[BUFSIZ];
   /* store the numbers in array Rand10 */
   for(int i = 0 ; i < 10 ; i++){
     Rand10[i] = rand() % 100 + 1;
   } 
   /* write the values to the file, each value at seperate line */
   file = fopen("child2.txt", "w");
   if(file == NULL){
     perror("File Open");
     exit(25);
   }
   for(int i = 0 ; i < 10 ; i++){
     fprintf(file, "%d\n", Rand10[i]);
   }
   fclose(file); 
   sleep(2); 
   /* 
   * once the child finish writing the values to the file, he informs his parent 
   * by sending a message through the pipe 
   */
   strcpy(message, "Hi father, I'm P2, the numbers are written to child2.txt successfully !"); 
   if(write(w , message , strlen(message)) == -1){
     perror("write");
     exit(26);
   }
   
}



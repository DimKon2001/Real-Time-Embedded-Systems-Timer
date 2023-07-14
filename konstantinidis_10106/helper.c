//Definition of the function that prints the user menu.
void print_debug(int case_n);


int execution_mode(){
  int execution_mode_choice;

  printf("Select program execution mode:\n");
  printf("1. One timer with a period of 1 sec.\n" );
  printf("2. One timer with period of 0.1 sec.\n" );
  printf("3. One timer with period of 0.01 sec.\n" );
  printf("4. Four timers with periods of 1, 0.1 and 0.01 seconds.\n" );

  //Check if someone inputs an invalid data type as scanf input .
  while((scanf("%d", &execution_mode_choice) != 1) | (execution_mode_choice != 1 & execution_mode_choice != 2 & execution_mode_choice != 3 & execution_mode_choice != 4)){
    printf("Please enter a valid number (1-4).\n" );
    
  }
  return execution_mode_choice;
}

//NOT OK
void prog_init(int* option, int* n_timers, int* P){

  if(*option == 1){
      *n_timers=1;
      *P=*n_timers;
      *option=0;
  }else if(*option == 2){
      *n_timers=1;
      *P=*n_timers;
      *option=1;
  }else if(*option == 3){
      *n_timers=1;
      *P=*n_timers;
      *option=2;
  }else if(*option == 4){
      *n_timers=3;
      *P=*n_timers;
      *option=0;
  }

}
void initialize_cons_queue(int *num_consumers, int *size_queue){
  

  printf("Select the size of the queue:\n");

  //Check if someone inputs an invalid data type as scanf input .
  if(scanf("%d", size_queue)!= 1) {
    //empty the scanf variable.
    //scanf("%*s");
    printf("Please enter one valid option. \n" );
    initialize_cons_queue(num_consumers, size_queue);
    return;
  }

  printf("Select the number of consumers:\n");

  if(scanf("%d", num_consumers)!= 1) {
    //empty the scanf variable.
    //scanf("%*s");
    printf("Please enter one valid option. \n" );
    initialize_cons_queue(num_consumers, size_queue);
    return;
  }
  
    printf("Queue size and number of consumers are set to %d and %d respectively.\n", *size_queue , *num_consumers);
}

void print_debug(int case_n){

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  if(case_n){
    printf("Executing at local time and date: %s", asctime (timeinfo) );
  }else{
    printf("Full queue while executing at local time and date: %s", asctime (timeinfo) );
  }

}



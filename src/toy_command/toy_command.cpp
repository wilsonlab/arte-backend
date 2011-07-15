#include "toy_command.h"

int main (int argc, char *argv[]){

  if(argc != 3){
    printf("Usage: toy_command ip_addy port_num");
    exit(1);
  }

  NetCom my_netcom;
  NetComDat my_netcom_dat;

  int port_num;
  sscanf(argv[2], "%d", &port_num);

  my_netcom_dat = my_netcom.initUdpTx(argv[1], port_num);

  bool running = true;
  command_t the_command;
  //char command_text[MAX_COMMAND_STR_LEN];
  //the_command.command_str = &command_text[0];

  while(running){

    printf("\nPlease enter a command for arte, or type exit to quit:\n");
    fgets( the_command.command_str, MAX_COMMAND_STR_LEN+1, stdin );
    
    for(int i = 0; i < MAX_COMMAND_STR_LEN + 1; i++){
      if(the_command.command_str[i] == '\n'){
	//printf("Caught newline! at i=%d\n",i);
	the_command.command_str[i] = '\0';
	the_command.n_char = i;
	break;
      }
    }

    if( strcmp(the_command.command_str, "exit") == 0 ){
      running = false;
    } else{
      //printf("about to send command:[%s]  strlen is:%d\n",
	     the_command.command_str,strlen(the_command.command_str));
      NetCom::txCommand(my_netcom_dat, &the_command);
    } 
  }
  
  return 0;

}


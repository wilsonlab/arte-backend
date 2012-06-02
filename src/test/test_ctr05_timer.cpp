#include "../ctr05_timer.h"
#include <iostream>
#include <string>

using namespace std;
int get_choice(){
  cout << "1. Construct" << endl << "2. Destroy" << endl 
       << "3. Start Count" << endl << "4. Stop Count" << endl
       << "5. Start Emit" << endl << "6. Stop Emit" << endl
       << "7. Tic" << endl << "8. Toc" << endl
       << "9. Get_count" << endl << "10. Get_time" << endl
       << "11. Quit" << endl << "12. Pass" << endl;
  int choice;
  cin >> choice;
  return choice;
}


int main(int argc, char *argv[]){

  Ctr05Timer *myTimer;

  int c;
  while(1){
    c = get_choice();
    if(c==1){
      std::string timer_mode ("ec");
      myTimer = new Ctr05Timer ( timer_mode );
    } else if(c==2){
      delete myTimer;
    } else if(c==3){
      myTimer->start_counting();
    } else if(c==4){
      myTimer->stop_counting();
    } else if(c==5){
      myTimer->start_emitting();
    } else if(c==6){
      myTimer->stop_emitting();
    } else if(c==7){
      myTimer->tic();
    } else if(c==8){
      cout << "toc time: " << myTimer->toc() << endl;
    } else if(c==9){
      cout << "get_count(): " << myTimer->get_count() << endl;
    } else if(c==10){
      cout << "get_timestamp_secs(): " << myTimer->get_timestamp_secs() << endl;
    } else if(c==11){
      break;
    }
    myTimer->print_state();
  }
  return 0;
}

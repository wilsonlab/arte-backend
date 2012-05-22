#include <iostream>
#include <string>
#include <assert.h>
#include <google/protobuf/text_format.h>
#include "../glom.h"

int main( int argc, char *argv[] ){

  std::string in_filename;
  std::string out_filename;
  char in_mode, out_mode;

  for(int i = 1; i < argc; i++){
    if (strcmp(argv[i], "-i") == 0)
      in_filename.assign( argv[i+1] );
    if (strcmp(argv[i], "-o") == 0){
      out_filename.assign( argv[i+1] );
      std::cout << in_filename << std::endl;
    }
  }

  if( in_filename.find(".txt") == std::string::npos ){
    in_mode = 'b';
  } else {
    in_mode = 'a';
  }

  if( out_filename.find(".txt") == std::string::npos ){
    out_mode = 'b';
  } else {
    out_mode = 'a';
  }

  iGlom myInGlom  ( in_filename.c_str(),  in_mode );
  oGlom myOutGlom ( out_filename.c_str(), out_mode );

  int rc = 1;
  int rc2 = 1;
  ArtePb t;
  while (rc == 1){

    sleep(1);
    
    rc = myInGlom.pb_read ( t );
    std::cout << "pb_read rc = " << rc << std::endl;
    if( rc == 0 )
      std::cout << "Reached end of input file." << std::endl;
    if( rc == -1 )
      std::cout << "Encountered an input error!" << std::endl;
    if( rc == 1 ){
      std::string my_string;
      google::protobuf::TextFormat::PrintToString(t, &my_string);
      std::cout << "Read success!  Message is:" << std::endl 
		<< my_string << std::endl;
    }

    sleep(1);

    if (rc == 1){
      rc2 = myOutGlom.pb_write( t );
      if( rc2 == -1 )
	std::cout << "Encountered a write error!" << std::endl;
    }

  }
  
  std::cout << "Out of while loop." << std::endl;

  return 0;

}

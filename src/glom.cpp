#include <iostream>
#include <stdio.h>
#include <google/protobuf/text_format.h>
#include <time.h>
#include "glom.h"


iGlom::iGlom(const char *_file_name, const char _mode){

  fflush(stdout);
  filename.assign( _file_name );
  mode = _mode;
  std::ios_base::openmode ios_mode;
  if (_mode == 'b')
    ios_mode = std::ios::binary | std::ios::in;
  if (_mode == 'a')
    ios_mode = std::ios::in;
  

  file = new std::fstream ( filename.c_str(), ios_mode );
  if(!file->is_open()){
    std::cerr << "iGlom failed to open file " << filename << std::endl;
  }

  pthread_mutex_init( &glom_mutex, NULL );

}

iGlom::~iGlom(){
  file->close();
}

int iGlom::pb_read(ArtePb& _pb_to_read_into){

  pthread_mutex_lock( &glom_mutex );

  uint32_t next_buff_size;

  // read from binary file
  if (mode == 'b'){
    // get the size of the next buffer
    file->read( reinterpret_cast<char*>(&next_buff_size), 
	       sizeof(next_buff_size) );

    if(next_buff_size == 0){
      pthread_mutex_unlock( &glom_mutex );
      return 0;
    }

    if( file->eof() ){
      std::cerr << "iGlom reached input prematurely in " << filename << std::endl;
      pthread_mutex_lock( &glom_mutex );
      return -1;
    }
    
    char_buffer.reserve(next_buff_size);
    file->read( &char_buffer[0], next_buff_size);
    bool parse_success = 
      _pb_to_read_into.ParseFromArray( &char_buffer[0], next_buff_size );

    if(!parse_success){
      std::cerr << "Parse error in iGlom on file " << filename << std::endl;
      pthread_mutex_unlock( &glom_mutex );
      return -1;
    }

    pthread_mutex_unlock( &glom_mutex );
    return 1;
  }

  // read from ascii file
  if (mode == 'a'){
    
    (*file) >> next_buff_size;
    file->get();

    if (next_buff_size == 0){
      pthread_mutex_unlock( &glom_mutex );
      return 0;
    }

    if (file->eof()){
      std::cerr << "iGlom reached end of file prematurely in ascii file: " 
		<< filename << std::endl;
      pthread_mutex_unlock( &glom_mutex );
      return -1;
    }

    char_buffer.reserve(next_buff_size);
    file->get( &char_buffer[0], next_buff_size );
    string_buffer.assign( char_buffer.begin(), 
			  char_buffer.begin()+next_buff_size-1 );
    
    // Consume the \n written after the buffer text
    file->get();

    bool parse_success = 
      google::protobuf::TextFormat::ParseFromString(string_buffer,
						    &_pb_to_read_into );

    if(!parse_success){
      std::cerr << "Parse error in iGlom on ascii file " 
		<< filename << std::endl;
      pthread_mutex_unlock( &glom_mutex );
      return -1;
    }
    
    pthread_mutex_unlock( &glom_mutex );

    return 1;
    
  }
  
  std::cerr << "iGlom::pb_read: this block of code should never be reached.\n";
  pthread_mutex_unlock ( &glom_mutex );
  return -1;

}





oGlom::oGlom(const char *_file_name, const char _mode){
  
  bool got_empty_file = false;
  filename.assign( _file_name );

  mode = _mode;
  std::ios_base::openmode ios_mode;
  if(_mode == 'b')
    ios_mode = std::ios::binary | std::ios::out | std::ios::ate | std::ios::app;
  if(_mode == 'a')
    ios_mode = std::ios::out | std::ios::ate | std::ios::app;

  // replace _date in requested filename with a nice date string
  int date_pos = filename.find("_date");
  if( date_pos != std::string::npos){
    time_t rawtime;
    time ( &rawtime );
    std::string date_string ("_yyyy_mmm_dd");
    std::string ctime_string( ctime( &rawtime ) );
    date_string.replace(1,  4, ctime_string, 20, 4);
    date_string.replace(6,  3, ctime_string,  4, 3);
    date_string.replace(10, 2, ctime_string,  8, 2);
    filename.replace( date_pos, 5, date_string );
  }

  // Add _new to filename until we make a unique file (prevent overwrites)
  while( !got_empty_file ){
    file = new std::fstream ( filename.c_str(), ios_mode );
    if(!file->is_open()){
      std::cerr << "oGlom failed to open file " << filename << std::endl;
    } else {
      std::ifstream::pos_type size = file->tellg();
      if (size > 0){
	file->close();
	delete file;
	// change the filename and let the loop iterate again
	size_t dot_pos = filename.find_first_of('.');
	if (dot_pos == std::string::npos)
	  dot_pos = filename.length();
	std::cout << "Filename " << " was not empty, trying " 
		  << filename.insert( dot_pos, "_new") << std::endl;
      } else {
	got_empty_file = true;
      }
    }
  }
  
  pthread_mutex_init( &glom_mutex, NULL );

}


bool oGlom::pb_write(ArtePb &_pb_to_write){

  uint32_t buffer_size;
  int rc;
  bool serialize_success = false;

  if( !_pb_to_write.IsInitialized() ){
    std::cerr << "oGlom attempt to write uninitialized ArtePb to file "
	      << filename << std::endl;
    return false;
  }


  if( mode == 'b' ){

    buffer_size = _pb_to_write.ByteSize();    
    pthread_mutex_lock( &glom_mutex );
    rc = 0;
    try{
      file->write( reinterpret_cast<char*>(&buffer_size), 
		  sizeof(buffer_size) );
    }
    catch (std::exception &e){
      std::cerr << e.what() << std::endl;
      rc = -1;
    }

    if( rc == 0 )
      serialize_success = _pb_to_write.SerializeToOstream( file );

    pthread_mutex_unlock( &glom_mutex );

    if( rc != 0 ){
      std::cerr << "oGlom file->write() error on file " 
		<< filename << std::endl;
      return -1;
    }

  }
    
  if( mode == 'a' ){

    pthread_mutex_lock( &glom_mutex );
    serialize_success = 
      google::protobuf::TextFormat::PrintToString(_pb_to_write,
						  &string_buffer);

    buffer_size = string_buffer.length();
    try{
      (*file) << buffer_size << std::endl;
      (*file) << string_buffer << std::endl;
    } catch (std::exception &e){
      rc = -1;
      std::cerr << "oGlom write failure on ascii file " 
		<< filename << std::endl;
    }
    
    pthread_mutex_unlock( &glom_mutex );

  }


  if( !serialize_success ){
    google::protobuf::TextFormat::PrintToString(_pb_to_write,
						&string_buffer);
    std::cerr << "oGlom ArtePb.SerializeToOstream error on file "
	      << filename << std::endl << "with message: " << std::endl
	      << string_buffer << std::endl;    
    return -1;
  }

  return 0;

}


oGlom::~oGlom(){

  if (mode == 'b'){
    uint32_t end_tag = 0;
    file->write ( reinterpret_cast<char*>(&end_tag), sizeof(end_tag) );
  }

  if (mode == 'a'){
    (*file) << '0';
  }

  file->close();

}

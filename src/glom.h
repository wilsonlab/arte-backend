#ifndef GLOM_H_
#define GLOM_H_

// Glom - read and write glommed arte_pb files; file wrapper.
//
// Progressive connatination class for arte_pb protocol buffers, whose boundaries are generally 
// Irrecoverable from the buffer data alone.  Glom serializes buffers by writing the size of the
// Next buffer in the file before writing the pb itself. A final uint32_t 0 is written to show
// Good file closure after the final buffer and to check proper parsing in other programs.
//
// Binary case ( _mode = 'b')
// 10, 10 byte long arte_pb buffer, 50, 50 byte arte_pb buffer, 0 (EOF)
// 
// ASCII case ( _mode = 'a');
// ascii 10\n, 10 byte arte_pb message ascii , ascii 50\n, 50 byte arte_pb message ascii , ascii 0
//
// Code & idea come from: http://stackoverflow.com/questions/3513028/marshall-multiple-protobuf-to-file
// Nicholas Palko: https://github.com/npalko/uRPC/blob/master/src/glob.hpp
//
// ImAlsoGreg@gmail.com - please contact me about any questions or improvements.

#include <fstream>
#include <pthread.h>
#include "arte_pb.pb.h"

// iGlom class deserializes arte_pb buffers
class iGlom{

 public:
  // Constructor takes a filename and a mode char:  'a' for ascii, 'b' for binary
  // Glom respects existing files, and will change the filename to one
  // that doesn't conflict with existing ones if a conflict is found
  iGlom(char* _file_name, char _mode);
  ~iGlom();

  bool pb_read(arte_pb&  _pb_to_read_into);
  
  // Things to think about implementing if we end up wanting them:
  //int pb_advance(int advance_count);
  //int pb_rewind(int rewind_count);
  //int pb_advance_to_timestamp(uint32_t _target_timestamp);
 private:
  std::fstream file;
  bool binary_mode;
  pthread_mutex_t glom_mutex;
};

// oGlom class serializes arte_pb buffers
class oGlom{

 public:
  oGlom(char* _file_name, char* _mode);
  ~oGlom();

  bool pb_write(arte_pb& _pb_to_write);

 private:
  std::fstream file;
  bool binary_mode;
  pthread_mutex_t glom_mutex;
};

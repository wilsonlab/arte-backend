#ifndef UTIL_H_
#define UTIL_H_

#define _FILE_OFFSET_BITS  64
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include "assert.h"
#include <stdio.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <NIDAQmx.h>
#include "global_defs.h"
#include <cstring>
#include <iostream>
#include <assert.h>

class neural_daq; //forward declaration



//defining path for files relative to home
std::string appendhome( std::string const & filename );

// convert float64 (double) values to int16_t
rdata_t ftor(double *f_val);         // single value
void ftor_a(double *f_vals,rdata_t  *r_vals, int n_elem);  // array

// parse string "val1 val2 etc" into array.  set n_elem = -1 to avoid checking the count
template <class T> 
void parse_line_for_vals(std::string the_line,T *t, int n_elem){
  using namespace std;
  istringstream iss;
  iss.str(the_line);
  int n = 0;
  while((! iss.eof()) && iss.good() ){
    iss >> t[n];
    n+=1;
  }
  // only do this check if we care about the n_elem specification
  if(n_elem == -1){
    if(n != n_elem){
      std::cerr << "While parsing nums from a string, found " << n << " numbers when " << n_elem << " were requested." << std::endl;
      std::cerr << "Maybe an error in arteopt, or in the xml .conf files.  If n_chans, check for n thresh values and n window_heights, etc." << std::endl;
      std::cerr << "Input string:" << the_line << std::endl;
      std::cerr << "(watch out for white spaces?)" << std::endl;
      for(int a = 0; a < n; a++)
	std::cout << "array element is: " << t[a] << std::endl;
    }
    std::cout << "Requested " << n_elem << " elements and got " << n << std::endl;
    assert(n == n_elem);
  }
}

// template class to set a trode's field to a value specified in that trode's property tree, or to that of the default trode propetry tree.
// if there is no default property tree, easiest thing to do is just pass the trode's property tree twice
template <class T>
int assign_property(std::string &tree_key, T * t, const boost::property_tree::ptree &this_trode_pt, const boost::property_tree::ptree &default_trode_pt, int n_elem){

  std::istringstream iss;
  std::string the_result;

  the_result = this_trode_pt.get<std::string>(tree_key, default_trode_pt.get<std::string>(tree_key) );
  iss.str(the_result);

  parse_line_for_vals <T> (iss.str(), t, n_elem);

}

template <class T>
int assign_property_ftor(const char *tree_key, T * t, const boost::property_tree::ptree &this_trode_pt, const boost::property_tree::ptree &default_trode_pt, int n_elem){

  std::string tk = tree_key;
  float64 *f_vals = new float64 [n_elem];
  assign_property<float64>(tk, f_vals, this_trode_pt, default_trode_pt, n_elem);
  ftor_a(f_vals, t, n_elem);

}

// override for passing a char[] instead of a full std::string object.  maybe or maybe not we should just make this
// the default parameter list?
template <class T>
int assign_property(const char *tree_key, T * t, const boost::property_tree::ptree &this_trode_pt, const boost::property_tree::ptree &default_trode_pt, int n_elem){

  std::string tk = tree_key;
  assign_property <T> (tk, t, this_trode_pt, default_trode_pt, n_elem);
}

// simple template class to simply array initialization?
template <class T>
void init_array(T * t, T init_value, int num_el){
  T zero_val = 0;
  if( init_value == zero_val)
    memset(t, (int)init_value,  sizeof(init_value) * num_el);
  else{
    for (int n = 0; n < num_el; n++)
      t[n] = init_value;
  }
}

void daq_err_check(int32 error);
void ECmx(int32 error);
void daq_err_check(int32 error, TaskHandle *task_handle_array, int n_tasks);
void daq_err_check_end(int32 error, neural_daq this_nd); // stops and clears task when errors


int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);
int32 CVICALLBACK timerDigTrigCallback(TaskHandle taskHandle, int32 signalID, void *callbackData);

FILE *try_fopen( const char *filename, const char *mode);
void try_fclose( FILE *the_file );

template <class T>
void try_fread( T *ptr, size_t count, FILE * stream){
  if(stream == NULL){
    printf("try_fread error: bad file\n");
    exit(1);
  }
  size_t result = fread( ptr, sizeof(ptr[0]), count, stream);
  if( result != count ){
    printf("size mismatch error.\n");
  }
}

template <class T>
void try_fwrite( T *ptr, size_t count, FILE * stream){
  if( stream == NULL){
    printf("try_fwrite error: bad file \n");
    exit(1);
  }
  size_t result = fwrite(ptr, sizeof(ptr[0]), count, stream);
  if (result != count){
    printf("try_fwrite: size mismatch \n");
  }
}

#endif


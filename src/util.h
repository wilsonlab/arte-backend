#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include "assert.h"
#include <stdio.h>

// Template functions can't have their interface and implementation separated.
// So please don't move this fn to util.cpp
// Thanks
template <class T> 
void parse_line_for_nums(std::string the_line,int n_elem, T *t){
  
  using namespace std;

  istringstream iss;

  iss.str(the_line);


  // counting this way seems to be sensitive to white spaces at 
  // beginning and end of string.  Can this be fixed?
  // (try it with test_util.cpp)
  int n = 0;
  while(! iss.eof()){
    iss >> t[n];
    n+=1;
  }

  if(n != n_elem){
    std::cerr << "While parsing nums from a string, found " << n << " numbers when " << n_elem << " were requested." << std::endl;
    std::cerr << "Maybe an error in arteopt, or in the xml .conf files.  If n_chans, check for n thresh values and n window_heights, etc." << std::endl;
    std::cerr << "Input string:" << the_line << std::endl;
    std::cerr << "(watch out for white spaces?)" << std::endl;
    for(int a = 0; a < n; a++)
      std::cout << "array element is: " << t[a] << std::endl;
  }
  assert(n == n_elem);
}

// Populate a trode's fields with either the value given by the property tree,
// or, if property tree has no info for this trode, use the default value
// To specify that the trode being populated _IS_ the default trode, pass NULL
// as the last argument.
// var is a pointer to the member variable to be set.  this_trode_ptree is a ptree
// node corresponding to only this tetrode.  tree_key is a string key into the ptree
// to the target parameter. default_var is NULL or a pointer to the default value to use
// in the case that tree_key into ptree doesn't give a value.
template <class T>
int assign_trode_property(T *var, boost::property_tree::ptree this_trode_ptree, std::string tree_key, T *default_var){

  // How do you check for failure when searching a ptree?
  //  istringstream iss;
  //iss.chr(this_trode_ptree

  //if(default_var == NULL){  // then we're popu

}




#endif

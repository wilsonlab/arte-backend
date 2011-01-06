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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/exceptions.hpp>

// Template functions can't have their interface and implementation separated.
// So please don't move these fns to util.cpp
// Thanks

// take a string which haz between 1 and n space-delimited values
// that need to get written into an array.
// pass the string to parse as arg 1, a pointer to the array to populate as t
// pass expected number of elements in n_elem, or -1 to tell the function not to check
template <class T> 
void parse_line_for_vals(std::string the_line,T *t, int n_elem){
  
  using namespace std;

  istringstream iss;

  iss.str(the_line);

  int n = 0;
  while(! iss.eof()){
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
    assert(n == n_elem);
  }
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
int assign_property(std::string &tree_key, T * t, const boost::property_tree::ptree &this_trode_pt, const boost::property_tree::ptree &default_trode_pt, int n_elem){

  std::istringstream iss;
  std::string the_result;

  the_result = this_trode_pt.get<std::string>(tree_key, default_trode_pt.get<std::string>(tree_key) );
  iss.str(the_result);

  parse_line_for_vals <T> (iss.str(), t, n_elem);

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
  else
    for (int n = 0; n < num_el; n++)
      t[n] = init_value;}

#endif

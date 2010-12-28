#include "../src/util.h"

// <float *> parse_line_for_nums(string, 5, float *);

int main(int argc, char *argv[]){

  std::cout << "first part of main" << std::endl;

  float *a;

  double *b;

  b = new double [5];

  std::string str ("0.4 1.2212 2.345 34.0 7");

  std::cout << "about to enter function." << std::endl;

  parse_line_for_nums<double>(str, 5,b);

  return 0;

}

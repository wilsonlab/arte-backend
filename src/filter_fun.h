#ifndef FILTER_FUN_H_
#define FILTER_FUN_H_

#include "filtered_buffer.h"

// This function may belong in filt.h/.cpp, but
// because it takes a Filtered_buffer as an argument,
// it needs to include Filtered_buffer.
// But filtered_buffer needs to include the header to this function
// so that would be circular.

// The solution was to break this into a separate file
// another option would simply be to declare and define 
// the function in filtered_buffer.h/.cpp, but it doesn't
// really logically fit there.  Suggestions on how to fix
// the structure are welcome!
void filter_buffer( Filtered_buffer * fb );



#endif

#include <iostream>
#include <stdint.h>

int main(){

	const char c[] = "Hi!";
	uint32_t* i;
	i = (uint32_t *) c;

	uint32_t j = 2189640;	
	char* d;
	d = (char*) &j;
	
	
	std::cout<<*i<<std::endl;
	std::cout<<d<<std::endl;

}

#include <iostream>
#include <stdint.h>
#include <netcom.h>



int main(){
	
	timestamp_t t = 12345;
	std::cout<<"Input Number:"<<t<<std::endl;
	char* b = new char[4];

	std::cout<<"virgin bf:";printBuff(b,4);
	tsToBuff(&t, b, 4);
	std::cout<<"tsToBuff :";printBuff(b,4);

	timestamp_t u = hton32(t);
	memcpy(b, &u, 4);
	std::cout<<"memcpy bs:";printBuff(b,4);

//	memcpy(b, &t, 4);
//	std::cout<<"memcpy   :";printBuff(b,4);

	timestamp_t ts = buffToTs(b, 4);
	std::cout<<"buffToTs:"<<ts<<std::endl;
	memcpy(&ts, b, 4);
	std::cout<<"memcpy  :"<<ts<<std::endl;
	std::cout<<"casting :"<<*((timestamp_t*)b)<<std::endl;	
}

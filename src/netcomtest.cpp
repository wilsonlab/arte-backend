#include "netcom.h"

int main(){
	
	for (int i=0; i<10; i++)
	{
		NetCom::txSyncCount("localhost", 4000, i*1000, 1);
	}

	return 0;
}

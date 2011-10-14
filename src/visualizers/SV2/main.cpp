#include "GlutWindow.h"

int main(int argc, char *argv[]){

	const char* ports[] = {	"6300", "6301", "6302", "6303", "6304", "6305", "6306", "6307",
	 					"7008", "7009", "7010", "7011", "7012", "7013", "7014", "7015",
	//					"7016", "7017", "7018", "7019", "7020",	 "7021", "7022", "7023",
	//					"7024", "7025", "7026", "7027", "7028", "7029", "7030", "7031",
					};	
						
	createGlutWindow(20,20,800,600,(char*)"title", argc, argv);

	setPorts((char **)ports);
	setLayoutDims(4,4);
	showWindow();
}
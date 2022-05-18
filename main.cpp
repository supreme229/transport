#include <iostream>

#include "utils.h"

#include "transport.h"
#include "window.h"

using namespace std;

int main(int argc, char *argv[])
{
	int input_correct = validateInput(argc, argv);

	if (!input_correct)
	{
		return -1;
	}

	Transport transport(argv);

	if(transport.setup()){
		return -1;
	}

	//Window window(stoi(argv[4]));

	

	transport.receiveFile();

	return 0;
}
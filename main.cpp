#include <iostream>

#include "transport.h"
#include "utils.h"

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

	transport.receiveFile();

	return 0;
}
// Bartosz Janikowski, 315489

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
		cout << "usage: [server ip address] [server port] [file result name] [file size]" << endl;
		return -1;
	}

	Transport transport(argv);

	if (!transport.setup())
	{
		return -1;
	}

	transport.receiveFile();

	return 0;
}

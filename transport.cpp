#include <iostream>

#include "utils.h"

using namespace std;

int main(int argc, char *argv[])
{

    int input_correct = ValidateInput(argc, argv);

    if(!input_correct){
        return -1;
    }



    return 0;
}
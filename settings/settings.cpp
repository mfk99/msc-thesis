#include <iostream>
using namespace std;

int debug = 0;

void enterSettings(){
    cout << "Toggling debug. It is currently " << debug << "\n";
    if (debug == 0) debug = 1;
    else debug = 0;
    cout << "Debug is now " << debug << "\n";
}
#include <iostream>
#include <iomanip>
#include "generate_file.h"

using namespace std;
int main() {
    std::cout << "Start\n";
    generate_file(100);
    std::cout << "Done";
    return 0;
}
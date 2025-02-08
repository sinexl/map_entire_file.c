#include "MappedFileWrite.hpp"
#include <iostream>
using namespace std;

int main() {
    MappedFileWrite f("./test_file.txt");
    for (size_t i = 0; i < f.size(); i++) {
        cout << f[i] << endl;
    }

    f[1] = '0';

    f.close();

    return 0;
}

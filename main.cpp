#include <iostream>

using namespace std;

int myatoi(const char* a)
{
    int result(0);
    for(;a && *a; ++a) {
        result = result * 10 + static_cast<int>(*a - '0');
    }
    return result;
}

int main() {
    string a;
    cin >> a;
    cout << endl << myatoi(a.c_str()) << endl;
    return 0;
}
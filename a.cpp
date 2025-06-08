#include <iostream>
using namespace std;

int main() {
    int a = 42;
    int* ptr = &a;

    cout << "Valeur de a      : " << a << endl;
    cout << "Adresse de a     : " << &a << endl;
    cout << "Valeur de ptr    : " << ptr << endl;
    cout << "Valeur pointee   : " << *ptr << endl;

    *ptr = 99;
    cout << "Nouvelle valeur de a : " << a << endl;

    return 0;
}

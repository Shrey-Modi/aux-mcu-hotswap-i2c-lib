// main method hello workd

#include <iostream>
#include <string>


void editChar(char* str, int index, char c)
{
    str[index] = c;
}

int main()
{
    char str[] = "Hello World!";
    editChar(str, 0, 'J');
    int16_t a = 0x0fa0 >> 3;
    // a=a/16;
    std::cout << a << std::endl;
    // double b = -1.1;
    // std::cout << b << std::endl;

    // printf("%x %x\n", a, b);
}
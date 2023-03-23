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
    std::cout << str << std::endl;
}
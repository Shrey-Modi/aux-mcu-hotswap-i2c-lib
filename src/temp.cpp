#include <iostream>

void itoa(char *array, int16_t i)
{
    uint32_t j = 0;
    uint32_t s_space = 0;
    if (i < 0)
    {
        array[j] = '-';
        i = -i;
        j++;
        s_space++;
    }

    uint32_t rem = 0;
    uint32_t len = 0;
    uint32_t n = i;

    while (n != 0)
    {
        len++;
        n /= 10;
    }

    for (uint32_t k = 0; k < len; k++)
    {
        rem = i % 10;
        i = i / 10;
        array[len - (k + 1)+s_space] = rem + '0';
    }

    array[len+s_space] = '\0';    
}

int main()
{
    char array[15];
    int16_t a = 40960 / (((float)40/32767) * 1300);

    itoa(array, a);



    std::cout << array << std::endl;

    printf("%d\n", a);
    

    
}
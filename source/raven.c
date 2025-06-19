#include <stdio.h>

int main(int argc, char **argv)
{
    (void)argv;

    if (argc != 2)
    {
        printf("usage: raven input\n");
        return 1;
    }

    return 0;
}

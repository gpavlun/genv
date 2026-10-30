#include <stdio.h>

#include "error.h"

enum {
    ERR_DIVIDE_BY_ZERO = 1,
    ERR_INVALID_ARGUMENT,
    ERR_OUT_OF_MEMORY
};


int divide2(int a, int b)
{
    clear_err(divide2);

    if (b == 0) {
        set_err(divide2, ERR_DIVIDE_BY_ZERO);
        return 0;
    }

    return a / b;
}

int divide1(int a, int b)
{
    clear_err(divide1);

    if (b == 0) {
        set_err(divide1, ERR_DIVIDE_BY_ZERO);
        return 0;
    }

    return a / b;
}

int divide(int a, int b)
{
    clear_err(divide);

    if (b == 0) {
        set_err(divide, ERR_DIVIDE_BY_ZERO);
        return 0;
    }

    return a / b;
}

int main(void)
{
    int value = divide(10, 0);

    if_err(divide) {
        error_code_t error = get_err(divide);

        if (error == ERR_DIVIDE_BY_ZERO) {
            printf("divide by zero error\n");
        }
    }

    value = divide1(10, 0);

    if_err(divide) {
        error_code_t error = get_err(divide);
        clear_err(divide);

        if (error == ERR_DIVIDE_BY_ZERO) {
            printf("divide by zero error\n");
        }
    }
    if_err(divide1) {
        error_code_t error = get_err(divide);

        if (error == ERR_DIVIDE_BY_ZERO) {
            printf("divide by zero error\n");
        }
    }


    value = divide2(10, 2);

    if_err(divide2) {
        printf("are you sure?\n");
    }







    error_shutdown();

    return 0;
}
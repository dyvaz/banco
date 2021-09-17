#include "./bank_err.h"

unsigned char __bank_last_err = BANK_ERR_NONE;

unsigned char bank_last_error()
{
    return __bank_last_err;
}
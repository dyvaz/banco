#include "./bank_err.h"

unsigned char __bank_last_err = BANK_ERR_NONE;

unsigned char bank_last_error()
{
    return __bank_last_err;
}

void bank_clear_error()
{
    __bank_last_err = BANK_ERR_NONE;
}

void set_err_if_none(unsigned char err)
{
    if (__bank_last_err == BANK_ERR_NONE)
    {
        __bank_last_err = err;
    }
}
#ifndef BANK_ERR_H
#define BANK_ERR_H

#define BANK_ERR_NONE 0
#define BANK_ERR_UNEXPECTED_NULL 1
#define BANK_ERR_CANT_APPLY_LOG_ENTRY 2
#define BANK_ERR_CANT_OPEN_FILE_READ 3
#define BANK_ERR_CANT_OPEN_FILE_WRITE 4
#define BANK_ERR_INVALID_PIN 5
#define BANK_ERR_ACCOUNT_CLOSED 6
#define BANK_ERR_NEGATIVE_AMOUNT 7
#define BANK_ERR_INSUFFICIENT_BALANCE 8
#define BANK_ERR_ACCOUNT_NOT_FOUND 9
#define BANK_ERR_CANT_READ_FILE 10
#define BANK_ERR_CANT_WRITE_FILE 11
#define BANK_ERR_UNKNOWN_LOG_ENTRY_KIND 12
#define BANK_ERR_BALANCE_NOT_ZERO 13

unsigned char bank_last_error();

#endif
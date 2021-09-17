#ifndef BANK_H
#define BANK_H

#include <stdbool.h>
#include <stdio.h>
#include "./bank_log.h"

typedef struct
{
    unsigned long number;
    long balance;
    char pin[4];
    bool open;
} account;

typedef struct
{
    unsigned char kind;
    unsigned long from;
    unsigned long to;
    long amount;
} audit_log;

typedef struct
{
    size_t accounts_size;
    account **accounts;
    char *filename;
} bank;

bool bank_alloc(bank *bank);
bool bank_free(bank *bank);
bool bank_apply_log(bank *bank, bank_audit_log_entry *log);
bool bank_init_from_file(bank *bank, const char *filename);

bool bank_account_open(bank *bank, char pin[4], unsigned long *number);
bool bank_account_close(bank *bank, unsigned long number, char pin[4]);

bool bank_account_deposit(bank *bank, unsigned long number, long amount);
bool bank_account_withdraw(bank *bank, unsigned long number, long amount, char pin[4]);
bool bank_account_transfer(bank *bank, unsigned long from, unsigned long to, long amount, char pin[4]);
bool bank_account_balance(bank *bank, unsigned long number, long *balance);

#endif
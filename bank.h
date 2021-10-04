#pragma once

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

bank *bank_alloc();
bool bank_free(bank *bnk);
bool bank_apply_log(bank *bnk, bank_audit_log_entry *log);
bool bank_init_from_file(bank *bnk, const char *filename);

bool bank_account_open(bank *bnk, char pin[4], unsigned long *number);
bool bank_account_close(bank *bnk, unsigned long number, char pin[4]);

bool bank_account_deposit(bank *bnk, unsigned long number, long amount);
bool bank_account_withdraw(bank *bnk, unsigned long number, long amount, char pin[4]);
bool bank_account_transfer(bank *bnk, unsigned long from, unsigned long to, long amount, char pin[4]);
bool bank_account_balance(bank *bnk, unsigned long number, long *balance);
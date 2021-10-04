#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    char data[32];
} bank_audit_log_raw;

typedef struct
{
    unsigned char kind;
    unsigned long number;
    char pin[4];
} bank_audit_log_account_open_close;

typedef struct
{
    unsigned char kind;
    unsigned long number;
    long amount;
} bank_audit_log_account_deposit_withdraw;

typedef struct
{
    unsigned char kind;
    unsigned long from;
    unsigned long to;
    long amount;
} bank_audit_log_account_transfer;

typedef union
{
    bank_audit_log_raw raw;
    bank_audit_log_account_open_close open_close;
    bank_audit_log_account_deposit_withdraw deposit_withdraw;
    bank_audit_log_account_transfer transfer;
} bank_audit_log_entry;

bool bank_audit_log_next(FILE *f, bank_audit_log_entry *log);
bool bank_audit_log_append(bank_audit_log_entry *log, const char *filename);
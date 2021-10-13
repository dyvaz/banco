#include "../bank_err.h"
#include "../bank.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1

#if DEBUG
#define _bank_print bank_print
#else
#define _bank_print(...)
#endif

#define invariant(expr, msg)                                              \
    if (!(expr))                                                          \
    {                                                                     \
        fprintf(stderr, "invariant failed at %s:%d", __FILE__, __LINE__); \
        if (msg != NULL)                                                  \
        {                                                                 \
            fprintf(stderr, " with message %s", msg);                     \
        }                                                                 \
        fprintf(stderr, "\n");                                            \
        exit(1);                                                          \
    }

#define ok(expr) invariant(expr, "expected true, got false")
#define nok(expr) ok(!expr)
#define randchar(mod) (char)(rand() % mod)
#define iserr(err)                \
    ok(bank_last_error() == err); \
    bank_clear_error();

void bank_print(bank *bnk)
{
    printf("bank(%p):\n", bnk);
    if (bnk == NULL)
    {
        return;
    }
    printf(" filename(%p): %s\n", bnk->filename, bnk->filename);
    printf(" accounts_size: %zu\n", bnk->accounts_size);
    printf(" accounts(%p):%s\n", bnk->accounts, bnk->accounts_size == 0 ? " []" : "");
    for (size_t i = 0; i < bnk->accounts_size; i++)
    {
        printf("  - number: %zu\n", bnk->accounts[i]->number);
        printf("    balance: %zd\n", bnk->accounts[i]->balance);
        printf("    pin: %c%c%c%c\n", bnk->accounts[i]->pin[0], bnk->accounts[i]->pin[1], bnk->accounts[i]->pin[2], bnk->accounts[i]->pin[3]);
        printf("    open: %s\n", bnk->accounts[i]->open ? "yes" : "no");
    }
}

int main(int argc, char **argv)
{
    char *filename = "bank.test.bin";
    char pins[3][4];
    unsigned long numbers[3] = {0};
    long balance;

gen_pins:
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            pins[i][j] = randchar(10) + 48;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = i + 1; j < 3; j++)
        {
            bool different = false;
            for (int k = 0; k < 3; k++)
            {
                if (pins[i][k] != pins[j][k])
                {
                    different = true;
                }
            }
            if (!different)
            {
                goto gen_pins;
            }
        }
    }

    bank *bnk = bank_alloc(filename);
    ok(bnk);

    _bank_print(bnk);

    bank_init_from_file(bnk, filename);

    for (int i = 0; i < 3; i++)
    {
        ok(bank_account_open(bnk, pins[i], &numbers[i]));
        ok(numbers[i]);
        for (int j = i - 1; j >= 0; j--)
        {
            ok(numbers[i] != numbers[j]);
        }
        _bank_print(bnk);
    }

    ok(bank_account_deposit(bnk, numbers[0], 1010));
    _bank_print(bnk);
    ok(bank_account_deposit(bnk, numbers[1], 1030));
    _bank_print(bnk);
    ok(bank_account_deposit(bnk, numbers[2], 1040));
    _bank_print(bnk);
    for (int i = 0; i < 10; i++)
    {
        ok(bank_account_deposit(bnk, numbers[0], 1));
        _bank_print(bnk);
    }

    ok(bank_account_withdraw(bnk, numbers[0], 20, pins[0]));
    _bank_print(bnk);
    ok(bank_account_withdraw(bnk, numbers[2], 540, pins[2]));
    _bank_print(bnk);
    ok(bank_account_transfer(bnk, numbers[1], numbers[2], 100, pins[1]));
    _bank_print(bnk);

    ok(bank_free(bnk));
    bnk = NULL;
    _bank_print(bnk);

    bnk = bank_alloc(filename);
    ok(bnk);
    _bank_print(bnk);

    ok(bank_init_from_file(bnk, filename));
    _bank_print(bnk);

    ok(bank_account_balance(bnk, numbers[0], &balance));
    _bank_print(bnk);
    ok(balance == 1000);
    ok(bank_account_balance(bnk, numbers[1], &balance));
    _bank_print(bnk);
    ok(balance == 930);
    ok(bank_account_balance(bnk, numbers[2], &balance));
    _bank_print(bnk);
    ok(balance == 600);

    nok(bank_account_close(bnk, numbers[2], pins[2]));
    iserr(BANK_ERR_BALANCE_NOT_ZERO);

    ok(bank_account_transfer(bnk, numbers[2], numbers[0], 600, pins[2]));
    _bank_print(bnk);
    ok(bank_account_close(bnk, numbers[2], pins[2]));
    _bank_print(bnk);

    nok(bank_account_deposit(bnk, numbers[2], 10));
    iserr(BANK_ERR_ACCOUNT_CLOSED);

    nok(bank_account_withdraw(bnk, numbers[0], 2000, pins[0]));
    iserr(BANK_ERR_INSUFFICIENT_BALANCE);

    nok(bank_account_withdraw(bnk, numbers[0], 1000, pins[1]));
    iserr(BANK_ERR_INVALID_PIN);

    nok(bank_account_deposit(bnk, numbers[0], -10));
    iserr(BANK_ERR_NEGATIVE_AMOUNT);

    ok(bank_free(bnk));

    nok(bank_init_from_file(NULL, filename));
    iserr(BANK_ERR_UNEXPECTED_NULL);

    return 0;
}
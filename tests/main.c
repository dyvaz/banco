#include "../bank_err.h"
#include "../bank.h"

#include <stdlib.h>
#include <stdio.h>

#define invariant(expr, msg)                                              \
    if (!expr)                                                            \
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

int main(int argc, char **argv)
{
    const char *filename = "bank.test.bin";
    bank *bank;
    char pins[3][4];
    unsigned long numbers[3] = {0};
    unsigned long balance;

gen_pins:
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            pins[i][j] = randchar(10);
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

    ok(bank_alloc(bank));

    ok(bank_init_from_file(bank, filename));

    for (int i = 0; i < 3; i++)
    {
        ok(bank_account_open(bank, pins[i], &numbers[i]));
        ok(numbers[i] != 0);
        for (int j = i - 1; j >= 0; j--)
        {
            ok(numbers[i] != numbers[j]);
        }
    }

    ok(bank_account_deposit(bank, numbers[0], 1010));
    ok(bank_account_deposit(bank, numbers[0], 1030));
    ok(bank_account_deposit(bank, numbers[0], 1040));
    for (int i = 0; i < 10; i++)
    {
        ok(bank_account_deposit(bank, numbers[0], 1));
    }

    ok(bank_account_withdraw(bank, numbers[0], 20, pins[0]));
    ok(bank_account_withdraw(bank, numbers[2], 540, pins[2]));
    ok(bank_account_transfer(bank, numbers[1], numbers[2], 100, pins[1]));

    ok(bank_free(bank));
    ok(bank == NULL);

    ok(bank_alloc(bank));

    ok(bank_account_balance(bank, numbers[0], &balance));
    ok(balance == 1000);
    ok(bank_account_balance(bank, numbers[1], &balance));
    ok(balance == 930);
    ok(bank_account_balance(bank, numbers[2], &balance));
    ok(balance == 600);

    nok(bank_account_close(bank, numbers[2], pins[2]));
    ok(bank_last_error() == BANK_ERR_BALANCE_NOT_ZERO);

    ok(bank_account_transfer(bank, numbers[2], numbers[0], 600, pins[2]));
    ok(bank_account_close(bank, numbers[2], pins[2]));

    nok(bank_account_withdraw(bank, numbers[0], 2000, pins[0]));
    ok(bank_last_error() == BANK_ERR_INSUFFICIENT_BALANCE);

    nok(bank_account_withdraw(bank, numbers[0], 1000, pins[1]));
    ok(bank_last_error() == BANK_ERR_INVALID_PIN);

    ok(bank_free(bank));
    ok(bank == NULL);

    return 0;
}
#include "./bank.h"
#include "./bank_err.h"
#include <stdlib.h>
#include <string.h>

int index_of(bank *b, unsigned long number)
{
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return -1;
  }
  for (int i = 0; i < b->accounts_size; i++)
  {
    if (b->accounts[i]->number == number)
    {
      return i;
    }
  }
  return -1;
}

bool check_pin(bank *b, char pin[4], int i)
{
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  for (int in = 0; in < 4; in++)
  {
    if (b->accounts[i]->pin[in] != pin[in])
    {
      return false;
    }
  }
  return true;
}

bank *bank_alloc(char *filename)
{
  bank *b;
  b = malloc(sizeof(bank));
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  b->accounts_size = 0;
  b->accounts = NULL;
  b->filename = filename;
  return b;
}

bool bank_free(bank *b)
{
  if (b == NULL)
  {
    return true;
  }
  if (b->accounts == NULL)
  {
    free(b);
    b = NULL;
    return true;
  }
  for (int i = 0; i < b->accounts_size; i++)
  {
    if (b->accounts[i] == NULL)
    {
      continue;
    }
    free(b->accounts[i]);
    b->accounts[i] = NULL;
  }
  free(b->accounts);
  b->accounts = NULL;
  free(b);
  b = NULL;
  return true;
}

bool bank_apply_log(bank *b, bank_audit_log_entry *log)
{
  if (b == NULL || log == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  account *acc;
  account **arrAcc;
  int i;
  int fIn;
  int tIn;
  switch (log->raw.data[0])
  {
  case 'o':
    acc = malloc(sizeof(account));
    if (acc == NULL)
    {
      set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
      return false;
    }
    acc->number = log->open_close.number;
    acc->balance = 0;
    memcpy(acc->pin, log->open_close.pin, 4);
    acc->open = true;
    arrAcc = realloc(b->accounts, sizeof(acc) * (b->accounts_size + 1));
    if (arrAcc == NULL)
    {
      set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
      return false;
    }
    b->accounts = arrAcc;
    b->accounts[b->accounts_size] = acc;
    b->accounts_size++;
    return true;
  case 'c':
    i = index_of(b, log->open_close.number);
    if (i == -1)
    {
      set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
      return false;
    }
    b->accounts[i]->open = false;
    return true;
  case 'd':
    i = index_of(b, log->deposit_withdraw.number);
    if (i == -1)
    {
      set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
      return false;
    }
    b->accounts[i]->balance += log->deposit_withdraw.amount;
    return true;
  case 'w':
    i = index_of(b, log->deposit_withdraw.number);
    if (i == -1)
    {
      set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
      return false;
    }
    b->accounts[i]->balance -= log->deposit_withdraw.amount;
    return true;
  case 't':
    fIn = index_of(b, log->transfer.from);
    tIn = index_of(b, log->transfer.to);
    if (fIn == -1 || tIn == -1)
    {
      set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
      return false;
    }
    b->accounts[fIn]->balance -= log->transfer.amount;
    b->accounts[tIn]->balance += log->transfer.amount;
    return true;
  default:
    set_err_if_none(BANK_ERR_UNKNOWN_LOG_ENTRY_KIND);
    return false;
  }
  set_err_if_none(BANK_ERR_CANT_APPLY_LOG_ENTRY);
  return false;
}

bool bank_init_from_file(bank *b, const char *filename)
{
  if (b == NULL || filename == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  FILE *f;
  f = fopen(filename, "r");
  if (f == NULL)
  {
    return false;
  }
  bank_audit_log_entry log;
  while (bank_audit_log_next(f, &log))
  {
    bank_apply_log(b, &log);
  }
  return true;
}

bool bank_account_open(bank *b, char pin[4], unsigned long *number)
{
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  *number = b->accounts_size + 1;
  if (index_of(b, *number) != -1)
  {
    return false;
  }
  for (int i = 0; i < 4; i++)
  {
    if (pin[i] < 48 || pin[i] > 57)
    {
      set_err_if_none(BANK_ERR_INVALID_PIN);
      return false;
    }
  }
  bank_audit_log_entry log;
  log.open_close.kind = 'o';
  log.open_close.number = *number;
  memcpy(log.open_close.pin, pin, 4);
  if (!bank_audit_log_append(&log, b->filename))
  {
    return false;
  }
  bank_apply_log(b, &log);
  return true;
}

bool bank_account_close(bank *b, unsigned long number, char pin[4])
{
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  int i = index_of(b, number);
  if (i == -1)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
    return false;
  }
  if (!check_pin(b, pin, i))
  {
    set_err_if_none(BANK_ERR_INVALID_PIN);
    return false;
  }
  if (b->accounts[i]->balance != 0)
  {
    set_err_if_none(BANK_ERR_BALANCE_NOT_ZERO);
    return false;
  }
  bank_audit_log_entry log;
  log.open_close.kind = 'c';
  log.open_close.number = number;
  memcpy(log.open_close.pin, pin, 4);
  if (!bank_audit_log_append(&log, b->filename))
  {
    return false;
  }
  bank_apply_log(b, &log);
  return true;
}

bool bank_account_deposit(bank *b, unsigned long number, long amount)
{
  if (amount <= 0)
  {
    set_err_if_none(BANK_ERR_NEGATIVE_AMOUNT);
    return false;
  }
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  int i = index_of(b, number);
  if (i == -1)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
    return false;
  }
  if (!b->accounts[i]->open)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_CLOSED);
    return false;
  }
  bank_audit_log_entry log;
  log.deposit_withdraw.kind = 'd';
  log.deposit_withdraw.number = number;
  log.deposit_withdraw.amount = amount;
  if (!bank_audit_log_append(&log, b->filename))
  {
    return false;
  }
  bank_apply_log(b, &log);
  return true;
}

bool bank_account_withdraw(bank *b, unsigned long number, long amount, char pin[4])
{
  if (amount <= 0)
  {
    set_err_if_none(BANK_ERR_NEGATIVE_AMOUNT);
    return false;
  }
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  int i = index_of(b, number);
  if (i == -1)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
    return false;
  }

  if (!b->accounts[i]->open)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_CLOSED);
    return false;
  }
  if (!check_pin(b, pin, i))
  {
    set_err_if_none(BANK_ERR_INVALID_PIN);
    return false;
  }
  if (b->accounts[i]->balance < amount)
  {
    set_err_if_none(BANK_ERR_INSUFFICIENT_BALANCE);
    return false;
  }
  bank_audit_log_entry log;
  log.deposit_withdraw.kind = 'w';
  log.deposit_withdraw.number = number;
  log.deposit_withdraw.amount = amount;
  if (!bank_audit_log_append(&log, b->filename))
  {
    return false;
  }
  bank_apply_log(b, &log);
  return true;
}

bool bank_account_transfer(bank *b, unsigned long from, unsigned long to, long amount, char pin[4])
{
  if (amount <= 0)
  {
    set_err_if_none(BANK_ERR_NEGATIVE_AMOUNT);
    return false;
  }
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  int fIn = index_of(b, from);
  int tIn = index_of(b, to);
  if (fIn == -1 || tIn == -1)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
    return false;
  }
  if (!b->accounts[fIn]->open || !b->accounts[tIn]->open)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_CLOSED);
    return false;
  }
  if (!check_pin(b, pin, fIn))
  {
    set_err_if_none(BANK_ERR_INVALID_PIN);
    return false;
  }
  if (b->accounts[fIn]->balance < amount)
  {
    set_err_if_none(BANK_ERR_INSUFFICIENT_BALANCE);
    return false;
  }
  bank_audit_log_entry log;
  log.transfer.kind = 't';
  log.transfer.from = from;
  log.transfer.to = to;
  log.transfer.amount = amount;
  if (!bank_audit_log_append(&log, b->filename))
  {
    return false;
  }
  bank_apply_log(b, &log);
  return true;
}

bool bank_account_balance(bank *b, unsigned long number, long *balance)
{
  if (b == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  int i = index_of(b, number);
  if (i == -1)
  {
    set_err_if_none(BANK_ERR_ACCOUNT_NOT_FOUND);
    return false;
  }
  *balance = 0;
  *balance = b->accounts[i]->balance;
  return true;
}

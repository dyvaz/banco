#include "./bank_log.h"
#include "./bank_err.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool bank_audit_log_append(bank_audit_log_entry *log, const char *filename)
{
  if (log == NULL || filename == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  FILE *ptr;
  ptr = fopen(filename, "a");
  if (ptr == NULL)
  {
    set_err_if_none(BANK_ERR_CANT_OPEN_FILE_WRITE);
    return false;
  }
  if (fwrite(log->raw.data, 1, 32, ptr) != 32)
  {
    set_err_if_none(BANK_ERR_CANT_WRITE_FILE);
    fclose(ptr);
    return false;
  }
  fclose(ptr);
  return true;
}

bool bank_audit_log_next(FILE *f, bank_audit_log_entry *log)
{
  if (log == NULL || f == NULL)
  {
    set_err_if_none(BANK_ERR_UNEXPECTED_NULL);
    return false;
  }
  if (fread(log->raw.data, 1, 32, f) != 32)
  {
    return false;
  }
  return true;
}
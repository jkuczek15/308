#ifndef BANK_FUNCTIONS
#define BANK_FUNCTIONS	

#include "Bank.h"
#include "utils.h"

int getBalance(int accountID, int totalAccounts);
int transaction(int accountID, int value, int totalAccounts);
int validation(int accountID, int totalAccounts);

#endif

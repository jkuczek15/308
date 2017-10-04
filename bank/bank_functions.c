#include "bank_functions.h"

int getBalance(int accountID, int totalAccounts)
{
  if(accountID <= 0){
    printf("Account ID must be a positive nonzero integer.\n");
    return -1;
  }// end if accountID is invalid

  if(accountID > totalAccounts){
    printf("There are currently only %d accounts. Account ID must be equal to or less than %d.\n", totalAccounts, totalAccounts);
    return -1;
  }// end if accountID > accounts
  
  return read_account(accountID);  	
}// end function getBalance


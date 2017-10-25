#include "bank_functions.h"

int getBalance(int accountID, int totalAccounts)
{
  if(validation(accountID, totalAccounts) == -1){
    return -1;
  }// end if basic validation failed
 
  return read_account(accountID);  	
}// end function getBalance

int transaction(int accountID, int value, int totalAccounts)
{
  int currentBalance = getBalance(accountID, totalAccounts);  
  int newBalance = currentBalance + value;

  if(currentBalance == -1){
    return -1;
  }// end if there was an error in validation
  
  if(newBalance < 0){
    return -2;
  }// end if the new balance is below zero 

  write_account(accountID, newBalance);
  return 0;
}// end function transaction

int validation(int accountID, int totalAccounts)
{
  if(accountID <= 0){
    return -1;
  }// end if accountID is invalid

  if(accountID > totalAccounts){
    return -1;
  }// end if accountID > accounts

  return 0;
}// end function validation

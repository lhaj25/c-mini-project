#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

struct Account {
    char username[50];
    char email[50];
    char password[50];
    float balance;
    char currency[10];
    char crypto[20];
    float amountToConvert;
};

struct Transaction {
    char type[50]; 
    char details[100];
    float amount;
    char date[30];
    struct Transaction *next;
};

struct Transaction *transactionStack = NULL;


void pushTransaction(const char *type, const char *details, float amount) {
    struct Transaction *newTransaction = (struct Transaction *)malloc(sizeof(struct Transaction));

    strcpy(newTransaction->type, type);
    strcpy(newTransaction->details, details);
    newTransaction->amount = amount;

   
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newTransaction->date, "%02d-%02d-%04d %02d:%02d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

    newTransaction->next = transactionStack;
    transactionStack = newTransaction;
}


void displayTransactionHistory() {
    if (transactionStack == NULL) {
        printf("\nNo transactions to display.\n");
        return;
    }

    printf("\n=== Transaction History ===\n");
    struct Transaction *current = transactionStack;
    while (current != NULL) {
        printf("Type: %s\nDetails: %s\nAmount: %.2f\nDate: %s\n\n",
               current->type, current->details, current->amount, current->date);
        current = current->next;
    }
}


int isValidEmail(const char *email) {
    return (strchr(email, '@') != NULL && strchr(email, '.') != NULL);
}

int isValidUsername(const char *username) {
    return (strlen(username) > 0 && strlen(username) < 50);
}

int isValidPassword(const char *password) {
    return (strlen(password) >= 6);
}

int isValidAmount(float amount) {
    return (amount > 0);
}

int isValidCurrency(const char *currency) {
    return (strcmp(currency, "Dollar") == 0 || strcmp(currency, "Euro") == 0);
}

int isValidCrypto(const char *crypto) {
    return (strcmp(crypto, "Bitcoin") == 0 || strcmp(crypto, "Ethereum") == 0 || strcmp(crypto, "Litecoin") == 0);
}

void createAccount(struct Account *acc) {
    do {
        printf("Enter your username: ");
        scanf("%s", (*acc).username);
        if (!isValidUsername((*acc).username)) {
            printf("Invalid username. Please try again.\n");
        }
    } while (!isValidUsername((*acc).username));

    do {
        printf("Enter your email: ");
        scanf("%s", (*acc).email);
        if (!isValidEmail((*acc).email)) {
            printf("Invalid email format. Please try again.\n");
        }
    } while (!isValidEmail((*acc).email));

    do {
        printf("Enter your password (minimum 6 characters): ");
        scanf("%s", (*acc).password);
        if (!isValidPassword((*acc).password)) {
            printf("Password must be at least 6 characters long. Please try again.\n");
        }
    } while (!isValidPassword((*acc).password));

    do {
        printf("Enter the amount of money you want to convert to cryptocurrency: ");
        scanf("%f", &(*acc).amountToConvert);
        if (!isValidAmount((*acc).amountToConvert)) {
            printf("Amount must be a positive number. Please try again.\n");
        }
    } while (!isValidAmount((*acc).amountToConvert));

    do {
        printf("Choose your currency (Dollar/Euro): ");
        scanf("%s", (*acc).currency);
        if (!isValidCurrency((*acc).currency)) {
            printf("Invalid currency. Please enter either 'Dollar' or 'Euro'.\n");
        }
    } while (!isValidCurrency((*acc).currency));

    do {
        printf("Choose a cryptocurrency (Bitcoin, Ethereum, Litecoin): ");
        scanf("%s", (*acc).crypto);
        if (!isValidCrypto((*acc).crypto)) {
            printf("Invalid cryptocurrency. Please choose from 'Bitcoin', 'Ethereum', or 'Litecoin'.\n");
        }
    } while (!isValidCrypto((*acc).crypto));

    (*acc).balance = 0;

    // Conversion rates for Dollar and Euro to Bitcoin (for simplicity)
    float conversionRate = 0.1;
    if (strcmp((*acc).currency, "Euro") == 0) {
        conversionRate = 0.09;
    }

    (*acc).balance += (*acc).amountToConvert * conversionRate;

    printf("\nAccount successfully created!\n");
    printf("Welcome, %s!\n", (*acc).username);
    printf("Your chosen currency is: %s\n", (*acc).currency);
    printf("Your chosen cryptocurrency is: %s\n", (*acc).crypto);
    printf("Your cryptocurrency balance is: %.2f coins\n", (*acc).balance);

    saveAccountToFile(acc);
}


void saveAccountToFile(struct Account *acc) {
    FILE *file = fopen("accounts.txt", "a");
    if (file == NULL) {
        file = fopen("accounts.txt", "w");
        if (file == NULL) {
            printf("Error opening file for writing!\n");
            return;
        }
    }
    fprintf(file, "%s %s %s %.2f %.2f %s %s\n",
            (*acc).username, (*acc).email, (*acc).password,
            (*acc).balance, (*acc).amountToConvert,
            (*acc).currency, (*acc).crypto);

    fclose(file);
}


int loadAccountFromFile(struct Account *acc, const char *username) {
    FILE *file = fopen("accounts.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 0;
    }

    while (fscanf(file, "%s %s %s %f %f %s %s",
                  (*acc).username, (*acc).email, (*acc).password,
                  &(*acc).balance, &(*acc).amountToConvert,
                  (*acc).currency, (*acc).crypto) == 7) {
        if (strcmp((*acc).username, username) == 0) {
            fclose(file);
            return 1; 
        }
    }

    fclose(file);
    return 0; 
}


void updateAccountInFile(struct Account *acc) {
    FILE *file = fopen("accounts.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (file == NULL || tempFile == NULL) {
        printf("Error opening files!\n");
        return;
    }

    struct Account tempAcc;
    while (fscanf(file, "%s %s %s %f %f %s %s",
                  tempAcc.username, tempAcc.email, tempAcc.password,
                  &tempAcc.balance, &tempAcc.amountToConvert,
                  tempAcc.currency, tempAcc.crypto) == 7) {

        if (strcmp(tempAcc.username, (*acc).username) == 0) {
            fprintf(tempFile, "%s %s %s %.2f %.2f %s %s\n",
                    (*acc).username, (*acc).email, (*acc).password,
                    (*acc).balance, (*acc).amountToConvert,
                    (*acc).currency, (*acc).crypto);
        } else {
            fprintf(tempFile, "%s %s %s %.2f %.2f %s %s\n",
                    tempAcc.username, tempAcc.email, tempAcc.password,
                    tempAcc.balance, tempAcc.amountToConvert,
                    tempAcc.currency, tempAcc.crypto);
        }
    }

    fclose(file);
    fclose(tempFile);
    remove("accounts.txt");
    rename("temp.txt", "accounts.txt");
}


void tradeCryptocurrency(struct Account *acc) {
    int action;
    float amount;
    float cryptoPrice = 0.0;

    if (strcmp(acc->crypto, "Bitcoin") == 0) {
        cryptoPrice = 50000.0;
    } else if (strcmp(acc->crypto, "Ethereum") == 0) {
        cryptoPrice = 3000.0;
    } else if (strcmp(acc->crypto, "Litecoin") == 0) {
        cryptoPrice = 200.0;
    }

    printf("\nCurrent price of 1 %s: %.2f %s\n", acc->crypto, cryptoPrice, acc->currency);
    printf("Your current balance in %s: %.2f coins\n", acc->crypto, acc->balance);

    printf("\n1. Buy cryptocurrency\n");
    printf("2. Sell cryptocurrency\n");
    printf("Choice: ");
    scanf("%d", &action);

    if (action == 1) {
        printf("Enter amount of cryptocurrency to buy: ");
        scanf("%f", &amount);

        if (amount <= 0) {
            printf("Invalid amount.\n");
            return;
        }

        float totalCost = amount * cryptoPrice;

        if (acc->balance >= totalCost) {
            acc->balance -= totalCost;
            printf("Bought %.2f %s for %.2f %s\n", amount, acc->crypto, totalCost, acc->currency);

            char details[100];
            sprintf(details, "Bought %.2f %s", amount, acc->crypto);
            pushTransaction("Buy", details, totalCost);

            updateAccountInFile(acc);
        } else {
            printf("Insufficient balance.\n");
        }
    } else if (action == 2) {
        printf("Enter amount of cryptocurrency to sell: ");
        scanf("%f", &amount);

        if (amount <= 0 || amount > acc->balance) {
            printf("Invalid amount.\n");
            return;
        }

        float totalEarnings = amount * cryptoPrice;
        acc->balance += totalEarnings;

        printf("Sold %.2f %s for %.2f %s\n", amount, acc->crypto, totalEarnings, acc->currency);

        char details[100];
        sprintf(details, "Sold %.2f %s", amount, acc->crypto);
        pushTransaction("Sell", details, totalEarnings);

        updateAccountInFile(acc);
    } else {
        printf("Invalid choice.\n");
    }
}


void buyServices(struct Account *acc) {
    int serviceChoice;
    float serviceCost;

    printf("\nAvailable services:\n");
    printf("1. Premium Subscription - $10\n");
    printf("2. Customer Support - $5\n");
    printf("3. Extra Storage - $15\n");
    printf("4. Exit\n");
    printf("Choose a service: ");
    scanf("%d", &serviceChoice);

    if (serviceChoice == 1) {
        serviceCost = 10.0;
    } else if (serviceChoice == 2) {
        serviceCost = 5.0;
    } else if (serviceChoice == 3) {
        serviceCost = 15.0;
    } else if (serviceChoice == 4) {
        return;
    } else {
        printf("Invalid choice.\n");
        return;
    }

    if (acc->balance >= serviceCost) {
        acc->balance -= serviceCost;
        printf("Successfully purchased the service! Remaining balance: %.2f\n", acc->balance);

        char details[100];
        sprintf(details, "Bought service costing %.2f", serviceCost);
        pushTransaction("Service", details, serviceCost);

        updateAccountInFile(acc);
    } else {
        printf("Insufficient balance for this service.\n");
    }
}


void displayMenu(struct Account *acc) {
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. Trade cryptocurrency\n");
        printf("2. Buy services\n");
        printf("3. Check balance\n");
        printf("4. View transaction history\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                tradeCryptocurrency(acc);
                break;
            case 2:
                buyServices(acc);
                break;
            case 3:
                printf("Current balance: %.2f\n", acc->balance);
                break;
            case 4:
                displayTransactionHistory();
                break;
            case 5:
                printf("Exiting...\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

int main() {
    int mainChoice;
    struct Account userAccount;
    do {
        printf("\n=== Cryptocurrency Platform ===\n");
        printf("1. Create new account\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &mainChoice);

        if (mainChoice == 1) {
            createAccount(&userAccount);
            displayMenu(&userAccount); 
        } else if (mainChoice == 2) {
            char username[50], password[50];
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);

            if (loadAccountFromFile(&userAccount, username)) {
                if (strcmp(userAccount.password, password) == 0) {
                    printf("Login successful!\n");
                    displayMenu(&userAccount); 
                } else {
                    printf("Invalid credentials!\n");
                }
            } else {
                printf("Account not found!\n");
            }
        } else if (mainChoice == 3) {
            printf("Exiting program.\n");
        } else {
            printf("Invalid choice.\n");
        }
    } while (mainChoice != 3); 

    return 0;
}

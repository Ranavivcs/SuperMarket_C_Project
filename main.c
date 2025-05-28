#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"
#include "utils.h"

int main() {
    SuperMarket market;
    srand((unsigned int)time(NULL));

    int compressedMode;
    printf("Enter 1 for compressed mode, 0 for uncompressed mode: ");
    scanf("%d", &compressedMode);

    const char* superFile = compressedMode ? "super_compress.bin" : SUPER_FILE_NAME;

    // Initialize the supermarket with macros
    CHECK_MSG_RETURN_0(initSuperMarket(&market, superFile, CUSTOMER_FILE_NAME, compressedMode), "Error initializing SuperMarket.");

    int option;
    int stop = 0;

    do {
        option = menu();
        switch (option) {
        case eShowSuperMarket:
            printSuperMarket(&market);
            break;

        case eAddProduct:
            if (!addProduct(&market))
                printMessage("Error adding product", NULL);
            break;

        case eAddCustomer:
            if (!addCustomer(&market))
                printMessage("Error adding customer", NULL);
            break;

        case eCustomerDoShopping:
            if (!doShopping(&market))
                printMessage("Error in shopping", NULL);
            break;

        case ePrintCart:
            doPrintCart(&market);
            break;

        case eCustomerManageShoppingCart:
            if (!manageShoppingCart(&market))
                printMessage("Error in shopping cart management", NULL);
            break;

        case eSortProducts:
            sortProducts(&market);
            break;

        case eSearchProduct:
            findProduct(&market);
            break;

        case ePrintProductByType:
            printProductByType(&market);
            break;

        case EXIT:
            printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
            stop = 1;
            break;

        default:
            printMessage("Wrong option", NULL);
            break;
        }
    } while (!stop);

    handleCustomerStillShoppingAtExit(&market);

    // Save the supermarket with macros
    CHECK_MSG_RETURN_0(saveSuperMarketToFile(&market, superFile, CUSTOMER_FILE_NAME, compressedMode),
        "Error saving supermarket to file");

    freeMarket(&market);

    return 1;
}

int menu() {
    int option;
    printf("\n");
    printf("Please choose one of the following options\n");
    for (int i = 0; i < eNofOptions; i++)
        printf("%d - %s\n", i, menuStrings[i]);
    printf("%d - Quit\n", EXIT);
    scanf("%d", &option);
    // Clean buffer
    char tav;
    scanf("%c", &tav);
    return option;
}



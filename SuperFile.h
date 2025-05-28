#pragma once

#include <stdio.h>
#include "Supermarket.h"
#include "Customer.h"

int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName, int compressedMode);
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName, int compressedMode);

int	saveCustomersToTextFile(const Customer* customerArr, int customerCount, const char* customersFileName);
Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount);
void freeCustomerCloseFile(Customer* customerArr, int customerIndex, FILE* fp);

int saveCompressedFile(const char* filename, int productCount, const char* companyName);
int readCompressedFile(const char* filename, int* productCount, char** companyName);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "General.h"
#include "FileHelper.h"
#include "SuperFile.h"
#include "Product.h"
#include "myMacros.h"
#include "utils.h"

#define MAX_PRODUCT_COUNT 1023
#define MAX_NAME_LENGTH 63


int saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName, int compressedMode) {
	FILE* fp = fopen(fileName, compressedMode ? "wb" : "wb");
	CHECK_MSG_RETURN_0(fp == NULL, "Error opening supermarket file to write."); //first usage of CHECK_MSG_RETURN

	if (compressedMode) {
		// Save the compressed data
		if (!saveCompressedFile(fp, pMarket->productCount, pMarket->name)) {
			CLOSE_RETURN_0(fp); 
		}

		// Save each product in compressed format
		for (int i = 0; i < pMarket->productCount; i++) {
			if (!saveCompressedProduct(fp, pMarket->productArr[i])) {
				CLOSE_RETURN_0(fp); 
			}
		}
	}
	else {
		// Save uncompressed data
		if (!writeStringToFile(pMarket->name, fp, "Error writing supermarket name\n")) {
			CLOSE_RETURN_0(fp); 
		}

		if (!writeIntToFile(pMarket->productCount, fp, "Error writing product count\n")) {
			CLOSE_RETURN_0(fp);
		}

		for (int i = 0; i < pMarket->productCount; i++) {
			if (!saveProductToFile(pMarket->productArr[i], fp)) {
				CLOSE_RETURN_0(fp);
			}
		}
	}

	fclose(fp);

	// Save customers to a separate file (always uncompressed for simplicity)
	saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);

	return 1;
}


int loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName, int compressedMode) {
	FILE* fp = fopen(fileName, compressedMode ? "rb" : "rb");
	CHECK_MSG_RETURN_0(fp == NULL, "Error opening supermarket file to read.");

	if (compressedMode) {
		int productCount;
		char* companyName;

		// Load compressed data
		if (!readCompressedFile(fp, &productCount, &companyName)) {
			CLOSE_RETURN_0(fp);
		}

		pMarket->productCount = productCount;
		pMarket->name = companyName;

		// Allocate space for products
		pMarket->productArr = (Product**)malloc(productCount * sizeof(Product*));
		CHECK_RETURN_0(pMarket->productArr == NULL);

		for (int i = 0; i < productCount; i++) {
			pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
			if (!pMarket->productArr[i]) {
				FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
			}

			if (!loadCompressedProduct(fp, pMarket->productArr[i])) {
				FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
			}
		}

	}
	else {
		// Load uncompressed data
		pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
		CHECK_RETURN_0(pMarket->name == NULL);

		int count;
		if (!readIntFromFile(&count, fp, "Error reading product count\n")) {
			FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
		}

		pMarket->productArr = (Product**)malloc(count * sizeof(Product*));
		CHECK_RETURN_0(pMarket->productArr == NULL);

		pMarket->productCount = count;

		for (int i = 0; i < count; i++) {
			pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
			if (!pMarket->productArr[i]) {
				FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
			}

			if (!loadProductFromFile(pMarket->productArr[i], fp)) {
				FREE_CLOSE_FILE_RETURN_0(fp, pMarket->name);
			}
		}
	}

	fclose(fp);

	// Load customers from a separate file (always uncompressed for simplicity)
	pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
	CHECK_RETURN_0(pMarket->customerArr == NULL);

	return 1;
}


int	saveCustomersToTextFile(const Customer* customerArr, int customerCount, const char* customersFileName)
{
	FILE* fp;

	fp = fopen(customersFileName, "w");
	if (!fp) {
		printf("Error opening customers file to write\n");
		return 0;
	}

	fprintf(fp, "%d\n", customerCount);
	for (int i = 0; i < customerCount; i++)
		customerArr[i].vTable.saveToFile(&customerArr[i], fp);

	fclose(fp);
	return 1;
}

Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount)
{
	FILE* fp;

	fp = fopen(customersFileName, "r");
	if (!fp) {
		printf("Error open customers file to write\n");
		return NULL;
	}

	Customer* customerArr = NULL;
	int customerCount;

	fscanf(fp, "%d\n", &customerCount);

	if (customerCount > 0)
	{
		customerArr = (Customer*)calloc(customerCount, sizeof(Customer)); //cart will be NULL!!!
		if (!customerArr)
		{
			fclose(fp);
			return NULL;
		}

		for (int i = 0; i < customerCount; i++)
		{
			if (!loadCustomerFromFile(&customerArr[i], fp))
			{
				freeCustomerCloseFile(customerArr, i, fp);
				return NULL;
			}
		}
	}

	fclose(fp);
	*pCount = customerCount;
	return customerArr;
}


void freeCustomerCloseFile(Customer* customerArr, int count, FILE* fp)
{
	for (int i = 0; i < count; i++)
	{
		free(customerArr[i].name);
		customerArr[i].name = NULL;
		if (customerArr[i].pDerivedObj)
		{
			free(customerArr[i].pDerivedObj);
			customerArr[i].pDerivedObj = NULL;
		}
	}
	free(customerArr);
	fclose(fp);
}


int saveCompressedFile(const char* filename, int productCount, const char* companyName)
{
	if (productCount > MAX_PRODUCT_COUNT || strlen(companyName) > MAX_NAME_LENGTH)
	{
		fprintf(stderr, "Error: Product count or company name length exceeds the limit.\n");
		return 0;
	}

	FILE* file = fopen(filename, "wb");

	CHECK_MSG_RETURN_0(file, "Error Opening file for writing\n");

	//----------- create first two bytes for metadata ---------

	unsigned short metadata = 0;
	metadata |= (productCount & 0x03FF); //allocate 10 bits for product count
	metadata |= (strlen(companyName) & 0x003F) << 10; //allocate 6 bits for name length

	//----------- write metadata to the file ---------

	CHECK_RETURN_0(fwrite(&metadata, sizeof(metadata), 1, file));

	//----------- write the company name without null terminator ---------

	CHECK_RETURN_0(fwrite(companyName, sizeof(char), strlen(companyName), file));

	fclose(file);

	printf("Compressed file saved successfully.\n");
	return 1;
}

int readCompressedFile(const char* filename, int* productCount, char** companyName)
{
	FILE* file = fopen(filename, "rb");

	CHECK_MSG_RETURN_0(file, "Error Opening file for reading\n");

	//read the metaData (2 bytes)

	unsigned short metadata;

	CHECK_RETURN_0(fread(&metadata, sizeof(metadata), 1, file));

	//----------- Extract productCount(10 bits) and nameLength(6 bits) ---------
	*productCount = metadata & 0x03FF; // First 10 bits;
	int nameLength = (metadata >> 10) & 0x003F; // Next 6 bits

	//----------- Allocate memory for the companyName ---------

	*companyName = (char*)malloc((nameLength + 1) * sizeof(char));

	CHECK_MSG_RETURN_0(*companyName, "Error allocating memory for company name.\n");

	//----------- read companyName from the File ---------

	if (fread(*companyName, sizeof(char), nameLength, file) != nameLength)
	{
		fprintf(stderr, "Error reading company name from file.\n");
		free(*companyName);
		fclose(file);
		return 0;
	}

	(*companyName)[nameLength] = '\0'; // Null-terminate the string

	CLOSE_RETURN_0(file);

	printf("Compressed file read successfully.\n");
	return 1;
}












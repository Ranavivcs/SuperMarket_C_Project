#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "FileHelper.h"

#define MIN_DIG 3
#define MAX_DIG 5

#define MAX_NAME_LENGTH 15
#define BARCODE_LENGTH 7


static const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };
static const char* typePrefix[eNofProductType] = { "FV", "FR", "FZ", "SH" };


void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	generateBarcode(pProduct);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	initDate(&pProduct->expiryDate);
	pProduct->price = getPositiveFloat("Enter product price\t");
	pProduct->count = getPositiveInt("Enter product number of items\t");
}

void initProductName(Product* pProduct)
{
	do {
		printf("enter product name up to %d chars\n", NAME_LENGTH);
		myGets(pProduct->name, sizeof(pProduct->name), stdin);
	} while (checkEmptyString(pProduct->name));
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (fwrite(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error saving product to file\n");
		return 0;
	}
	return 1;
}

int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (fread(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error reading product from file\n");
		return 0;
	}
	return 1;
}

int	compareProductsByName(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return strcmp(pProd1->name, pProd2->name);
}

int	compareProductsByCount(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	return pProd1->count - pProd2->count;
}

int	compareProductsByPrice(const void* prod1, const void* prod2)
{
	const Product* pProd1 = *(Product**)prod1;
	const Product* pProd2 = *(Product**)prod2;

	if (pProd1->price > pProd2->price)
		return 1;
	if (pProd1->price < pProd2->price)
		return -1;
	return 0;
}

void printProduct(const Product* pProduct)
{
	char* dateStr = getDateStr(&pProduct->expiryDate);
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %13d %7s %15s\n", typeStr[pProduct->type], pProduct->price, pProduct->count, " ", dateStr);
	free(dateStr);
}

void printProductPtr(void* v1)
{
	Product* pProduct = *(Product**)v1;
	printProduct(pProduct);
}

void generateBarcode(Product* pProd)
{
	char temp[BARCODE_LENGTH + 1];
	int barcodeNum;

	strcpy(temp, getProductTypePrefix(pProd->type));
	do {
		barcodeNum = MIN_BARCODE + rand() % (RAND_MAX - MIN_BARCODE + 1); //Minimum 5 digits
	} while (barcodeNum > MAX_BARCODE);
	
	sprintf(temp + strlen(temp), "%d", barcodeNum);

	strcpy(pProd->barcode, temp);
}

void getBarcodeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg, "Code should be of %d length exactly\n"
				 "Must have %d type prefix letters followed by a %d digits number\n"
				 "For example: FR20301",
				 BARCODE_LENGTH, PREFIX_LENGTH, BARCODE_DIGITS_LENGTH);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts("Invalid barcode length");
			ok = 0;
		}
		else
		{
			//check first PREFIX_LENGTH letters are upper case and valid type prefix
			char* typeSubStr = (char*)malloc(PREFIX_LENGTH + 1);
			if (!typeSubStr)
				return;
			strncpy(typeSubStr, temp, PREFIX_LENGTH);
			typeSubStr[PREFIX_LENGTH] = '\0';
			int prefixOk = 0;
			int i;

			for (i = 0; i < eNofProductType; i++)
			{
				if (strcmp(typeSubStr, typePrefix[i]) == 0)
				{
					prefixOk = 1;
					break; //found valid type prefix
				}
			}

			free(typeSubStr); //free the allocated memory

			if (!prefixOk)
			{
				puts("Invalid type prefix");
				ok = 0;
			}
			else
			{
				for (i = PREFIX_LENGTH; i < BARCODE_LENGTH; i++)
				{
					if (!isdigit(temp[i]))
					{
						puts("Only digits after type prefix\n");
						puts(msg);
						ok = 0;
						break;
					}
					digCount++;
				}

				if (digCount != BARCODE_DIGITS_LENGTH)
				{
					puts("Incorrect number of digits");
					ok = 0;
				}
			}
		}
	} while (!ok);

	strcpy(code, temp);
}

eProductType getProductType()
{
	int option;

	printf("\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);

	getchar();

	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

const char* getProductTypePrefix(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typePrefix[type];
}

int isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

void updateProductCount(Product* pProduct)
{
	int count;

	do {
		printf("How many items to add to stock? ");
		scanf("%d", &count);
	} while (count < 1);

	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}

int saveCompressedProduct(FILE* file, const Product* product) {
	if (strlen(product->name) > MAX_NAME_LENGTH) {
		fprintf(stderr, "Error: Product name exceeds maximum length of 15.\n");
		return 0;
	}

	unsigned char buffer[6] = { 0 };

	// ----- Pack barcode digits into buffer 0 and buffer 1 -----
	for (int i = 0; i < 5; i++) {
		int digit = product->barcode[i + 2] - '0'; // Skip first 2 chars
		buffer[i / 2] |= digit << ((1 - (i % 2)) * 4); // Pack 4 bits per digit
	}

	// ----- Pack last barcode digit, product type, name length into buffer 2 -----
	buffer[2] = ((product->barcode[6] - '0') & 0xF); // Last digit of barcode
	buffer[2] |= (product->type & 0x3) << 4;         // Product type (2 bits)
	buffer[2] |= (strlen(product->name) & 0xF) << 6; // Name length (4 bits)

	// ----- Pack quantity into buffer 3 -----
	buffer[3] = (unsigned char)(product->count & 0xFF); // 8 bits for count

	// ----- Pack price into buffer 4 and buffer 5 -----
	unsigned int shekels = (unsigned int)(product->price);
	unsigned int agorot = (unsigned int)((product->price - shekels) * 100);
	buffer[4] = (unsigned char)((agorot & 0x7F) | ((shekels & 0x1) << 7)); // 7 bits agorot + 1 bit shekel
	buffer[5] = (unsigned char)((shekels >> 1) & 0xFF); // Remaining 8 bits for shekels

	// ----- Write buffer to file -----
	if (fwrite(buffer, sizeof(buffer), 1, file) != 1) {
		fprintf(stderr, "Error writing compressed product to file.\n");
		return 0;
	}

	// ----- Write product name to file -----
	if (fwrite(product->name, sizeof(char), strlen(product->name), file) != strlen(product->name)) {
		fprintf(stderr, "Error writing product name to file.\n");
		return 0;
	}

	return 1;
}




int loadCompressedProduct(FILE* file, Product* product) {
	unsigned char buffer[6] = { 0 };

	// ----- Read buffer from file -----
	if (fread(buffer, sizeof(buffer), 1, file) != 1) {
		fprintf(stderr, "Error reading compressed product from file.\n");
		return 0;
	}

	// ----- Unpack barcode digits from buffer 0 and buffer 1 -----
	char barcode[BARCODE_LENGTH + 1] = { 0 }; // Ensure null-terminated
	const char* typePrefix = getProductTypePrefix(product->type);
	barcode[0] = typePrefix[0]; // First character of type prefix
	barcode[1] = typePrefix[1]; // Second character of type prefix
	for (int i = 0; i < 5; i++) {
		int digit = (buffer[i / 2] >> ((1 - (i % 2)) * 4)) & 0xF; // Extract 4 bits
		barcode[i + 2] = digit + '0';                             // Convert to char
	}
	strcpy(product->barcode, barcode);

	// ----- Unpack product type and name length from buffer 2 -----
	product->type = (buffer[2] >> 4) & 0x3;          // Extract product type (2 bits)
	int nameLength = (buffer[2] >> 6) & 0xF;         // Extract name length (4 bits)

	// ----- Unpack quantity from buffer 3 -----
	product->count = buffer[3];                      // Extract quantity (8 bits)

	// ----- Unpack price from buffer 4 and buffer 5 -----
	unsigned int agorot = buffer[4] & 0x7F;          // Extract 7 bits for agorot
	unsigned int shekels = ((buffer[4] >> 7) & 0x1) | // Extract 1 bit of shekels
		(buffer[5] << 1);         // Extract remaining 8 bits of shekels
	product->price = shekels + (agorot / 100.0);     // Combine shekels and agorot

	// ----- Read product name from file -----
	if (fread(product->name, sizeof(char), nameLength, file) != (size_t)nameLength) {
		fprintf(stderr, "Error reading product name from file.\n");
		return 0;
	}
	product->name[nameLength] = '\0'; // Null-terminate the string

	return 1;
}
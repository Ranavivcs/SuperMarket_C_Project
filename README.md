# SuperMarket C Project

A modular Supermarket Management System written in C.  
This project handles product inventory, customer data, shopping carts, and file-based data persistence.

## 🛠 Features

- Add, edit, and remove products
- Manage club members and customer records
- Shopping cart system for customers
- File I/O for saving and loading supermarket data (`SuperMarket.bin`)
- Console-based user interface

 ## 📁 File Structure Overview

```
SuperMarket/
├── Customer.c / Customer.h         # Customer data & operations
├── Product.c / Product.h           # Product inventory logic
├── ShoppingCart.c / ShoppingCart.h # Shopping cart system
├── ShoppingItem.c / ShoppingItem.h # Items inside cart
├── General.c / General.h           # General helper functions
├── FileHelper.c / FileHelper.h     # File reading/writing
├── Date.c / Date.h                 # Date handling
├── main.c                          # Entry point of the program
├── SuperMarket.bin                 # Binary file with saved data
├── .gitignore
└── README.md
```


## 💻 How to Compile

Use `gcc` or any C compiler:

```bash
gcc -Wall -std=c99 -o SuperMarket main.c Customer.c Product.c ShoppingCart.c ShoppingItem.c FileHelper.c General.c Date.c Utils.c
🚀 Run the Program
bash
Copy
Edit
./SuperMarket
Make sure SuperMarket.bin exists or the program will create it automatically.

👨‍💻 Author
Ran Aviv
Computer Science Student @ Afeka - College of Engineering


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* -----------------------------
   Data structure definitions
   ----------------------------- */

typedef struct Product {
    int id;
    char name[50];
    int quantity;
    double price;
} Product;

typedef struct BSTNode {
    Product product;
    struct BSTNode *left, *right;
} BSTNode;

typedef struct CartNode {
    int id;
    char name[50];
    int quantity;
    double price;
    struct CartNode *next;
} CartNode;

/* -----------------------------
   Function Declarations
   ----------------------------- */

// BST (Inventory) Functions
BSTNode* createBSTNode(Product p);
BSTNode* bstSearch(BSTNode *root, int id);
BSTNode* bstInsert(BSTNode *root, Product p);
void inorderDisplay(BSTNode *root);
int bstUpdateQuantity(BSTNode *root, int id, int soldQty);
void freeBST(BSTNode *root);

// Cart Functions
CartNode* createCartNode(int id, const char *name, int qty, double price);
void cartAddItem(int id, const char *name, int qty, double price);
void clearCart();
void displayCart();

// Functional Modules
BSTNode* addProductToInventory(BSTNode *inventory);
void displayInventory(BSTNode *inventory);
void addToCart(BSTNode *inventory);
void generateBill(BSTNode *inventory);
BSTNode* seedInventory(BSTNode *inventory);



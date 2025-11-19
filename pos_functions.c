// pos_system_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "pos.h"    // Struct definitions

/* -----------------------------
   Utility helpers
   ----------------------------- */

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != EOF && c != '\n');
}

static char *str_trim_newline(char *s) {
    if (!s) return s;
    size_t n = strlen(s);
    if (n > 0 && s[n-1] == '\n') s[n-1] = '\0';
    return s;
}

static void *xmalloc(size_t n) {
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "Fatal: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* -----------------------------
   Globals
   ----------------------------- */

CartNode *cartHead = NULL;

/* -----------------------------
   BST (Inventory) Functions
   ----------------------------- */

BSTNode* createBSTNode(Product p) {
    BSTNode *node = xmalloc(sizeof(BSTNode));
    node->product = p;
    node->left = node->right = NULL;
    return node;
}

BSTNode* bstSearch(BSTNode *root, int id) {
    if (!root) return NULL;
    if (id == root->product.id) return root;
    if (id < root->product.id) return bstSearch(root->left, id);
    return bstSearch(root->right, id);
}

BSTNode* bstInsert(BSTNode *root, Product p) {
    if (!root) return createBSTNode(p);

    if (p.id < root->product.id)
        root->left = bstInsert(root->left, p);
    else if (p.id > root->product.id)
        root->right = bstInsert(root->right, p);
    else {
        // Update existing product
        root->product.quantity = p.quantity;
        root->product.price = p.price;
        size_t nprod = sizeof(root->product.name);
        strncpy(root->product.name, p.name, nprod - 1);
        root->product.name[nprod - 1] = '\0';
    }

    return root;
}

void inorderDisplay(BSTNode *root) {
    if (!root) return;
    inorderDisplay(root->left);
    printf("ID: %d | Name: %s | Qty: %d | Price: %.2f\n",
           root->product.id, root->product.name,
           root->product.quantity, root->product.price);
    inorderDisplay(root->right);
}

void freeBST(BSTNode *root) {
    if (!root) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

/* -----------------------------
   Cart (Linked List) Functions
   ----------------------------- */

CartNode* createCartNode(int id, const char *name, int qty, double price) {
    CartNode *node = malloc(sizeof(CartNode));
    if (!node) {
        printf("Memory error!\n");
        exit(1);
    }
    node->id = id;
    size_t nname = sizeof(node->name);
    strncpy(node->name, name, nname - 1);
    node->name[nname - 1] = '\0';
    node->quantity = qty;
    node->price = price;
    node->next = NULL;
    return node;
}

void cartAddItem(int id, const char *name, int qty, double price) {
    if (qty <= 0) {
        printf(" Invalid quantity.\n");
        return;
    }

    CartNode *cur = cartHead, *prev = NULL;

    while (cur) {
        if (cur->id == id) {
            cur->quantity += qty;
            printf("Updated cart: %s new qty = %d\n", cur->name, cur->quantity);
            return;
        }
        prev = cur;
        cur = cur->next;
    }

    CartNode *newNode = createCartNode(id, name, qty, price);

    if (!prev)
        cartHead = newNode;
    else
        prev->next = newNode;

    printf("Added to cart: %s (qty %d)\n", name, qty);
}

void clearCart() {
    CartNode *cur = cartHead;
    while (cur) {
        CartNode *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    cartHead = NULL;
}

void displayCart() {
    if (!cartHead) {
        printf("Cart is empty.\n");
        return;
    }

    printf("\n--- Cart ---\n");
    CartNode *cur = cartHead;

    while (cur) {
        printf("ID: %d | Name: %s | Qty: %d | Unit Price: %.2f | Subtotal: %.2f\n",
               cur->id, cur->name, cur->quantity,
               cur->price, cur->quantity * cur->price);
        cur = cur->next;
    }

    printf("--- End Cart ---\n\n");
}

/* -----------------------------
   Cart Removal
   ----------------------------- */

// Forward declaration of input helper
static int read_int(const char *prompt, int *out_value, int positive_only);

// Helper: find a cart node by ID
static CartNode* cartFindById(int id) {
    CartNode *cur = cartHead;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

int cartRemoveItem(int id) {
    CartNode *cur = cartHead, *prev = NULL;
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next; else cartHead = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

void removeFromCart() {
    if (!cartHead) {
        printf("Cart is empty.\n");
        return;
    }
    // Show current cart so user can pick an ID
    displayCart();
    int id = 0;
    if (!read_int("Enter product ID to edit/remove: ", &id, 1)) return;

    CartNode *item = cartFindById(id);
    if (!item) {
        printf("Item with ID %d not found in cart.\n", id);
        return;
    }

    printf("Selected: %s | Current Qty: %d | Unit Price: %.2f\n",
           item->name, item->quantity, item->price);

    char buf[64];
    printf("Choose action - (c) change quantity, (r) remove item, (n) cancel: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    char opt = buf[0];

    if (opt == 'c' || opt == 'C') {
        int newQty = 0;
        if (!read_int("Enter new quantity (0 to remove): ", &newQty, 0)) return;
        if (newQty < 0) {
            printf("Quantity cannot be negative.\n");
            return;
        } else if (newQty == 0) {
            cartRemoveItem(id);
            printf("Removed item with ID %d from cart.\n", id);
        } else {
            item->quantity = newQty;
            printf("Updated quantity. New Qty: %d\n", item->quantity);
        }
    } else if (opt == 'r' || opt == 'R') {
        cartRemoveItem(id);
        printf("Removed item with ID %d from cart.\n", id);
    } else {
        printf("No changes made.\n");
    }
}

/* -----------------------------
   Input Helpers
   ----------------------------- */

static int read_int(const char *prompt, int *out_value, int positive_only) {
    char buf[128];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;

        // Skip leading whitespace and blank lines (handles leftover newlines)
        char *s = buf;
        while (isspace((unsigned char)*s)) s++;
        if (*s == '\0') continue;

        char *endptr;
        long v = strtol(s, &endptr, 10);

        if (endptr == s) {
            printf(" Invalid input. Enter an integer.\n");
            continue;
        }

        // Allow trailing whitespace
        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0') {
            printf(" Invalid input. Enter an integer.\n");
            continue;
        }

        if (positive_only && v <= 0) {
            printf(" Must be positive.\n");
            continue;
        }

        *out_value = (int)v;
        return 1;
    }
}

static int read_double(const char *prompt, double *out_value, int positive_only) {
    char buf[128];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;

        // Skip leading whitespace and blank lines
        char *s = buf;
        while (isspace((unsigned char)*s)) s++;
        if (*s == '\0') continue;

        char *endptr;
        double d = strtod(s, &endptr);

        if (endptr == s) {
            printf(" Invalid number.\n");
            continue;
        }

        while (isspace((unsigned char)*endptr)) endptr++;
        if (*endptr != '\0') {
            printf(" Invalid number.\n");
            continue;
        }

        if (positive_only && d <= 0) {
            printf(" Must be positive.\n");
            continue;
        }

        *out_value = d;
        return 1;
    }
}

/* -----------------------------
   Add Product to Inventory
   ----------------------------- */

BSTNode* addProductToInventory(BSTNode *inventory) {
    Product p;
    char buf[128];

    if (!read_int("Enter product ID: ", &p.id, 1))
        return inventory;

    BSTNode *existing = bstSearch(inventory, p.id);

    if (existing) {
        printf("Product exists: %s | Current Qty: %d | Price: %.2f\n",
               existing->product.name, existing->product.quantity, existing->product.price);

        // Quantity update options
        printf("Change quantity? (a=add / r=replace / n=skip): ");
        if (!fgets(buf, sizeof(buf), stdin)) return inventory;
        char qopt = buf[0];
        if (qopt == 'a' || qopt == 'A') {
            int addQty = 0;
            if (read_int("Enter quantity to add: ", &addQty, 1)) {
                existing->product.quantity += addQty;
                printf("Quantity increased. New Qty: %d\n", existing->product.quantity);
            }
        } else if (qopt == 'r' || qopt == 'R') {
            int newQty = 0;
            if (read_int("Enter new quantity: ", &newQty, 1)) {
                existing->product.quantity = newQty;
                printf("Quantity replaced. New Qty: %d\n", existing->product.quantity);
            }
        } else {
            printf("Quantity unchanged.\n");
        }

        // Price update option
        printf("Change price? (y/n): ");
        if (!fgets(buf, sizeof(buf), stdin)) return inventory;
        char popt = buf[0];
        if (popt == 'y' || popt == 'Y') {
            double newPrice = 0.0;
            if (read_double("Enter new price: ", &newPrice, 1)) {
                existing->product.price = newPrice;
                printf("Price updated. New Price: %.2f\n", existing->product.price);
            }
        } else {
            printf("Price unchanged.\n");
        }

        return inventory;
    }

    // New product flow
    while (1) {
        printf("Enter product name: ");
        if (!fgets(buf, sizeof(buf), stdin)) return inventory;
        str_trim_newline(buf);

        int bad = 0;
        for (int i = 0; buf[i]; i++)
            if (isdigit((unsigned char)buf[i])) bad = 1;

        if (bad) {
            printf("Names cannot contain digits.\n");
            continue;
        }

        size_t n = sizeof(p.name);
        strncpy(p.name, buf, n - 1);
        p.name[n - 1] = '\0';
        break;
    }

    read_int("Enter quantity: ", &p.quantity, 1);
    read_double("Enter price: ", &p.price, 1);

    inventory = bstInsert(inventory, p);
    printf("Product added.\n");
    return inventory;
}

/* -----------------------------
   Add to Cart
   ----------------------------- */

void addToCart(BSTNode *inventory) {
    int id = 0, qty = 0;

    read_int("Enter product ID to add: ", &id, 1);

    BSTNode *node = bstSearch(inventory, id);

    if (!node) {
        printf("Product not found.\n");
        return;
    }

    if (node->product.quantity <= 0) {
        printf("Product out of stock.\n");
        return;
    }

    printf("Found: %s | Stock: %d | Price: %.2f\n",
           node->product.name, node->product.quantity, node->product.price);

    while (1) {
        read_int("Enter quantity: ", &qty, 1);
        if (qty <= node->product.quantity) break;
        printf("Not enough stock.\n");
    }

    cartAddItem(node->product.id, node->product.name, qty, node->product.price);
}

/* -----------------------------
   Billing
   ----------------------------- */

void generateBill(BSTNode *inventory) {
    if (!cartHead) {
        printf("Cart empty.\n");
        return;
    }

    CartNode *cur = cartHead;
    double total = 0;
    int valid = 0;

    printf("\n===== BILL =====\n");

    while (cur) {
        BSTNode *node = bstSearch(inventory, cur->id);

        if (!node || node->product.quantity < cur->quantity) {
            printf("Skipping %s (not enough stock)\n", cur->name);
            cur = cur->next;
            continue;
        }

        node->product.quantity -= cur->quantity;

        double sub = cur->quantity * cur->price;
        total += sub;
        valid++;

        printf("%s | Qty: %d | Subtotal: %.2f | Remaining: %d\n",
               cur->name, cur->quantity, sub, node->product.quantity);

        cur = cur->next;
    }

    printf("------------------\n");
    printf("TOTAL: %.2f\n", total);
    printf("==================\n");

    clearCart();
}

/* -----------------------------
   Seed Inventory
   ----------------------------- */

BSTNode* seedInventory(BSTNode *inventory) {
    Product p;

    p.id = 101; strcpy(p.name, "Pen");     p.quantity = 50;  p.price = 10;   inventory = bstInsert(inventory, p);
    p.id = 102; strcpy(p.name, "Notebook");p.quantity = 30;  p.price = 25.5; inventory = bstInsert(inventory, p);
    p.id = 103; strcpy(p.name, "Eraser");  p.quantity = 100; p.price = 5;    inventory = bstInsert(inventory, p);
    p.id = 104; strcpy(p.name, "Marker");  p.quantity = 20;  p.price = 40;   inventory = bstInsert(inventory, p);

    return inventory;
}

/* -----------------------------
   Header Complements
   ----------------------------- */

void displayInventory(BSTNode *inventory) {
    if (!inventory) {
        printf("Inventory is empty.\n");
        return;
    }
    printf("\n--- Inventory (Sorted by ID) ---\n");
    inorderDisplay(inventory);
    printf("--------------------------------\n");
}

int bstUpdateQuantity(BSTNode *root, int id, int soldQty) {
    BSTNode *node = bstSearch(root, id);
    if (!node) return 0;
    if (soldQty > node->product.quantity) return 0;
    node->product.quantity -= soldQty;
    return 1;
}

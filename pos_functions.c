#include "pos.h"

CartNode *cartHead = NULL;

/* -----------------------------
   BST (Inventory) Functions
   ----------------------------- */

BSTNode* createBSTNode(Product p) {
    BSTNode *node = (BSTNode*)malloc(sizeof(BSTNode));
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
        strcpy(root->product.name, p.name);
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

int bstUpdateQuantity(BSTNode *root, int id, int soldQty) {
    BSTNode *node = bstSearch(root, id);
    if (!node) return 0;
    if (node->product.quantity < soldQty) return 0;
    node->product.quantity -= soldQty;
    return 1;
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
    CartNode *node = (CartNode*)malloc(sizeof(CartNode));
    node->id = id;
    strcpy(node->name, name);
    node->quantity = qty;
    node->price = price;
    node->next = NULL;
    return node;
}

void cartAddItem(int id, const char *name, int qty, double price) {
    if (qty <= 0) {
        printf("❌ Invalid quantity.\n");
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
    if (!prev) cartHead = newNode;
    else prev->next = newNode;
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
               cur->id, cur->name, cur->quantity, cur->price, cur->quantity * cur->price);
        cur = cur->next;
    }
    printf("--- End Cart ---\n\n");
}

/* -----------------------------
   Functional Modules
   ----------------------------- */

BSTNode* addProductToInventory(BSTNode *inventory) {
    Product p;

    // Validate Product ID (integer only)
    int valid;
    do {
        printf("Enter product ID (integer): ");
        valid = scanf("%d", &p.id);
        if (!valid) {
            printf("❌ Invalid input. Only integers allowed for product ID.\n");
            while (getchar() != '\n'); // flush buffer
        } else if (p.id <= 0) {
            printf("❌ Product ID must be positive.\n");
            valid = 0;
        }
    } while (!valid);

    BSTNode *existing = bstSearch(inventory, p.id);
    if (existing) {
        printf("⚠ Product with ID %d already exists (%s).\n", existing->product.id, existing->product.name);
        printf("Do you want to overwrite it? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            printf("Operation cancelled. Product not modified.\n");
            return inventory;
        }
    }

    // Validate Product Name (no digits)
    int isValidName;
    do {
        isValidName = 1;
        printf("Enter product name: ");
        scanf("%s", p.name);
        for (int i = 0; p.name[i] != '\0'; i++) {
            if (isdigit(p.name[i])) {
                isValidName = 0;
                printf("❌ Product name cannot contain numbers. Try again.\n");
                break;
            }
        }
    } while (!isValidName);

    // Quantity validation
    do {
        printf("Enter quantity to add: ");
        if (scanf("%d", &p.quantity) != 1 || p.quantity < 0) {
            printf("❌ Quantity must be a positive integer or zero.\n");
            while (getchar() != '\n');
            p.quantity = -1;
        }
    } while (p.quantity < 0);

    // Price validation
    do {
        printf("Enter price per unit: ");
        if (scanf("%lf", &p.price) != 1 || p.price <= 0) {
            printf("❌ Price must be a positive number.\n");
            while (getchar() != '\n');
            p.price = 0;
        }
    } while (p.price <= 0);

    inventory = bstInsert(inventory, p);
    printf("✅ Product added/updated in inventory.\n");
    return inventory;
}

void displayInventory(BSTNode *inventory) {
    if (!inventory) {
        printf("Inventory is empty.\n");
        return;
    }
    printf("\n--- Inventory (sorted by ID) ---\n");
    inorderDisplay(inventory);
    printf("--- End Inventory ---\n\n");
}

/* -----------------------------
   Add to Cart (with Out-of-Stock Check)
   ----------------------------- */

void addToCart(BSTNode *inventory) {
    if (!inventory) {
        printf("Inventory empty. Cannot add to cart.\n");
        return;
    }

    int id, qty;
    int validInput;

    // Validate Product ID
    do {
        printf("Enter product ID to add to cart: ");
        validInput = scanf("%d", &id);

        if (!validInput) {
            printf("❌ Invalid input. Product ID must be an integer.\n");
            while (getchar() != '\n');
            continue;
        }
        if (id <= 0) {
            printf("❌ Product ID must be a positive integer.\n");
            validInput = 0;
        }
    } while (!validInput);

    // Search for product
    BSTNode *node = bstSearch(inventory, id);
    if (!node) {
        printf("❌ Product with ID %d not found.\n", id);
        return;
    }

    // Check stock
    if (node->product.quantity == 0) {
        printf("❌ Product '%s' (ID %d) is OUT OF STOCK.\n", node->product.name, node->product.id);
        return;
    }

    printf("✅ Found: %s | Available Qty: %d | Price: %.2f\n",
           node->product.name, node->product.quantity, node->product.price);

    // Validate Quantity
    do {
        printf("Enter quantity to add to cart: ");
        validInput = scanf("%d", &qty);

        if (!validInput) {
            printf("❌ Invalid input. Quantity must be an integer.\n");
            while (getchar() != '\n');
            continue;
        }
        if (qty <= 0) {
            printf("❌ Quantity must be a positive integer.\n");
            validInput = 0;
        } else if (qty > node->product.quantity) {
            printf("❌ Not enough stock. Available: %d\n", node->product.quantity);
            validInput = 0;
        }
    } while (!validInput);

    cartAddItem(node->product.id, node->product.name, qty, node->product.price);
}

/* -----------------------------
   Generate Bill (Validated)
   ----------------------------- */

void generateBill(BSTNode *inventory) {
    if (!cartHead) {
        printf("Cart empty. Nothing to bill.\n");
        return;
    }

    double total = 0.0;
    int validItems = 0;
    CartNode *cur = cartHead;

    printf("\n===== BILL =====\n");

    while (cur) {
        if (cur->quantity <= 0) {
            printf("⚠ Skipping %s (invalid quantity: %d)\n", cur->name, cur->quantity);
            cur = cur->next;
            continue;
        }

        BSTNode *node = bstSearch(inventory, cur->id);
        if (!node) {
            printf("⚠ %s (ID %d) not found in inventory — skipped.\n", cur->name, cur->id);
            cur = cur->next;
            continue;
        }

        if (node->product.quantity < cur->quantity) {
            printf("⚠ Insufficient stock for %s (available: %d, requested: %d) — skipped.\n",
                   node->product.name, node->product.quantity, cur->quantity);
            cur = cur->next;
            continue;
        }

        node->product.quantity -= cur->quantity;
        double subtotal = cur->quantity * cur->price;
        total += subtotal;
        validItems++;

        printf("%s | Qty: %d | Unit: %.2f | Subtotal: %.2f | Remaining: %d\n",
               cur->name, cur->quantity, cur->price, subtotal, node->product.quantity);

        cur = cur->next;
    }

    if (validItems == 0) {
        printf("--------------------------\n");
        printf("⚠ No valid items to bill. Nothing charged.\n");
    } else {
        printf("--------------------------\n");
        printf("TOTAL AMOUNT: Rs %.2f\n", total);
    }

    printf("===== END BILL =====\n");
    clearCart();
    printf("🧹 Cart cleared after billing.\n");
}

/* -----------------------------
   Seed Inventory
   ----------------------------- */

BSTNode* seedInventory(BSTNode *inventory) {
    Product p;
    p.id = 101; strcpy(p.name, "Pen"); p.quantity = 50; p.price = 10.0; inventory = bstInsert(inventory, p);
    p.id = 102; strcpy(p.name, "Notebook"); p.quantity = 30; p.price = 25.5; inventory = bstInsert(inventory, p);
    p.id = 103; strcpy(p.name, "Eraser"); p.quantity = 100; p.price = 5.0; inventory = bstInsert(inventory, p);
    p.id = 104; strcpy(p.name, "Marker"); p.quantity = 20; p.price = 40.0; inventory = bstInsert(inventory, p);
    return inventory;
}

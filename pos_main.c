#include "pos.h"
#include <stdbool.h>

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static int read_menu_choice(int *out) {
    char buf[64];
    printf("Choose an option: ");
    if (!fgets(buf, sizeof(buf), stdin)) return 0;

    char *s = buf;
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') s++;
    if (*s == '\0') return -1;

    char *end;
    long v = strtol(s, &end, 10);
    if (end == s) return -1;
    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') end++;
    if (*end != '\0') return -1;

    *out = (int)v;
    return 1;
}

int main() {
    BSTNode *inventory = NULL;
    inventory = seedInventory(inventory);
    bool flag=true;

    while (flag) {
        printf("\n===== POS SIMULATOR =====\n");
        printf("1. Add product to inventory\n");
        printf("2. Display inventory\n");
        printf("3. Add item to cart\n");
        printf("4. Remove item from cart\n");
        printf("5. Display cart\n");
        printf("6. Generate bill\n");
        printf("7. Exit\n");
        int choice;
        int rc = read_menu_choice(&choice);
        if (rc != 1) {
            printf("Invalid option. Try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                inventory = addProductToInventory(inventory);
                break;
            case 2:
                displayInventory(inventory);
                break;
            case 3:
                displayInventory(inventory);
                addToCart(inventory);
                break;
            case 4:
                removeFromCart();
                break;
            case 5:
                displayCart();
                break;
            case 6:
                generateBill(inventory);
                break;
            case 7:
                clearCart();
                freeBST(inventory);
                printf("Exiting... Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Try again.\n");
                continue;
            
        }
    }
}


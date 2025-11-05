#include "pos.h"

int main() {
    BSTNode *inventory = NULL;
    inventory = seedInventory(inventory);

    while (1) {
        printf("\n===== POS SIMULATOR =====\n");
        printf("1. Add product to inventory\n");
        printf("2. Display inventory\n");
        printf("3. Add item to cart\n");
        printf("4. Display cart\n");
        printf("5. Generate bill\n");
        printf("6. Exit\n");
        printf("Choose an option: ");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                inventory = addProductToInventory(inventory);
                break;
            case 2:
                displayInventory(inventory);
                break;
            case 3:
                addToCart(inventory);
                break;
            case 4:
                displayCart();
                break;
            case 5:
                generateBill(inventory);
                break;
            case 6:
                clearCart();
                freeBST(inventory);
                printf("Exiting... Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Try again.\n");
        }
    }
}

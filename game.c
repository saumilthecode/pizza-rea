#include <stdio.h>
#include <string.h>
#include <conio.h>

#define MAX_INPUT 50
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define MAP_WIDTH 11
#define MAP_HEIGHT 11

typedef struct {
    char description[100];
    int north, south, east, west;
    char item[20];
} Room;

Room rooms[] = {
    {"You are in the Kitchen. There are fresh ingredients everywhere!", 1, -1, 2, -1, "cheese"},
    {"This is the Storage Room filled with various supplies.", -1, 0, -1, -1, "pepperoni"},
    {"You're in the Dining Area. It's cozy, with a table set for dinner.", -1, 3, -1, 0, "vegetables"},
    {"Welcome to the Oven Room. It's warm and smells delicious!", 2, -1, -1, -1, "pizza dough"}
};

int currentRoom = 0;
char inventory[100] = "";
int gameWon = 0;

void clearScreen() {
    system("cls");
}

void drawBox() {
    printf("\n");
    for (int i = 0; i < SCREEN_WIDTH; i++) printf("-");
    printf("\n");
}

void printWrapped(const char* text) {
    printf("%s\n", text);
}

void displayRoom() {
    clearScreen();
    drawBox();
    printWrapped(rooms[currentRoom].description);

    printf("Exits:");
    if (rooms[currentRoom].north != -1) printf(" NORTH");
    if (rooms[currentRoom].south != -1) printf(" SOUTH");
    if (rooms[currentRoom].east != -1) printf(" EAST");
    if (rooms[currentRoom].west != -1) printf(" WEST");

    printf("\nItems in room: %s", rooms[currentRoom].item);
    printf("\nInventory: %s", inventory);
}

void takeItem() {
    if (strlen(rooms[currentRoom].item) > 0) {
        if (strlen(inventory) > 0) {
            strcat(inventory, ", ");
        }
        strcat(inventory, rooms[currentRoom].item);
        printf("\nYou picked up: %s\n", rooms[currentRoom].item);
        strcpy(rooms[currentRoom].item, "");  // Remove item from room
    } else {
        printf("\nNo items to pick up here.\n");
    }
}

void movePlayer(char direction) {
    int oldRoom = currentRoom;

    switch (direction) {
        case 'n':
            if (rooms[currentRoom].north != -1) currentRoom = rooms[currentRoom].north;
            break;
        case 's':
            if (rooms[currentRoom].south != -1) currentRoom = rooms[currentRoom].south;
            break;
        case 'e':
            if (rooms[currentRoom].east != -1) currentRoom = rooms[currentRoom].east;
            break;
        case 'w':
            if (rooms[currentRoom].west != -1) currentRoom = rooms[currentRoom].west;
            break;
        default:
            printf("\nInvalid direction!\n");
            return;
    }

    if (currentRoom != oldRoom) {
        displayRoom();
    }
}

void checkWinCondition() {
    if (strstr(inventory, "cheese") && strstr(inventory, "pepperoni") && strstr(inventory, "vegetables") && strstr(inventory, "pizza dough")) {
        gameWon = 1;
        printf("\nCongratulations! You've collected all the ingredients and made a pizza!\n");
    }
}

int main() {
    char input[MAX_INPUT];

    displayRoom();

    while (!gameWon) {
        printf("\nEnter command (move [n/s/e/w], take, or inventory): ");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = 0;  // Remove newline character

        if (strncmp(input, "move ", 5) == 0) {
            char direction = input[5];
            movePlayer(direction);
        } else if (strcmp(input, "take") == 0) {
            takeItem();
        } else if (strcmp(input, "inventory") == 0) {
            printf("\nYou are carrying: %s\n", inventory);
        } else {
            printf("\nInvalid command!\n");
        }

        checkWinCondition();
    }

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>


#define MAX_INPUT 50
#define MAX_ITEMS 10
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define MAP_WIDTH 11
#define MAP_HEIGHT 11

typedef struct {
    char description[100];
    int north, south, east, west;
    char item[20];
} PizzeriaRoom;

PizzeriaRoom pizzeriaRooms[] = {
    {"You are in the Kitchen, full of delicious smells.", 1, -1, 2, -1, "cheese"},
    {"You enter the cool Storage Room, where pepperoni is kept.", -1, 0, -1, -1, "pepperoni"},
    {"You walk into the Dining Area with tables set.", -1, 3, -1, 0, "plates"},
    {"This is the Oven Room, where pizzas are baked to perfection.", 2, -1, -1, -1, "pizza dough"}
};

int currentLocation = 0;
char ingredientsBag[MAX_ITEMS][20];
int itemCount = 0;

void clearScreen() {
    clrscr();  // Use Turbo C's built-in screen clear function
}

void gotoxy(int x, int y) {
    union REGS regs;
    regs.h.ah = 0x02;
    regs.h.bh = 0x00;
    regs.h.dh = y;
    regs.h.dl = x;
    int86(0x10, &regs, &regs);
}

void drawBox() {
    int i;  // Declaration moved to the top

    gotoxy(0, 0);
    putch(218);  // Top-left corner
    for (i = 1; i < SCREEN_WIDTH - 1; i++) putch(196);  // Top border
    putch(191);  // Top-right corner
    
    for (i = 1; i < SCREEN_HEIGHT - 1; i++) {
        gotoxy(0, i);
        putch(179);  // Left border
        gotoxy(SCREEN_WIDTH - 1, i);
        putch(179);  // Right border
    }
    
    gotoxy(0, SCREEN_HEIGHT - 1);
    putch(192);  // Bottom-left corner
    for (i = 1; i < SCREEN_WIDTH - 1; i++) putch(196);  // Bottom border
    putch(217);  // Bottom-right corner
}

void printWrapped(int x, int y, const char* text, int width) {
    int curX = x, curY = y;
    int len = strlen(text);
    int i = 0, j;  // Declaration moved to the top

    while (i < len) {
        j = 0;
        while (j < width && i + j < len && text[i + j] != '\n') j++;
        
        if (i + j < len && text[i + j] != '\n') {
            while (j > 0 && text[i + j - 1] != ' ') j--;
        }
        
        gotoxy(curX, curY);
        printf("%.*s", j, &text[i]);
        i += j;
        if (text[i] == '\n' || text[i] == ' ') i++;
        curY++;
    }
}

void drawCompass() {
    int compassX = SCREEN_WIDTH - 10;
    int compassY = SCREEN_HEIGHT - 8;

    // North direction
    gotoxy(compassX, compassY);
    if (pizzeriaRooms[currentLocation].north != -1) {
        textattr(YELLOW);  // Yellow for accessible direction
        printf("   N   ");
    } else {
        textattr(RED);     // Red for inaccessible direction
        printf("   N   ");
    }

    // Middle vertical bar
    gotoxy(compassX, compassY + 1);
    textattr(WHITE);      // Default color for separator
    printf("   |   ");
    
    // West direction
    gotoxy(compassX, compassY + 2);
    if (pizzeriaRooms[currentLocation].west != -1) {
        textattr(YELLOW);  // Yellow for accessible direction
        printf("W");
    } else {
        textattr(RED);     // Red for inaccessible direction
        printf("W");
    }

    // Middle plus sign
    textattr(WHITE);      // Default color for separator
    printf("--+--");

    // East direction
    if (pizzeriaRooms[currentLocation].east != -1) {
        textattr(YELLOW);  // Yellow for accessible direction
        printf("E");
    } else {
        textattr(RED);     // Red for inaccessible direction
        printf("E");
    }

    // Middle vertical bar
    gotoxy(compassX, compassY + 3);
    textattr(WHITE);      // Default color for separator
    printf("   |   ");
    
    // South direction
    gotoxy(compassX, compassY + 4);
    if (pizzeriaRooms[currentLocation].south != -1) {
        textattr(YELLOW);  // Yellow for accessible direction
        printf("   S   ");
    } else {
        textattr(RED);     // Red for inaccessible direction
        printf("   S   ");
    }

    textattr(WHITE);  // Reset text color to default
}

void displayCommands() {
    int commandsX = SCREEN_WIDTH - 20;
    int commandsY = 2;
    gotoxy(commandsX, commandsY);
    printf("Commands:");
    gotoxy(commandsX, commandsY + 1);
    printf(" north   - Move north");
    gotoxy(commandsX, commandsY + 2);
    printf(" south   - Move south");
    gotoxy(commandsX, commandsY + 3);
    printf(" east    - Move east");
    gotoxy(commandsX, commandsY + 4);
    printf(" west    - Move west");
    gotoxy(commandsX, commandsY + 5);
    printf(" take item - Collect item");
    gotoxy(commandsX, commandsY + 6);
    printf(" quit    - Exit game");
}

void displayBackpack() {
    int i;  // Declare the loop variable here
    gotoxy(2, 12);  // Display backpack below the current item
    printf("Backpack: ");
    
    if (itemCount == 0) {
        printf("Empty");
    } else {
        for (i = 0; i < itemCount; i++) {  // Properly declare and use 'i'
            if (i > 0) printf(", ");
            printf("%s", ingredientsBag[i]);
        }
    }
}

void drawMap() {
    int mapX = (SCREEN_WIDTH - 40) / 2;
    int mapY = SCREEN_HEIGHT - 10;  // Adjust map position above the input area

    gotoxy(mapX, mapY);
    textattr(YELLOW);  // Set color for map header
    printf("Pizzeria Map: ");
    
    // Create a visual layout of the rooms and set colors based on movement possibilities
    gotoxy(mapX, mapY + 1);
    
    // Kitchen (K)
    gotoxy(mapX, mapY + 2);
    if (pizzeriaRooms[currentLocation].north == 0) {
        textattr(YELLOW);  // Yellow for north if accessible
    } else {
        textattr(WHITE);   // White if inaccessible
    }
    printf("   K   \n");

    // Storage Room (S) and Dining Area (D)
    gotoxy(mapX, mapY + 3);
    if (pizzeriaRooms[currentLocation].west == 1) {
        textattr(YELLOW);  // Yellow for west if accessible
    } else {
        textattr(WHITE);
    }
    printf("S");

    textattr(WHITE);  // Default color for the space between rooms
    printf("  ");
    
    if (pizzeriaRooms[currentLocation].east == 2 || pizzeriaRooms[currentLocation].south == 2) {
        textattr(YELLOW);  // Yellow for east/south if accessible
    } else {
        textattr(WHITE);
    }
    printf("D");

    // Oven Room (O)
    gotoxy(mapX, mapY + 4);
    if (pizzeriaRooms[currentLocation].south == 3) {
        textattr(YELLOW);  // Yellow for south if accessible
    } else {
        textattr(WHITE);
    }
    printf("   O   \n");
    
    textattr(WHITE);  // Reset to default color
}

void displayRoom() {
    clearScreen();
    drawBox();
    
    // Display room description
    gotoxy(2, 6);
    printf("You are in: ");
    printWrapped(2, 7, pizzeriaRooms[currentLocation].description, SCREEN_WIDTH - 4);
    
    // Display current item in the room
    gotoxy(2, 10);
    if (strcmp(pizzeriaRooms[currentLocation].item, "none") != 0) {
        printf("Current Item: %s", pizzeriaRooms[currentLocation].item);
    } else {
        printf("Current Item: None");
    }
    
    // Display backpack contents between current item and command input
    displayBackpack();
    
    // Draw compass and commands
    drawCompass();
    displayCommands();
    drawMap();  // Draw the map at the bottom
}

void takeItem() {
    if (strcmp(pizzeriaRooms[currentLocation].item, "none") != 0) {
        if (itemCount < MAX_ITEMS) {
            strcpy(ingredientsBag[itemCount], pizzeriaRooms[currentLocation].item);
            itemCount++;
            printf("\nYou took %s!\n", pizzeriaRooms[currentLocation].item);
            strcpy(pizzeriaRooms[currentLocation].item, "none");  // Remove the item from the room
        } else {
            printf("\nBackpack is full!\n");
        }
    } else {
        printf("\nThere is no item to take!\n");
    }
}

void move(char direction) {
    int newLocation = -1;

    switch (direction) {
        case 'n':
            newLocation = pizzeriaRooms[currentLocation].north;
            break;
        case 's':
            newLocation = pizzeriaRooms[currentLocation].south;
            break;
        case 'e':
            newLocation = pizzeriaRooms[currentLocation].east;
            break;
        case 'w':
            newLocation = pizzeriaRooms[currentLocation].west;
            break;
    }
    
    if (newLocation != -1) {
        currentLocation = newLocation;
        printf("You move %c.\n", direction);
    } else {
        printf("You can't go that way!\n");
    }
}

void customDelay(int milliseconds) {
    delay(milliseconds);  // Adjust this if needed for your environment
}

void checkWinCondition() {
    // Implement win condition logic here
}

int main() {
    char input[MAX_INPUT];
    
    while (1) {
        displayRoom();
        
        gotoxy(2, SCREEN_HEIGHT - 2);  // Adjust if necessary based on map position
        printf("Enter command: ");
        fgets(input, MAX_INPUT, stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "north") == 0) {
            move('n');
        } else if (strcmp(input, "south") == 0) {
            move('s');
        } else if (strcmp(input, "east") == 0) {
            move('e');
        } else if (strcmp(input, "west") == 0) {
            move('w');
        } else if (strcmp(input, "take item") == 0) {
            takeItem();
        } else if (strcmp(input, "show backpack") == 0) {
            displayBackpack();
        } else {
            printf("Unknown command. Please try again.\n");
        }
        
        checkWinCondition();
        customDelay(1000);  // 1-second delay between screen updates
    }
    
    return 0;
}

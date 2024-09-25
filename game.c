#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#define MAX_INPUT 50
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
char ingredientsBag[50] = "";
int pizzaReady = 0;

char pizzeriaMap[MAP_HEIGHT][MAP_WIDTH];

int roomCoordinates[][2] = {
    {5, 5},  /* Kitchen */
    {5, 3},  /* Storage Room */
    {7, 5},  /* Dining Area */
    {7, 7}   /* Oven Room */
};

void clearScreen() {
    union REGS regs;
    regs.h.ah = 0x06;  /* Scroll up function */
    regs.h.al = 0;     /* Clear entire screen */
    regs.h.ch = 0;     /* Upper left corner row */
    regs.h.cl = 0;     /* Upper left corner col */
    regs.h.dh = 24;    /* Lower right corner row */
    regs.h.dl = 79;    /* Lower right corner col */
    regs.h.bh = 0x07;  /* White on black */
    int86(0x10, &regs, &regs);
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
    int i;
    gotoxy(0, 0);
    putch(218);
    for (i = 1; i < SCREEN_WIDTH - 1; i++) putch(196);
    putch(191);

    for (i = 1; i < SCREEN_HEIGHT - 1; i++) {
        gotoxy(0, i);
        putch(179);
        gotoxy(SCREEN_WIDTH - 1, i);
        putch(179);
    }

    gotoxy(0, SCREEN_HEIGHT - 1);
    putch(192);
    for (i = 1; i < SCREEN_WIDTH - 1; i++) putch(196);
    putch(217);
}

void printWrapped(int x, int y, const char* text, int width) {
    int curX = x, curY = y;
    int len = strlen(text);
    int i = 0;
    int j;

    while (i < len) {
        j = 0;
        while (j < width && i + j < len && text[i + j] != '\n') {
            j++;
        }

        if (i + j < len && text[i + j] != '\n') {
            while (j > 0 && text[i + j - 1] != ' ') {
                j--;
            }
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

    gotoxy(compassX, compassY);
    printf("   N   ");
    gotoxy(compassX, compassY + 1);
    printf("   |   ");
    gotoxy(compassX, compassY + 2);
    printf("W--+--E");
    gotoxy(compassX, compassY + 3);
    printf("   |   ");
    gotoxy(compassX, compassY + 4);
    printf("   S   ");

    /* Highlight available directions */
    if (pizzeriaRooms[currentLocation].north != -1) {
        gotoxy(compassX + 3, compassY);
        printf("\033[1;33mN\033[0m");
    }
    if (pizzeriaRooms[currentLocation].south != -1) {
        gotoxy(compassX + 3, compassY + 4);
        printf("\033[1;33mS\033[0m");
    }
    if (pizzeriaRooms[currentLocation].east != -1) {
        gotoxy(compassX + 6, compassY + 2);
        printf("\033[1;33mE\033[0m");
    }
    if (pizzeriaRooms[currentLocation].west != -1) {
        gotoxy(compassX, compassY + 2);
        printf("\033[1;33mW\033[0m");
    }
}

void initializeMap() {
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            pizzeriaMap[i][j] = ' ';
        }
    }
}

void updateMap() {
    int i;

    /* Initialize the map to spaces */
    initializeMap();

    /* Draw rooms */
    for (i = 0; i < 4; i++) {
        int x = roomCoordinates[i][0];
        int y = roomCoordinates[i][1];
        if (i == currentLocation) {
            pizzeriaMap[y][x] = 'X';
        } else {
            pizzeriaMap[y][x] = 'O';
        }
    }

    /* Draw connections */
    for (i = 0; i < 4; i++) {
        int x1 = roomCoordinates[i][0];
        int y1 = roomCoordinates[i][1];

        if (pizzeriaRooms[i].north != -1) {
            int northRoom = pizzeriaRooms[i].north;
            int x2 = roomCoordinates[northRoom][0];
            int y2 = roomCoordinates[northRoom][1];

            int y;
            for (y = y2 + 1; y < y1; y++) {
                pizzeriaMap[y][x1] = '|';
            }
        }
        if (pizzeriaRooms[i].south != -1) {
            int southRoom = pizzeriaRooms[i].south;
            int x2 = roomCoordinates[southRoom][0];
            int y2 = roomCoordinates[southRoom][1];

            int y;
            for (y = y1 + 1; y < y2; y++) {
                pizzeriaMap[y][x1] = '|';
            }
        }
        if (pizzeriaRooms[i].east != -1) {
            int eastRoom = pizzeriaRooms[i].east;
            int x2 = roomCoordinates[eastRoom][0];
            int y2 = roomCoordinates[eastRoom][1];

            int x;
            for (x = x1 + 1; x < x2; x++) {
                pizzeriaMap[y1][x] = '-';
            }
        }
        if (pizzeriaRooms[i].west != -1) {
            int westRoom = pizzeriaRooms[i].west;
            int x2 = roomCoordinates[westRoom][0];
            int y2 = roomCoordinates[westRoom][1];

            int x;
            for (x = x2 + 1; x < x1; x++) {
                pizzeriaMap[y1][x] = '-';
            }
        }
    }
}

void displayRoom() {
    int mapX, mapY;
    int i, j;

    clearScreen();
    drawBox();

    printWrapped(2, 1, pizzeriaRooms[currentLocation].description, SCREEN_WIDTH - 4);

    gotoxy(2, 3);
    printf("Exits:");
    if (pizzeriaRooms[currentLocation].north != -1) printf(" NORTH");
    if (pizzeriaRooms[currentLocation].south != -1) printf(" SOUTH");
    if (pizzeriaRooms[currentLocation].east != -1) printf(" EAST");
    if (pizzeriaRooms[currentLocation].west != -1) printf(" WEST");

    gotoxy(2, 5);
    printf("Items in room: %s", pizzeriaRooms[currentLocation].item);

    gotoxy(2, 7);
    printf("Ingredients collected: %s", ingredientsBag);

    updateMap();
    gotoxy(2, 9);
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            printf("%c", pizzeriaMap[i][j]);
        }
        printf("\n");
    }

    drawCompass();
}

int main() {
    char input[MAX_INPUT];

    while (!pizzaReady) {
        displayRoom();
        printf("\nWhat do you want to do? ");
        gets(input);

        if (strcmp(input, "north") == 0) {
            if (pizzeriaRooms[currentLocation].north != -1) {
                currentLocation = pizzeriaRooms[currentLocation].north;
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(input, "south") == 0) {
            if (pizzeriaRooms[currentLocation].south != -1) {
                currentLocation = pizzeriaRooms[currentLocation].south;
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(input, "east") == 0) {
            if (pizzeriaRooms[currentLocation].east != -1) {
                currentLocation = pizzeriaRooms[currentLocation].east;
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(input, "west") == 0) {
            if (pizzeriaRooms[currentLocation].west != -1) {
                currentLocation = pizzeriaRooms[currentLocation].west;
            } else {
                printf("You can't go that way.\n");
            }
        } else if (strcmp(input, "take item") == 0) {
            strcat(ingredientsBag, pizzeriaRooms[currentLocation].item);
            strcat(ingredientsBag, " ");
            if (strstr(ingredientsBag, "cheese") && strstr(ingredientsBag, "pepperoni") &&
                strstr(ingredientsBag, "plates") && strstr(ingredientsBag, "pizza dough")) {
                pizzaReady = 1;
                printf("You have all the ingredients! Time to bake the pizza!\n");
            }
        } else {
            printf("I don't understand that.\n");
        }
    }

    printf("\nCongratulations! You've collected all the ingredients and baked the perfect pizza!\n");
    return 0;
}

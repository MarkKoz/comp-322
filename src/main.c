#include <stdio.h>

const char* const MENU_TEXT =
    "Process creation and destruction\n"
    "--------------------------------\n"
    "1) Enter parameters\n"
    "2) Create a new child process\n"
    "3) Destroy all descendants of a process\n"
    "4) Quit program and free memory\n\n\n"
    "Enter selection: ";

void enter_parameters(void);
void create(void);
void destroy(void);
void quit(void);

int main(void)
{
    printf(MENU_TEXT);

    while (1) {
        char choice = '\0';
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                enter_parameters();
                break;
            case '2':
                create();
                break;
            case '3':
                destroy();
                break;
            case '4':
                quit();
                return 0;
            default:
                printf("Invalid selection, try again: ");
                continue; // Avoid printing the menu again.
        }

        printf("\n\n%s", MENU_TEXT);
    }
}

void enter_parameters(void) { }

void create(void) { }

void destroy(void) { }

void quit(void) { }

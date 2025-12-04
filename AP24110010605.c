#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUDENT_FILE "students.txt"
#define CREDENTIAL_FILE "credentials.txt"

char currentRole[32];
char currentUser[32];

struct Student {
    int roll;
    char name[100];
    float marks;
};

// FUNCTION DECLARATIONS
void startScreen();
int loginSystem();
void signUp();
void mainMenu();
void adminMenu();
void staffMenu();
void userMenu();
void guestMenu();
void addStudent();
void displayStudents();
void searchStudent();
void uppercase(char *s);

// MAIN FUNCTION
int main() {
    startScreen();
    return 0;
}

void uppercase(char *s) {
    for (; *s; ++s) *s = toupper((unsigned char)*s);
}

// START SCREEN (Login / Signup)
void startScreen() {
    int choice;

    while (1) {
        printf("\n===== WELCOME =====\n");
        printf("1. Login\n");
        printf("2. Sign Up\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            // clear invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input!\n");
            continue;
        }

        // consume newline left by scanf
        int c = getchar();
        if (c != '\n' && c != EOF) ungetc(c, stdin);

        switch (choice) {
        case 1:
            if (loginSystem())
                mainMenu();
            else
                printf("\nLogin Failed! Try Again.\n");
            break;

        case 2:
            signUp();
            break;

        case 3:
            printf("Exiting...\n");
            exit(0);

        default:
            printf("Invalid choice!\n");
        }
    }
}

// SIGN UP FUNCTION
void signUp() {
    char username[32], password[32], role[32];

    printf("\n===== SIGN UP =====\n");
    printf("Enter Username: ");
    if (scanf("%31s", username) != 1) return;
    printf("Enter Password: ");
    if (scanf("%31s", password) != 1) return;

    printf("Enter Role (ADMIN / STAFF / USER / GUEST): ");
    if (scanf("%31s", role) != 1) return;
    uppercase(role);

    FILE *fp = fopen(CREDENTIAL_FILE, "a");
    if (!fp) {
        printf("Error: Could not open credentials file!\n");
        return;
    }

    fprintf(fp, "%s %s %s\n", username, password, role);
    fclose(fp);

    printf("Account Created Successfully!\n");
}

// LOGIN SYSTEM
int loginSystem() {
    char username[32], password[32];
    char fileUser[32], filePass[32], fileRole[32];

    printf("\n============= Login Screen ===========\n");
    printf("Username: ");
    if (scanf("%31s", username) != 1) return 0;
    printf("Password: ");
    if (scanf("%31s", password) != 1) return 0;

    FILE *fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) {
        printf("Error: credentials.txt not found!\n");
        return 0;
    }

    while (fscanf(fp, "%31s %31s %31s", fileUser, filePass, fileRole) == 3) {
        // normalize role read from file
        uppercase(fileRole);

        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            strncpy(currentRole, fileRole, sizeof(currentRole)-1);
            currentRole[sizeof(currentRole)-1] = '\0';
            strncpy(currentUser, fileUser, sizeof(currentUser)-1);
            currentUser[sizeof(currentUser)-1] = '\0';
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// MAIN MENU
void mainMenu() {
    if (strcmp(currentRole, "ADMIN") == 0)
        adminMenu();
    else if (strcmp(currentRole, "STAFF") == 0)
        staffMenu();
    else if (strcmp(currentRole, "USER") == 0)
        userMenu();
    else if (strcmp(currentRole, "GUEST") == 0)
        guestMenu();
    else
        printf("Access Denied! Invalid Role.\n");
}

// ADMIN MENU
void adminMenu() {
    int choice;
    do {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Add Student\n");
        printf("2. Display Students\n");
        printf("3. Search Student\n");
        printf("4. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input!\n");
            continue;
        }
        int c = getchar();
        if (c != '\n' && c != EOF) ungetc(c, stdin);

        switch (choice) {
        case 1: addStudent(); break;
        case 2: displayStudents(); break;
        case 3: searchStudent(); break;
        case 4: printf("Logging out...\n"); return;
        default: printf("Invalid choice!\n");
        }
    } while (1);
}

// STAFF MENU
void staffMenu() {
    int choice;
    do {
        printf("\n===== STAFF MENU =====\n");
        printf("1. Display Students\n");
        printf("2. Search Student\n");
        printf("3. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input!\n");
            continue;
        }
        int c = getchar();
        if (c != '\n' && c != EOF) ungetc(c, stdin);

        switch (choice) {
        case 1: displayStudents(); break;
        case 2: searchStudent(); break;
        case 3: printf("Logging out...\n"); return;
        default: printf("Invalid choice!\n");
        }
    } while (1);
}

// USER MENU
void userMenu() {
    int choice;
    do {
        printf("\n===== USER MENU =====\n");
        printf("1. Display Students\n");
        printf("2. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input!\n");
            continue;
        }
        int c = getchar();
        if (c != '\n' && c != EOF) ungetc(c, stdin);

        switch (choice) {
        case 1: displayStudents(); break;
        case 2: printf("Logging out...\n"); return;
        default: printf("Invalid choice!\n");
        }
    } while (1);
}

// GUEST MENU
void guestMenu() {
    printf("\nWelcome GUEST: %s\n", currentUser);
    printf("You have read-only access.\n");
    displayStudents();
}

// ADD STUDENT
void addStudent() {
    FILE *fp = fopen(STUDENT_FILE, "a");
    struct Student st;
    char temp[128];

    if (!fp) {
        printf("Error opening file!\n");
        return;
    }

    printf("\nEnter Roll Number: ");
    if (scanf("%d", &st.roll) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Invalid roll number!\n");
        fclose(fp);
        return;
    }
    // clear newline
    int c = getchar();
    if (c != '\n' && c != EOF) ungetc(c, stdin);

    printf("Enter Name: ");
    if (!fgets(st.name, sizeof(st.name), stdin)) {
        printf("Error reading name!\n");
        fclose(fp);
        return;
    }
    st.name[strcspn(st.name, "\r\n")] = '\0'; // strip newline

    printf("Enter Marks: ");
    if (scanf("%f", &st.marks) != 1) {
        int c2; while ((c2 = getchar()) != '\n' && c2 != EOF) {}
        printf("Invalid marks!\n");
        fclose(fp);
        return;
    }

    // Save using a delimiter so names with spaces are safe
    fprintf(fp, "%d|%s|%.2f\n", st.roll, st.name, st.marks);
    fclose(fp);

    printf("Student Added Successfully!\n");
}

// DISPLAY STUDENTS
void displayStudents() {
    FILE *fp = fopen(STUDENT_FILE, "r");
    struct Student st;
    char line[256];

    if (!fp) {
        printf("No data available.\n");
        return;
    }

    printf("\n====== STUDENT LIST ======\n");
    while (fgets(line, sizeof(line), fp)) {
        // parse "roll|name|marks\n"
        if (sscanf(line, "%d|%99[^|]|%f", &st.roll, st.name, &st.marks) == 3) {
            printf("Roll: %d | Name: %s | Marks: %.2f\n", st.roll, st.name, st.marks);
        } else {
            // malformed line; you can print or skip
            // printf("Skipping malformed line: %s", line);
        }
    }

    fclose(fp);
}

// SEARCH STUDENT
void searchStudent() {
    FILE *fp = fopen(STUDENT_FILE, "r");
    struct Student st;
    char line[256];
    int r, found = 0;

    if (!fp) {
        printf("No data found.\n");
        return;
    }

    printf("Enter Roll Number to Search: ");
    if (scanf("%d", &r) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Invalid input!\n");
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%d|%99[^|]|%f", &st.roll, st.name, &st.marks) == 3) {
            if (st.roll == r) {
                printf("\nStudent Found!\n");
                printf("Roll: %d\nName: %s\nMarks: %.2f\n", st.roll, st.name, st.marks);
                found = 1;
                break;
            }
        }
    }

    if (!found)
        printf("Student Not Found!\n");

    fclose(fp);
}

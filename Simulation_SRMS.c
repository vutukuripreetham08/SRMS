
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URL 512
#define MAX_TITLE 128
#define SAVE_FILE "history.txt"

typedef struct Node {
    char url[MAX_URL];
    char title[MAX_TITLE];
    struct Node *prev, *next;
} Node;

typedef struct {
    Node *head, *tail, *current;
    int size;
} History;

void init_history(History *h) {
    h->head = h->tail = h->current = NULL;
    h->size = 0;
}

Node *create_node(const char *url, const char *title) {
    Node *n = (Node*)malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(n->url, url ? url : "", MAX_URL-1); n->url[MAX_URL-1] = '\0';
    strncpy(n->title, title ? title : "", MAX_TITLE-1); n->title[MAX_TITLE-1] = '\0';
    n->prev = n->next = NULL;
    return n;
}

// When visiting a new page: if there's forward nodes (current->next),
// delete them (like real browsers) then append new node and set current to it.
void visit(History *h, const char *url, const char *title) {
    // clear forward history
    if (h->current) {
        Node *it = h->current->next;
        while (it) {
            Node *tmp = it;
            it = it->next;
            free(tmp);
            h->size--;
        }
        h->current->next = NULL;
        h->tail = h->current;
    }

    Node *n = create_node(url, title);
    if (!h->head) {
        h->head = h->tail = h->current = n;
    } else {
        h->tail->next = n;
        n->prev = h->tail;
        h->tail = n;
        h->current = n;
    }
    h->size++;
    printf("Visited: %s (%s)\n", title, url);
}

void go_back(History *h) {
    if (!h->current) { printf("History empty.\n"); return; }
    if (h->current->prev) {
        h->current = h->current->prev;
        printf("Back -> %s (%s)\n", h->current->title, h->current->url);
    } else {
        printf("Already at the oldest page.\n");
    }
}

void go_forward(History *h) {
    if (!h->current) { printf("History empty.\n"); return; }
    if (h->current->next) {
        h->current = h->current->next;
        printf("Forward -> %s (%s)\n", h->current->title, h->current->url);
    } else {
        printf("Already at the newest page.\n");
    }
}

void show_current(History *h) {
    if (!h->current) { printf("No current page.\n"); return; }
    printf("CURRENT -> %s (%s)\n", h->current->title, h->current->url);
}

void show_history(History *h) {
    if (!h->head) { printf("History is empty.\n"); return; }
    printf("Full History (size=%d):\n", h->size);
    Node *it = h->head;
    int idx = 1;
    while (it) {
        if (it == h->current) printf(" -> ");
        else printf("    ");
        printf("[%d] %s (%s)\n", idx, it->title, it->url);
        it = it->next; idx++;
    }
}

// open a page by index (1-based)
void open_index(History *h, int index) {
    if (index < 1 || index > h->size) { printf("Invalid index.\n"); return; }
    Node *it = h->head;
    for (int i = 1; i < index; ++i) it = it->next;
    if (it) {
        h->current = it;
        printf("Opened [%d] -> %s (%s)\n", index, it->title, it->url);
    }
}

// remove a history node by index (1-based). Adjust head/tail/current accordingly.
void remove_index(History *h, int index) {
    if (index < 1 || index > h->size) { printf("Invalid index.\n"); return; }
    Node *it = h->head;
    for (int i = 1; i < index; ++i) it = it->next;
    if (!it) return;

    if (it->prev) it->prev->next = it->next;
    else h->head = it->next;

    if (it->next) it->next->prev = it->prev;
    else h->tail = it->prev;

    // Move current: prefer next, else prev, else NULL
    if (h->current == it) {
        if (it->next) h->current = it->next;
        else if (it->prev) h->current = it->prev;
        else h->current = NULL;
    }

    free(it);
    h->size--;
    printf("Removed entry at index %d. New size: %d\n", index, h->size);
}

void free_all(History *h) {
    Node *it = h->head;
    while (it) {
        Node *tmp = it;
        it = it->next;
        free(tmp);
    }
    h->head = h->tail = h->current = NULL;
    h->size = 0;
}

// Save history into a simple text file (one per line: title|url)
void save_history(History *h) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (!f) { perror("fopen"); return; }
    Node *it = h->head;
    while (it) {
        // replace '|' in text to avoid collision
        char title_safe[MAX_TITLE]; strncpy(title_safe, it->title, MAX_TITLE); title_safe[MAX_TITLE-1]='\0';
        char url_safe[MAX_URL]; strncpy(url_safe, it->url, MAX_URL); url_safe[MAX_URL-1]='\0';
        for (int i=0; title_safe[i]; ++i) if (title_safe[i] == '|') title_safe[i] = '/';
        for (int i=0; url_safe[i]; ++i) if (url_safe[i] == '|') url_safe[i] = '/';
        fprintf(f, "%s|%s\n", title_safe, url_safe);
        it = it->next;
    }
    fclose(f);
    printf("History saved to %s\n", SAVE_FILE);
}

// Load history from file (clears existing history)
void load_history(History *h) {
    FILE *f = fopen(SAVE_FILE, "r");
    if (!f) { printf("No saved history found (%s).\n", SAVE_FILE); return; }
    free_all(h);
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // remove newline
        size_t L = strlen(line);
        if (L && line[L-1] == '\n') line[L-1] = '\0';
        char *sep = strchr(line, '|');
        if (!sep) continue;
        *sep = '\0';
        char *title = line;
        char *url = sep + 1;
        // append
        Node *n = create_node(url, title);
        if (!h->head) h->head = h->tail = n;
        else { h->tail->next = n; n->prev = h->tail; h->tail = n; }
        h->size++;
    }
    fclose(f);
    h->current = h->tail; // open last page by default
    printf("History loaded from %s (size=%d). Current set to last entry.\n", SAVE_FILE, h->size);
}

void read_line(char *buf, int n) {
    if (!fgets(buf, n, stdin)) { buf[0] = '\0'; return; }
    size_t l = strlen(buf); if (l && buf[l-1] == '\n') buf[l-1] = '\0';
}

int menu() {
    printf("\n--- Browser History Menu ---\n");
    printf("1. Visit new page\n");
    printf("2. Back\n");
    printf("3. Forward\n");
    printf("4. Show current page\n");
    printf("5. Show full history\n");
    printf("6. Open history by index\n");
    printf("7. Remove history entry by index\n");
    printf("8. Save history to file\n");
    printf("9. Load history from file (replace)\n");
    printf("0. Exit\n");
    printf("Choose: ");
    char line[16]; read_line(line, sizeof(line));
    return atoi(line);
}

int main(void) {
    History h;
    init_history(&h);

    // Preload some sample visits
    visit(&h, "https://example.com", "Example");
    visit(&h, "https://news.example.com/top", "Top News");
    visit(&h, "https://openai.com", "OpenAI");
    visit(&h, "https://github.com", "GitHub");

    printf("Browser History Simulator\n");

    while (1) {
        int choice = menu();
        if (choice == 0) break;
        if (choice == 1) {
            char url[MAX_URL], title[MAX_TITLE];
            printf("Enter URL: "); read_line(url, sizeof(url));
            if (strlen(url) == 0) { printf("URL cannot be empty.\n"); continue; }
            printf("Enter Title: "); read_line(title, sizeof(title));
            if (strlen(title) == 0) strncpy(title, url, MAX_TITLE-1);
            visit(&h, url, title);
        } else if (choice == 2) {
            go_back(&h);
        } else if (choice == 3) {
            go_forward(&h);
        } else if (choice == 4) {
            show_current(&h);
        } else if (choice == 5) {
            show_history(&h);
        } else if (choice == 6) {
            char buf[16]; printf("Index to open: "); read_line(buf, sizeof(buf));
            int idx = atoi(buf);
            open_index(&h, idx);
        } else if (choice == 7) {
            char buf[16]; printf("Index to remove: "); read_line(buf, sizeof(buf));
            int idx = atoi(buf);
            remove_index(&h, idx);
        } else if (choice == 8) {
            save_history(&h);
        } else if (choice == 9) {
            load_history(&h);
        } else {
            printf("Invalid option.\n");
        }
    }

    save_history(&h); // auto-save on exit
    free_all(&h);
    printf("Exiting simulator. Goodbye.\n");
    return 0;
}

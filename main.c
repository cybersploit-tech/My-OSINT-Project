#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TASKS 100
#define MAX_TITLE 100
#define MAX_DESC 500
#define MAX_CATEGORY 50
#define FILENAME "tasks.dat"

typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH,
    PRIORITY_URGENT
} Priority;

typedef enum {
    STATUS_TODO = 1,
    STATUS_IN_PROGRESS,
    STATUS_COMPLETED,
    STATUS_CANCELLED
} Status;

typedef struct {
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESC];
    char category[MAX_CATEGORY];
    Priority priority;
    Status status;
    time_t created;
    time_t deadline;
    int completed;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int count;
    int nextId;
} TaskManager;

// Function declarations
void initTaskManager(TaskManager *tm);
void loadTasks(TaskManager *tm);
void saveTasks(TaskManager *tm);
void displayMenu();
void addTask(TaskManager *tm);
void viewAllTasks(TaskManager *tm);
void viewTaskById(TaskManager *tm);
void updateTask(TaskManager *tm);
void deleteTask(TaskManager *tm);
void markTaskComplete(TaskManager *tm);
void searchTasks(TaskManager *tm);
void filterByStatus(TaskManager *tm);
void filterByPriority(TaskManager *tm);
void filterByCategory(TaskManager *tm);
void viewStatistics(TaskManager *tm);
void sortTasks(TaskManager *tm);
void clearScreen();
void pauseScreen();
char* getPriorityString(Priority p);
char* getStatusString(Status s);
void displayTask(Task *t);
void displayTaskSummary(Task *t, int index);
int getIntInput(const char *prompt, int min, int max);
void getStringInput(const char *prompt, char *buffer, int maxLen);
time_t getDateInput(const char *prompt);

int main() {
    TaskManager tm;
    int choice;
    
    initTaskManager(&tm);
    loadTasks(&tm);
    
    printf("\n========================================\n");
    printf("   TASK MANAGEMENT SYSTEM v1.0\n");
    printf("========================================\n");
    pauseScreen();
    
    while (1) {
        clearScreen();
        displayMenu();
        choice = getIntInput("Enter your choice", 0, 13);
        
        switch (choice) {
            case 1:
                addTask(&tm);
                break;
            case 2:
                viewAllTasks(&tm);
                break;
            case 3:
                viewTaskById(&tm);
                break;
            case 4:
                updateTask(&tm);
                break;
            case 5:
                deleteTask(&tm);
                break;
            case 6:
                markTaskComplete(&tm);
                break;
            case 7:
                searchTasks(&tm);
                break;
            case 8:
                filterByStatus(&tm);
                break;
            case 9:
                filterByPriority(&tm);
                break;
            case 10:
                filterByCategory(&tm);
                break;
            case 11:
                sortTasks(&tm);
                break;
            case 12:
                viewStatistics(&tm);
                break;
            case 13:
                saveTasks(&tm);
                printf("\n✓ Tasks saved successfully!\n");
                pauseScreen();
                break;
            case 0:
                saveTasks(&tm);
                printf("\n✓ Tasks saved. Goodbye!\n");
                return 0;
            default:
                printf("\n✗ Invalid choice!\n");
                pauseScreen();
        }
    }
    
    return 0;
}

void initTaskManager(TaskManager *tm) {
    tm->count = 0;
    tm->nextId = 1;
}

void loadTasks(TaskManager *tm) {
    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("No existing data found. Starting fresh.\n");
        return;
    }
    
    fread(&tm->count, sizeof(int), 1, fp);
    fread(&tm->nextId, sizeof(int), 1, fp);
    fread(tm->tasks, sizeof(Task), tm->count, fp);
    
    fclose(fp);
    printf("✓ Loaded %d tasks from file.\n", tm->count);
}

void saveTasks(TaskManager *tm) {
    FILE *fp = fopen(FILENAME, "wb");
    if (fp == NULL) {
        printf("✗ Error saving tasks!\n");
        return;
    }
    
    fwrite(&tm->count, sizeof(int), 1, fp);
    fwrite(&tm->nextId, sizeof(int), 1, fp);
    fwrite(tm->tasks, sizeof(Task), tm->count, fp);
    
    fclose(fp);
}

void displayMenu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       TASK MANAGEMENT MENU            ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  1.  Add New Task                     ║\n");
    printf("║  2.  View All Tasks                   ║\n");
    printf("║  3.  View Task by ID                  ║\n");
    printf("║  4.  Update Task                      ║\n");
    printf("║  5.  Delete Task                      ║\n");
    printf("║  6.  Mark Task as Complete            ║\n");
    printf("║  7.  Search Tasks                     ║\n");
    printf("║  8.  Filter by Status                 ║\n");
    printf("║  9.  Filter by Priority               ║\n");
    printf("║  10. Filter by Category               ║\n");
    printf("║  11. Sort Tasks                       ║\n");
    printf("║  12. View Statistics                  ║\n");
    printf("║  13. Save Tasks                       ║\n");
    printf("║  0.  Exit                             ║\n");
    printf("╚════════════════════════════════════════╝\n");
}

void addTask(TaskManager *tm) {
    clearScreen();
    
    if (tm->count >= MAX_TASKS) {
        printf("\n✗ Task limit reached!\n");
        pauseScreen();
        return;
    }
    
    Task *t = &tm->tasks[tm->count];
    t->id = tm->nextId++;
    
    printf("\n═══ ADD NEW TASK ═══\n\n");
    
    getStringInput("Task Title", t->title, MAX_TITLE);
    getStringInput("Description", t->description, MAX_DESC);
    getStringInput("Category", t->category, MAX_CATEGORY);
    
    printf("\nPriority Levels:\n");
    printf("  1. Low\n  2. Medium\n  3. High\n  4. Urgent\n");
    t->priority = getIntInput("Select Priority", 1, 4);
    
    printf("\nStatus:\n");
    printf("  1. To Do\n  2. In Progress\n  3. Completed\n  4. Cancelled\n");
    t->status = getIntInput("Select Status", 1, 4);
    
    t->created = time(NULL);
    t->deadline = getDateInput("Deadline (YYYY-MM-DD)");
    t->completed = 0;
    
    tm->count++;
    
    printf("\n✓ Task #%d added successfully!\n", t->id);
    pauseScreen();
}

void viewAllTasks(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    printf("\n═══ ALL TASKS (%d total) ═══\n\n", tm->count);
    printf("%-4s %-30s %-15s %-10s %-12s\n", "ID", "Title", "Category", "Priority", "Status");
    printf("────────────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < tm->count; i++) {
        Task *t = &tm->tasks[i];
        printf("%-4d %-30.30s %-15.15s %-10s %-12s\n", 
               t->id, t->title, t->category, 
               getPriorityString(t->priority), 
               getStatusString(t->status));
    }
    
    pauseScreen();
}

void viewTaskById(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    int id = getIntInput("Enter Task ID", 1, tm->nextId - 1);
    
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].id == id) {
            displayTask(&tm->tasks[i]);
            pauseScreen();
            return;
        }
    }
    
    printf("\n✗ Task not found!\n");
    pauseScreen();
}

void updateTask(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    int id = getIntInput("Enter Task ID to update", 1, tm->nextId - 1);
    
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].id == id) {
            Task *t = &tm->tasks[i];
            
            printf("\n═══ UPDATE TASK #%d ═══\n", id);
            printf("Leave empty to keep current value\n\n");
            
            char buffer[MAX_DESC];
            
            printf("Current Title: %s\n", t->title);
            getStringInput("New Title", buffer, MAX_TITLE);
            if (strlen(buffer) > 0) strcpy(t->title, buffer);
            
            printf("\nCurrent Description: %s\n", t->description);
            getStringInput("New Description", buffer, MAX_DESC);
            if (strlen(buffer) > 0) strcpy(t->description, buffer);
            
            printf("\nCurrent Category: %s\n", t->category);
            getStringInput("New Category", buffer, MAX_CATEGORY);
            if (strlen(buffer) > 0) strcpy(t->category, buffer);
            
            printf("\nCurrent Priority: %s\n", getPriorityString(t->priority));
            printf("Priority: 1=Low, 2=Medium, 3=High, 4=Urgent, 0=Skip\n");
            int p = getIntInput("New Priority", 0, 4);
            if (p > 0) t->priority = p;
            
            printf("\nCurrent Status: %s\n", getStatusString(t->status));
            printf("Status: 1=ToDo, 2=InProgress, 3=Completed, 4=Cancelled, 0=Skip\n");
            int s = getIntInput("New Status", 0, 4);
            if (s > 0) {
                t->status = s;
                if (s == STATUS_COMPLETED) t->completed = 1;
            }
            
            printf("\n✓ Task updated successfully!\n");
            pauseScreen();
            return;
        }
    }
    
    printf("\n✗ Task not found!\n");
    pauseScreen();
}

void deleteTask(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    int id = getIntInput("Enter Task ID to delete", 1, tm->nextId - 1);
    
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].id == id) {
            printf("\nTask: %s\n", tm->tasks[i].title);
            printf("Are you sure you want to delete? (y/n): ");
            
            char confirm;
            scanf(" %c", &confirm);
            getchar();
            
            if (confirm == 'y' || confirm == 'Y') {
                for (int j = i; j < tm->count - 1; j++) {
                    tm->tasks[j] = tm->tasks[j + 1];
                }
                tm->count--;
                printf("\n✓ Task deleted successfully!\n");
            } else {
                printf("\n✗ Deletion cancelled.\n");
            }
            
            pauseScreen();
            return;
        }
    }
    
    printf("\n✗ Task not found!\n");
    pauseScreen();
}

void markTaskComplete(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    int id = getIntInput("Enter Task ID to mark complete", 1, tm->nextId - 1);
    
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].id == id) {
            tm->tasks[i].status = STATUS_COMPLETED;
            tm->tasks[i].completed = 1;
            printf("\n✓ Task marked as complete!\n");
            pauseScreen();
            return;
        }
    }
    
    printf("\n✗ Task not found!\n");
    pauseScreen();
}

void searchTasks(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    char keyword[MAX_TITLE];
    getStringInput("Enter search keyword", keyword, MAX_TITLE);
    
    printf("\n═══ SEARCH RESULTS ═══\n\n");
    
    int found = 0;
    for (int i = 0; i < tm->count; i++) {
        Task *t = &tm->tasks[i];
        if (strstr(t->title, keyword) || strstr(t->description, keyword) || 
            strstr(t->category, keyword)) {
            displayTaskSummary(t, ++found);
        }
    }
    
    if (found == 0) {
        printf("No tasks found matching '%s'\n", keyword);
    } else {
        printf("\nFound %d task(s)\n", found);
    }
    
    pauseScreen();
}

void filterByStatus(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    printf("\nStatus Filter:\n");
    printf("  1. To Do\n  2. In Progress\n  3. Completed\n  4. Cancelled\n");
    int status = getIntInput("Select Status", 1, 4);
    
    printf("\n═══ FILTERED TASKS ═══\n\n");
    
    int found = 0;
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].status == status) {
            displayTaskSummary(&tm->tasks[i], ++found);
        }
    }
    
    if (found == 0) {
        printf("No tasks found with this status\n");
    } else {
        printf("\nFound %d task(s)\n", found);
    }
    
    pauseScreen();
}

void filterByPriority(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    printf("\nPriority Filter:\n");
    printf("  1. Low\n  2. Medium\n  3. High\n  4. Urgent\n");
    int priority = getIntInput("Select Priority", 1, 4);
    
    printf("\n═══ FILTERED TASKS ═══\n\n");
    
    int found = 0;
    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].priority == priority) {
            displayTaskSummary(&tm->tasks[i], ++found);
        }
    }
    
    if (found == 0) {
        printf("No tasks found with this priority\n");
    } else {
        printf("\nFound %d task(s)\n", found);
    }
    
    pauseScreen();
}

void filterByCategory(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    char category[MAX_CATEGORY];
    getStringInput("Enter category name", category, MAX_CATEGORY);
    
    printf("\n═══ FILTERED TASKS ═══\n\n");
    
    int found = 0;
    for (int i = 0; i < tm->count; i++) {
        if (strcmp(tm->tasks[i].category, category) == 0) {
            displayTaskSummary(&tm->tasks[i], ++found);
        }
    }
    
    if (found == 0) {
        printf("No tasks found in category '%s'\n", category);
    } else {
        printf("\nFound %d task(s)\n", found);
    }
    
    pauseScreen();
}

void sortTasks(TaskManager *tm) {
    clearScreen();
    
    if (tm->count == 0) {
        printf("\n✗ No tasks found!\n");
        pauseScreen();
        return;
    }
    
    printf("\nSort by:\n");
    printf("  1. Priority (High to Low)\n");
    printf("  2. Deadline (Nearest first)\n");
    printf("  3. Created Date (Newest first)\n");
    printf("  4. Title (A-Z)\n");
    
    int choice = getIntInput("Select sort option", 1, 4);
    
    for (int i = 0; i < tm->count - 1; i++) {
        for (int j = 0; j < tm->count - i - 1; j++) {
            int swap = 0;
            
            switch (choice) {
                case 1:
                    swap = tm->tasks[j].priority < tm->tasks[j + 1].priority;
                    break;
                case 2:
                    swap = tm->tasks[j].deadline > tm->tasks[j + 1].deadline;
                    break;
                case 3:
                    swap = tm->tasks[j].created < tm->tasks[j + 1].created;
                    break;
                case 4:
                    swap = strcmp(tm->tasks[j].title, tm->tasks[j + 1].title) > 0;
                    break;
            }
            
            if (swap) {
                Task temp = tm->tasks[j];
                tm->tasks[j] = tm->tasks[j + 1];
                tm->tasks[j + 1] = temp;
            }
        }
    }
    
    printf("\n✓ Tasks sorted successfully!\n");
    pauseScreen();
}

void viewStatistics(TaskManager *tm) {
    clearScreen();
    
    printf("\n═══ TASK STATISTICS ═══\n\n");
    
    int todo = 0, inProgress = 0, completed = 0, cancelled = 0;
    int low = 0, medium = 0, high = 0, urgent = 0;
    
    for (int i = 0; i < tm->count; i++) {
        switch (tm->tasks[i].status) {
            case STATUS_TODO: todo++; break;
            case STATUS_IN_PROGRESS: inProgress++; break;
            case STATUS_COMPLETED: completed++; break;
            case STATUS_CANCELLED: cancelled++; break;
        }
        
        switch (tm->tasks[i].priority) {
            case PRIORITY_LOW: low++; break;
            case PRIORITY_MEDIUM: medium++; break;
            case PRIORITY_HIGH: high++; break;
            case PRIORITY_URGENT: urgent++; break;
        }
    }
    
    printf("Total Tasks: %d\n\n", tm->count);
    
    printf("Status Breakdown:\n");
    printf("  To Do:       %d (%.1f%%)\n", todo, tm->count > 0 ? (todo * 100.0 / tm->count) : 0);
    printf("  In Progress: %d (%.1f%%)\n", inProgress, tm->count > 0 ? (inProgress * 100.0 / tm->count) : 0);
    printf("  Completed:   %d (%.1f%%)\n", completed, tm->count > 0 ? (completed * 100.0 / tm->count) : 0);
    printf("  Cancelled:   %d (%.1f%%)\n", cancelled, tm->count > 0 ? (cancelled * 100.0 / tm->count) : 0);
    
    printf("\nPriority Breakdown:\n");
    printf("  Low:         %d (%.1f%%)\n", low, tm->count > 0 ? (low * 100.0 / tm->count) : 0);
    printf("  Medium:      %d (%.1f%%)\n", medium, tm->count > 0 ? (medium * 100.0 / tm->count) : 0);
    printf("  High:        %d (%.1f%%)\n", high, tm->count > 0 ? (high * 100.0 / tm->count) : 0);
    printf("  Urgent:      %d (%.1f%%)\n", urgent, tm->count > 0 ? (urgent * 100.0 / tm->count) : 0);
    
    if (tm->count > 0) {
        float completionRate = (completed * 100.0) / tm->count;
        printf("\nCompletion Rate: %.1f%%\n", completionRate);
    }
    
    pauseScreen();
}

void displayTask(Task *t) {
    char created[26], deadline[26];
    struct tm *tm_info;
    
    tm_info = localtime(&t->created);
    strftime(created, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    tm_info = localtime(&t->deadline);
    strftime(deadline, 26, "%Y-%m-%d", tm_info);
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("  TASK #%d\n", t->id);
    printf("╠════════════════════════════════════════╣\n");
    printf("  Title:       %s\n", t->title);
    printf("  Category:    %s\n", t->category);
    printf("  Priority:    %s\n", getPriorityString(t->priority));
    printf("  Status:      %s\n", getStatusString(t->status));
    printf("  Created:     %s\n", created);
    printf("  Deadline:    %s\n", deadline);
    printf("  Description:\n  %s\n", t->description);
    printf("╚════════════════════════════════════════╝\n");
}

void displayTaskSummary(Task *t, int index) {
    printf("%d. [#%d] %s\n", index, t->id, t->title);
    printf("   Category: %s | Priority: %s | Status: %s\n\n", 
           t->category, getPriorityString(t->priority), getStatusString(t->status));
}

char* getPriorityString(Priority p) {
    switch (p) {
        case PRIORITY_LOW: return "Low";
        case PRIORITY_MEDIUM: return "Medium";
        case PRIORITY_HIGH: return "High";
        case PRIORITY_URGENT: return "Urgent";
        default: return "Unknown";
    }
}

char* getStatusString(Status s) {
    switch (s) {
        case STATUS_TODO: return "To Do";
        case STATUS_IN_PROGRESS: return "In Progress";
        case STATUS_COMPLETED: return "Completed";
        case STATUS_CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

int getIntInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s (%d-%d): ", prompt, min, max);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            getchar();
            return value;
        }
        printf("✗ Invalid input! Please enter a number between %d and %d.\n", min, max);
        while (getchar() != '\n');
    }
}

void getStringInput(const char *prompt, char *buffer, int maxLen) {
    printf("%s: ", prompt);
    fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

time_t getDateInput(const char *prompt) {
    struct tm tm = {0};
    char dateStr[11];
    
    while (1) {
        printf("%s: ", prompt);
        fgets(dateStr, sizeof(dateStr), stdin);
        
        if (sscanf(dateStr, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3) {
            tm.tm_year -= 1900;
            tm.tm_mon -= 1;
            tm.tm_hour = 23;
            tm.tm_min = 59;
            tm.tm_sec = 59;
            return mktime(&tm);
        }
        
        printf("✗ Invalid date format! Use YYYY-MM-DD\n");
    }
}

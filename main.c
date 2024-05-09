#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 1GB of memory
#define MAX_MEM_SIZE 1074000000
// #define MAX_MEM_SIZE 6 * PAGE_SIZE

// 8kbytes
#define PAGE_SIZE 8192

#define NUMBER_OF_FRAMES MAX_MEM_SIZE / PAGE_SIZE

#define FILE_EXT ".prog"

// command for cleaning the cli.
// change to your system's equivalent or remove altogether
#define CLEAR_SCREEN system("clear");

// struct prototypes that were required for declarations further down
struct proc;
struct semaphor;
struct sem_li;

// Data types:
//------------------------------------------------------------------------------
typedef struct memoryPage {
    // char data[PAGE_SIZE]; //doesn't actually need to store any data
    long *associated_pTable_entry;
    char reference_bit;
} memPage;

typedef struct memoryPageTable {
    long *address; // address[virt] == phys;
    //-1 means it's stored in disk
} pageTable_t;

typedef struct memoryFrameTable {
    // list associating each and every frame in memory to the page stored in it
    memPage *frame[NUMBER_OF_FRAMES];
} frameTable_t;

typedef struct cmd {
    int arg;
    char *call;
} command_t;

typedef struct proc {
    char name[17]; // limited to 16 bytes
    int SID; // Segment ID (not used)
    int priority; // won't actually be used
    int seg_size; // in bytes
    char *used_semaphores; // list of semaphores, separated by spaces
    pageTable_t *pTable;
    command_t **code; // list of commands that comprise the code of the program
    int nCommands; // number of commands the program has

} Process;

typedef struct bcp_item {
    Process *proc;
    int next_instruction;
    char status; // r: ready, R: running, b: blocked, i:inactive
    long remaining_time;
    int PID;
    struct bcp_item *next;
} BCPitem_t;

typedef struct bcp {
    BCPitem_t *head;
} BCP_t;

typedef struct ioop {
    BCPitem_t *process; // stores a pointer to the blocked process
    char type; // r: read, w: write, p: print
    int remaining_time;
    struct ioop *next;
} IOop_t;

typedef struct ioqueue {
    IOop_t *head;
} IOqueue_t;

typedef struct semaphor {
    pthread_mutex_t mutex_lock;
    volatile int v; // semaphore's value
    char name;
    int refcount; // counts the number of processes that use the semaphore in
                  // question
    struct sem_li *waiting_list; // head of a semaphore's waiting process list
    struct semaphor *next;

} semaphore_t;

typedef struct sem_li // list of processes waiting on a given semaphore
{
    BCPitem_t *proc;
    struct sem_li *next;
} sem_list_item_t;

typedef struct all_sem_li // list of all existing semaphores
{
    semaphore_t *head;
} all_sem_list_t;

// Global variables:
//------------------------------------------------------------------------------
long available_memory = MAX_MEM_SIZE;
frameTable_t frameTable;
BCP_t BCP;
IOqueue_t IOqueue;
BCPitem_t *curr_running = NULL;
BCPitem_t *prev_running = NULL;
volatile int PID = 0;
volatile int stop = 0;
sem_t sem;
all_sem_list_t existing_semaphores;

// Functions:
//------------------------------------------------------------------------------
void semaphore_init(semaphore_t *semaph, volatile int v);
void proc_wakeup(BCPitem_t *proc);
void processFinish(BCPitem_t *proc);

void init_data_structures()
{
    // frameTable
    for (int i = 0; i < NUMBER_OF_FRAMES; i++)
        frameTable.frame[i] = NULL;

    // BCP
    BCP.head = NULL;

    // IO queue
    IOqueue.head = NULL;

    // mutex semaphore
    sem_init(&sem, 0, 1);
}

void semaphore_init(semaphore_t *semaph, volatile int v)
{
    pthread_mutex_init(&semaph->mutex_lock, NULL);
    semaph->waiting_list = NULL;
    semaph->v = v;
}

void showMenu()
{
    CLEAR_SCREEN
    printf("┌──────────────────────────────────────┐\n");
    printf("│       Operating System Simulator     │\n");
    printf("└──────────────────────────────────────┘\n");

    printf("┌──────────────────────────────────────┐\n");
    printf("│                Menu:                 │\n");
    printf("├──────────────────────────────────────┤\n");
    printf("│ [1] Create process                   │\n");
    printf("│ [2] View process info                │\n");
    printf("│ [3] View semaphore info              │\n");
    printf("│ [4] View memory info                 │\n");
    printf("│ [0] Quit                             │\n");
    printf("└──────────────────────────────────────┘\n");
    printf("  Currently running: ");
    if (curr_running) {
        printf("%s (%d)\n", curr_running->proc->name, curr_running->PID);
    } else
        printf("nothing\n");
    printf("\n  Option: \n");
}

void proc_sleep(BCPitem_t *proc)
{
    proc->status = 'i';
    return;
}

// inserts a semaphore into the global list of all existing semaphores
void insert_semaphore(semaphore_t *item)
{
    if (existing_semaphores.head == NULL) {
        existing_semaphores.head = item;
        return;
    }
    semaphore_t *aux, *prev = NULL;
    for (aux = existing_semaphores.head; aux; prev = aux, aux = aux->next) {
        // semaphore by this name already exists
        if (item->name == aux->name) {
            aux->refcount++;
            free(item);
            return;
        }
    }
    item->next = aux;
    if (prev)
        prev->next = item;
    else
        existing_semaphores.head = item;
}

// creates semaphore with given name and adds it to the list of all semaphores
void createSemaphore(char name)
{
    semaphore_t *new = malloc(sizeof(semaphore_t));
    new->next = NULL;
    new->name = name;
    new->refcount = 1;
    semaphore_init(new, 0);
    insert_semaphore(new);
}

// queues PROCESSES blocked by a failed call to semaphoreP()
void sem_queue(sem_list_item_t **list, BCPitem_t *proc)
{
    sem_list_item_t *new = malloc(sizeof(sem_list_item_t));
    new->next = NULL;
    new->proc = proc;

    if (*list == NULL) {
        *list = new;
        return;
    }
    sem_list_item_t *aux, *prev = NULL;
    for (aux = *list; aux; prev = aux, aux = aux->next)
        ;
    new->next = aux;
    if (prev)
        prev->next = new;
    else
        (*list) = new;
}

// takes the semaphore and the process that requested it
void semaphoreP(semaphore_t *semaph, BCPitem_t *proc)
{
    pthread_mutex_lock(&semaph->mutex_lock);
    if (semaph->v < 0) {
        sem_queue(&semaph->waiting_list, proc);
        proc_sleep(proc);
    }
    semaph->v--;
    pthread_mutex_unlock(&semaph->mutex_lock);
}

// releases the lock and wakes up the first waiting process
void semaphoreV(semaphore_t *semaph)
{
    pthread_mutex_lock(&semaph->mutex_lock);
    semaph->v++;
    if (semaph->v <= 0) {
        if (semaph->waiting_list) {
            BCPitem_t *proc = semaph->waiting_list->proc;
            semaph->waiting_list = semaph->waiting_list->next;
            proc_wakeup(proc);
        }
    }
    pthread_mutex_unlock(&semaph->mutex_lock);
}

// checks if any of the pages belonging to a process is stored in a given memory
// frame
int inFrameTable(int pos, Process *proc)
{

    if (frameTable.frame[pos] == NULL)
        for (int i = 0; i < ceil((float)proc->seg_size / PAGE_SIZE); i++)
            if (proc->pTable->address + i
                == frameTable.frame[pos]->associated_pTable_entry)
                return 1;
    return 0;
}

// loads a process' missing pages into memory and sets their virtual addresses
void memLoadReq(Process *proc)
{
    int nFrames = ceil((float)proc->seg_size / PAGE_SIZE);
    long *address = proc->pTable->address;
    int i, j, k = 0;
    int missing = 0;
    memPage *newPage = NULL;
    long *associated_page;

    for (i = 0; i < nFrames; i++)
        missing++;

    for (i = 0; i < nFrames; i++)
        if (address[i] == -1) // not in main memory
        {
            if (available_memory == 0) {
                // find first page with reference bit 0
                for (j = 0; j <= NUMBER_OF_FRAMES && missing > 0; j++) {
                    if (j == NUMBER_OF_FRAMES)
                        j = 0;

                    if (!inFrameTable(
                            j, proc)) // make sure that a page belonging to this
                                      // process is not unloaded in order to fit
                                      // another
                    {

                        if (frameTable.frame[j]->reference_bit == 1)
                            frameTable.frame[j]->reference_bit = 0;
                        else {
                            *(frameTable.frame[j]->associated_pTable_entry)
                                = -1;
                            free(frameTable.frame[j]);
                            newPage = malloc(sizeof(memPage));
                            memset(newPage, 0, sizeof(memPage));
                            newPage->associated_pTable_entry
                                = &(proc->pTable->address[i]);
                            frameTable.frame[j] = newPage;
                            frameTable.frame[j]->reference_bit
                                = 1; // sets reference bit of newPage to 1
                            proc->pTable->address[i] = j;
                            break;
                        }
                    }
                }

            } else // memory is available for the current page
            {
                for (k = 0; k < NUMBER_OF_FRAMES && missing > 0; k++)
                    if (frameTable.frame[k] == NULL) {
                        newPage = malloc(sizeof(memPage));
                        memset(newPage, 0, sizeof(memPage));
                        newPage->associated_pTable_entry
                            = &(proc->pTable->address[i]);
                        frameTable.frame[k] = newPage;
                        frameTable.frame[k]->reference_bit = 1;

                        // updates page table
                        proc->pTable->address[i] = k;
                        break;
                    }
                available_memory -= PAGE_SIZE;
            }
            missing--;
        } else // no page fault, set reference bit to 1
            frameTable.frame[address[i]]->reference_bit = 1;
}

// adds process to a queue of IO operations
void io_queue_add(BCPitem_t *item, char type)
{
    IOop_t *new = malloc(sizeof(IOop_t));

    // assuming time for operation is the same as arg since it wasn't specified
    new->remaining_time = item->proc->code[item->next_instruction]->arg;
    new->process = item;
    new->type = type;
    new->next = NULL;

    if (IOqueue.head == NULL) {
        IOqueue.head = new;
        return;
    }

    IOop_t *aux, *prev = NULL;
    for (aux = IOqueue.head; aux; prev = aux, aux = aux->next)
        ;
    new->next = aux;
    if (prev)
        prev->next = new;
    else
        IOqueue.head = new;
}

void queueProcess(BCPitem_t *proc) // adds proc into the scheduling list
{
    if (BCP.head == NULL) {
        BCP.head = proc;
        return;
    }

    // ordering by shortest remaining time
    BCPitem_t *aux, *prev = NULL;
    for (aux = BCP.head;
         aux != NULL && aux->remaining_time < proc->remaining_time;
         prev = aux, aux = aux->next)
        ;
    proc->next = aux;
    if (prev)
        prev->next = proc;
    else // proc is the new head
        BCP.head = proc;
}

void dequeueProcess(BCPitem_t *item)
{
    BCPitem_t *aux = BCP.head, *prev = NULL;
    for (; aux && aux != item; prev = aux, aux = aux->next)
        ;
    if (aux) {
        if (prev)
            prev->next = aux->next;
        else // aux is the current head
            BCP.head = BCP.head->next;
        aux->next = NULL;
    } else
        printf("Something went really wrong!!\n");
}

// wakes up a "sleeping" (waiting)  process
void proc_wakeup(BCPitem_t *proc)
{
    proc->status = 'r';
    dequeueProcess(proc);
    queueProcess(proc);
}

// interrupt current process and reschedule it
void processInterrupt()
{
    BCPitem_t *curr = curr_running;
    if (curr) {
        curr->status = 'r';
        dequeueProcess(curr);
        memLoadReq(curr->proc);
        queueProcess(curr);
    }
}

// advances the queue of IO operations by a single unit of time
void advanceIOqueue()
{
    IOop_t *aux = IOqueue.head;
    if (aux) {
        aux->remaining_time--;
        aux->process->remaining_time--;
        aux->process->proc->code[aux->process->next_instruction]->arg--;
        if (aux->remaining_time <= 0) // IO operation finished
        {
            aux->process->status = 'r';
            aux->process->next_instruction++;

            // this IO op was the last instruction of the program
            if (aux->process->next_instruction
                >= aux->process->proc->nCommands) {
                IOqueue.head = IOqueue.head->next;
                processFinish(aux->process);
                showMenu();
                return;
            }
            IOqueue.head = IOqueue.head->next;

            // reschedule the currently running process
            processInterrupt();

            // reschedule the now unblocked process
            dequeueProcess(aux->process);
            queueProcess(aux->process);
        }
    }
}

// retrieve a semaphore object given its name
semaphore_t *retrieveSemaphore(char name)
{
    semaphore_t *aux = existing_semaphores.head;
    while (aux) {
        if (aux->name == name)
            return aux;
        aux = aux->next;
    }
    printf("something went really wrong !!!!\n");
    sleep(1);
    return NULL;
}

void removeSemaphore(semaphore_t *item)
{
    semaphore_t *aux = existing_semaphores.head, *prev = NULL;
    for (; aux && aux != item; prev = aux, aux = aux->next)
        ;
    if (aux) {
        if (prev)
            prev->next = aux->next;
        else // aux is the current head
            existing_semaphores.head = existing_semaphores.head->next;
        free(aux);
    }
}
// properly frees the contents of a BCP register
void Free(BCPitem_t *a)
{
    int i = 0;

    // decrement reference count on all of the process' used semaphores
    semaphore_t *aux;
    while (a->proc->used_semaphores[i] != '\0') {
        aux = existing_semaphores.head;
        while (aux) {
            if (aux->name == a->proc->used_semaphores[i]) {
                aux->refcount--;
                if (aux->refcount
                    <= 0) // no processes using this semaphore anymore
                    removeSemaphore(aux);
                break;
            }
            aux = aux->next;
        }
        i++;
    }

    long *address = a->proc->pTable->address;
    int nFrames = ceil((float)a->proc->seg_size / PAGE_SIZE);
    for (i = 0; i < nFrames; i++) // free the virtual memory
    {
        if (address[i] != -1) {
            frameTable.frame[address[i]] = NULL;
            available_memory += PAGE_SIZE;
        }
    }
    free(a->proc->pTable);
    free(a->proc->code);
    free(a->proc);
    free(a);
}

int validateFilename(char *filename)
{
    int i;
    for (i = strlen(filename) - 1; i >= 0; i--) {
        if (filename[i] == '.')
            break;
    }
    if (strcmp(&filename[i], FILE_EXT) == 0)
        return 1;

    return 0;
}

// translate the program's code to an array of instructions
command_t **parsecommands(char *code, int *inst_counter)
{
    char temp[100];
    int i;
    int count = 0;
    for (i = 0; code[i] != '\0'; i++)
        if (code[i] == '\n' && code[i + 1] != '\n')
            count++;

    if (code[i - 1] != '\n') // file has no trailing newline
        count++;

    *inst_counter = count;

    char **lines = malloc(count * sizeof(char *));
    command_t **cmd = malloc(count * sizeof(command_t *));
    int oldcount = count;

    count = 0;
    int j;

    // break the code into lines
    sem_wait(&sem);
    i = 0;
    while (code[i] != '\0') {
        while (code[i] == '\n'
            && code[i] != '\0') // in case there are more than two newlines at
                                // the end of the file
            i++;

        if (code[i] == '\0')
            break;

        for (j = 0; code[i] != '\n' && code[i] != '\0'; i++, j++) {
            temp[j] = code[i];
        }
        temp[j] = '\0';
        lines[count] = malloc(strlen(temp) + 1);
        strcpy(lines[count], temp);
        count++;
        if (code[i] == '\0')
            break;

        i++; // go to next line
    }
    sem_post(&sem);

    // break each line into a pair of call and arg
    char *arg = NULL;
    for (i = 0; i < count; i++) {
        cmd[i] = malloc(sizeof(command_t));
        cmd[i]->call = strtok(lines[i], " ");
        arg = strtok(NULL, " ");
        if (arg)
            cmd[i]->arg = (int)strtol(arg, NULL, 10);
        else // command is a P() or a V()
            cmd[i]->arg = -1;
    }
    return cmd;
}

// make sure to check for NULL returns whenever this function is called
Process *readProgramfromDisk(char *filename)
{

    /* Use this to limit program files to using a specific extension
    if(!validateFilename(filename))
    {
            printf("Invalid filename!! Only " FILE_EXT " files allowed!\n");
            sleep(2);
            return NULL;
    }
    */

    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Requested file does not exist!\n");
        sleep(2);
        return NULL;
    }

    // get filesize
    fseek(file, 0L, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    Process *proc = malloc(sizeof(Process));

    fscanf(file, "%[^\n]", &proc->name);
    fscanf(file, "%d\n", &proc->SID);
    fscanf(file, "%d\n", &proc->priority);
    fscanf(file, "%d\n", &proc->seg_size);
    proc->seg_size *= 1024; // convert kbytes to bytes

    // initialize page table
    proc->pTable = malloc(sizeof(pageTable_t));
    proc->pTable->address
        = malloc(ceil((float)proc->seg_size / PAGE_SIZE) * sizeof(long));

    int i = 0;
    char c;
    // properly implement this as a list of semaphores, using strtok to remove
    // spaces
    long sem_start_pos = ftell(file);
    char prevchar;
    int num_of_semaphores
        = 0; // one of the characters read is a semaphore, the other a space
    while (1) // figure out how many semaphores the process uses (assuming they
              // only use single-letter names
    {
        c = fgetc(file);
        if (c == '\n' || c == EOF)
            break;
        if (c != ' ')
            num_of_semaphores++;
    }

    // allocate space for a list of used semaphores
    proc->used_semaphores
        = malloc(num_of_semaphores + 1); //+1 for a null terminator
    fseek(file, sem_start_pos, SEEK_SET);
    i = 0;
    while (1) // actually retrieve the semaphores
    {
        c = fgetc(file);
        if (c == '\n' || c == EOF)
            break;
        if (c != ' ') {
            proc->used_semaphores[i] = c;
            i++;
        }
    }
    proc->used_semaphores[num_of_semaphores] = '\0';

    // create the respective semaphores
    for (int num = 0; num < num_of_semaphores; num++) {
        createSemaphore(proc->used_semaphores[num]);
    }

    char *code = malloc(filesize - ftell(file) + 1); //+1 for a null terminator
    i = 0;
    fgetc(file); // consume semaphore's newline
    while (1) // save the remainder of the file as code
    {
        c = fgetc(file);
        if (c == EOF)
            break;
        code[i] = c;
        i++;
    }
    code[i] = '\0';
    int inst_number;
    proc->code = parsecommands(code, &inst_number);
    proc->nCommands = inst_number;

    fclose(file);

    return proc;
}

void printProcessInfo(Process *proc)
{
    printf("Name: %s\nSegment ID: %d\nPriority: %d\nSegment Size: %d bytes\n",
        proc->name, proc->SID, proc->priority, proc->seg_size);
    printf("Used semaphores: ");
    for (int i = 0; proc->used_semaphores[i] != '\0'; i++)
        printf("%c ", proc->used_semaphores[i]);
    printf("\n\n");

    //	printf("Page table:\n");
    //	for(int i = 0; i < ceil((float)proc->seg_size/PAGE_SIZE); i++)
    //		printf("[%d] address: %ld\n",i,proc->pTable->address[i]);
}

int isDigit(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

long calculateRemainingTime(Process *proc)
{
    long remaining_time = 0;
    for (int i = 0; i < proc->nCommands; i++) {
        if (proc->code[i]->arg != -1)
            remaining_time += proc->code[i]->arg;
    }
    return remaining_time;
}

// creates a process from information in a file and schedules it
void processCreate(char *filename)
{
    // create a bcp register of said process
    BCPitem_t *new = malloc(sizeof(BCPitem_t));
    new->proc = NULL;
    new->next = NULL;
    new->next_instruction = 0;
    new->proc = readProgramfromDisk(filename);
    if (!new->proc) {
        printf("Error retrieving program from disk\n");
        sleep(2);
        return;
    }
    long size = new->proc->seg_size;

    // indicate that all of the process' memory pages are currently stored on
    // disk
    for (int i = 0; i < ceil((float)new->proc->seg_size / PAGE_SIZE); i++)
        new->proc->pTable->address[i] = -1;

    // load process into memory
    memLoadReq(new->proc);

    new->PID = PID;
    PID++;
    new->status = 'r';
    if (new->proc == NULL) {
        free(new);
        return;
    }

    new->remaining_time = calculateRemainingTime(new->proc);

    sem_wait(&sem);
    processInterrupt();
    // queue the process in the scheduling list
    queueProcess(new);
    sem_post(&sem);
    printf("started process %s\n\n", new->proc->name);
}

void processFinish(BCPitem_t *proc)
{
    dequeueProcess(proc);
    Free(proc);
}

char *getStatus(char st)
{
    if (st == 'r')
        return "Ready";
    if (st == 'R')
        return "Running";
    if (st == 'b')
        return "Blocked";
    if (st == 'i')
        return "Inactive";
    return "Unknown";
}

void viewProcessInfo()
{
    sem_wait(&sem);
    CLEAR_SCREEN
    if (BCP.head == NULL) {
        printf("No processes currently scheduled!\n");
        sleep(3);
        sem_post(&sem);
        return;
    }
    printf("\nCurrent processes: ");
    printf("\nPID | Name (Status)\n");
    printf("-----------------------------------------\n");
    BCPitem_t *aux = NULL;
    for (aux = BCP.head; aux; aux = aux->next)
        printf("%d | %s (%s)\n", aux->PID, aux->proc->name,
            getStatus(aux->status));

    int search_pid;
    printf("\nWhich process do you want to view info about?\nPID: ");
    scanf("%d", &search_pid);
    int found = 0;
    for (aux = BCP.head; aux; aux = aux->next)
        if (aux->PID == search_pid) {
            printf("\n\n");
            CLEAR_SCREEN
            printProcessInfo(aux->proc);
            printf("Current instruction:\n");
            for (int i = 0; i < aux->proc->nCommands; i++) {
                if (i == aux->next_instruction) {
                    if (aux->proc->code[i]->arg != -1)
                        printf("> %s %d\n", aux->proc->code[i]->call,
                            aux->proc->code[i]->arg);
                    else
                        printf("> %s\n", aux->proc->code[i]->call);
                } else
                    printf("  %s\n", aux->proc->code[i]->call,
                        aux->proc->code[i]->arg);
            }
            printf("\nStatus: %s\n", getStatus(aux->status));
            printf("Remaining time: %ld\n", aux->remaining_time);
            found = 1;
            break;
        }
    if (!found)
        printf("Invalid PID! Process not found.\n");
    sleep(3);
    sem_post(&sem);
    return;
}

void showMemoryInfo()
{
    sem_wait(&sem);
    CLEAR_SCREEN
    printf(
        "Memory used: %d / %d ", MAX_MEM_SIZE - available_memory, MAX_MEM_SIZE);
    printf("(%.1f\%)\n",
        (100 * (MAX_MEM_SIZE - available_memory) / (float)(MAX_MEM_SIZE)));
    sleep(3);
    sem_post(&sem);
}
void showSemaphoreList()
{
    sem_wait(&sem);
    CLEAR_SCREEN
    printf("List of currently existing semaphores:\n");
    semaphore_t *aux = existing_semaphores.head;
    if (!aux) {
        printf("None!!\n");
        sleep(3);
        sem_post(&sem);
        return;
    }
    while (aux) {
        printf("%c | Refcount: %d\n", aux->name, aux->refcount);
        aux = aux->next;
    }
    sleep(3);
    sem_post(&sem);
}
void *menu()
{
    int opt;
    char filename[128];
    do {
        sem_wait(&sem);
        showMenu();
        sem_post(&sem);
        scanf(" %d", &opt);
        switch (opt) {
        case 0:
            stop = 1;
            break;
        case 1:
            sem_wait(&sem);
            printf("Program filename: ");
            scanf(" %[^\n]", filename);
            sem_post(&sem);
            processCreate(filename);
            break;
        case 2:
            viewProcessInfo();
            break;

        case 3:
            showSemaphoreList();
            break;

        case 4:
            showMemoryInfo();
            break;
        default:
            printf("Invalid option!\n");
            break;
        }
    } while (opt != 0);
}

// does all of the interpreting of the code
// runs every unit of time when there's a Running process
void interpreter(BCPitem_t *curr)
{
    Process *proc = curr->proc;
    command_t *instruction = proc->code[curr->next_instruction];

    if (instruction->call[0] == 'e') // exec
    {
        sem_wait(&sem);
        instruction->arg--;
        curr->remaining_time--;

        if (instruction->arg == 0)
            curr->next_instruction++;

        sem_post(&sem);
        return;
    }

    if (instruction->call[0] == 'r') // read
    {
        sem_wait(&sem);
        curr_running = NULL;
        curr->status = 'b';
        io_queue_add(curr, 'r');
        sem_post(&sem);
    }

    if (instruction->call[0] == 'w') // write
    {
        sem_wait(&sem);
        curr_running = NULL;
        curr->status = 'b';
        io_queue_add(curr, 'w');
        sem_post(&sem);
    }

    if (instruction->call[0] == 'p') // print
    {
        sem_wait(&sem);
        curr_running = NULL;
        curr->status = 'b';
        io_queue_add(curr, 'p');
        sem_post(&sem);
    }

    if (instruction->call[0] == 'P') // P(sem)
    {
        sem_wait(&sem);
        // name of the semaphore is on position 2 given that it is on the format
        // P(s)
        semaphore_t *argSem = retrieveSemaphore(instruction->call[2]);
        curr->next_instruction++;
        semaphoreP(argSem, curr);
        sem_post(&sem);
    }

    if (instruction->call[0] == 'V') // V(sem)
    {
        sem_wait(&sem);
        // name of the semaphore is on position 2 given that it is on the format
        // P(s)
        semaphore_t *argSem = retrieveSemaphore(instruction->call[2]);
        curr->next_instruction++;
        semaphoreV(argSem);
        sem_post(&sem);
    }
}
void *mainLoop()
{
    while (!stop) {
        prev_running = curr_running;
        curr_running = BCP.head;

        // skip blocked and inactive processes
        while (curr_running
            && (curr_running->status == 'b' || curr_running->status == 'i'))
            curr_running = curr_running->next;

        if (prev_running != curr_running) {
            sem_wait(&sem);
            showMenu();
            sem_post(&sem);
            if (curr_running) {
                // load process' missing pages into memory
                sem_wait(&sem);
                memLoadReq(curr_running->proc);
                sem_post(&sem);
            }
        }

        if (curr_running) {
            curr_running->status = 'R';
            interpreter(curr_running);
            if (curr_running != NULL
                && curr_running->next_instruction
                    >= curr_running->proc->nCommands) {
                sem_wait(&sem);
                processFinish(curr_running);
                sem_post(&sem);
            }
        }
        sem_wait(&sem);
        advanceIOqueue();
        sem_post(&sem);
        usleep(
            500); // arbitrarily chosen time period to advance each unit of time
    }
}
int main()
{
    init_data_structures();
    pthread_t kernel;
    pthread_t sim_menu;
    pthread_create(&sim_menu, NULL, menu, NULL);
    pthread_create(&kernel, NULL, &mainLoop, NULL);
    pthread_join(kernel, NULL);
    pthread_join(sim_menu, NULL);
}

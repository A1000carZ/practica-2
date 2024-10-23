#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <conio.h>

using namespace std;

const int PAGE_SIZE = 64;
const int TOTAL_MEMORY = 1536;
const int OS_MEMORY = 20;
const int MAX_FRAMES_PER_TASK = 3;
const int AVAILABLE_FRAMES = (TOTAL_MEMORY / PAGE_SIZE);
const int PAGE_SEQUENCE_SIZE = 8;
const int MIN_LINES = 100;
const int MAX_LINES = 1000;
const int NUM_TASKS = 20;

struct PageNode
{
    int pageNumber;
    int frame;
    bool status;
    bool referenced;
    bool modified;
    PageNode *next;
};

struct TaskNode
{
    int taskId;
    int lines;
    PageNode *pmt;
    int sequence[PAGE_SEQUENCE_SIZE];
    TaskNode *next;
};

struct MMTNode
{
    int frameNumber;
    int location;
    int status;
    MMTNode *next;
};

struct Frame
{
    int page;
    bool isEmpty;
    Frame()
    {
        page = -1;
        isEmpty = true;
    }
};

MMTNode *mmt = NULL;
TaskNode *jt = NULL;
int assignedFrames = 0;

void initializeMMT();
void initializeTask(int taskId, int numPages);
int assignFrame();
void generateRandomSequence(TaskNode *task);
void printMMT();
void printEmptyMMT();
void printJT();
void printAllPMT();
void printPMT(int taskId);
void printEmptyPMT(int taskId);
TaskNode *findTask(int taskId);
string toString(int value);
void fifoPageReplacement(const vector<int> &pageSequence);
void printMemoryState(const Frame *memory, MMTNode *mmt, int time, int pageFaults);
void printListaAux(queue<int, deque<int>> fifoQueue); // Function to print the auxiliary list

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    int taskChoice;
    initializeMMT();
    printMMT();

    for (int i = 1; i <= NUM_TASKS; ++i)
    {
        initializeTask(i, MAX_FRAMES_PER_TASK);
    }

    printJT();
    printAllPMT();
    cout << "Ingresa la tarea a utilizar: ";
    cin >> taskChoice;
    if (taskChoice < 1 || taskChoice > NUM_TASKS)
    {
        cout << "Tarea invalida. Intente de nuevo." << endl;
        return 1; // Exit with an error code
    }
    cout << "\nDemonstrating FIFO Page Replacement for Task J" << taskChoice << ":" << endl;
    TaskNode *task1 = findTask(taskChoice);
    if (task1 != NULL)
    {
        vector<int> pageSequence(task1->sequence, task1->sequence + PAGE_SEQUENCE_SIZE);
        fifoPageReplacement(pageSequence);
    }
    getch();
    return 0;
}

void initializeMMT()
{
    for (int i = AVAILABLE_FRAMES - 1; i >= 0; --i)
    {
        int frameLocation = i * PAGE_SIZE;
        MMTNode *newFrame = new MMTNode;
        newFrame->frameNumber = i;
        newFrame->location = frameLocation;
        newFrame->status = 0;
        newFrame->next = mmt;
        mmt = newFrame;
    }
}

void initializeTask(int taskId, int numPages)
{
    int randomLines = rand() % (MAX_LINES - MIN_LINES + 1) + MIN_LINES;

    TaskNode *newTask = new TaskNode;
    newTask->taskId = taskId;
    newTask->lines = randomLines;
    newTask->pmt = NULL;
    newTask->next = NULL;

    if (jt == NULL)
    {
        jt = newTask;
    }
    else
    {
        TaskNode *temp = jt;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newTask;
    }

    PageNode *lastPage = NULL;
    for (int i = 0; i < numPages; ++i)
    {
        if (i >= MAX_FRAMES_PER_TASK)
        {
            cout << "Error: La tarea J" << taskId << " excede el número máximo de marcos (" << MAX_FRAMES_PER_TASK << ")." << endl;
            break;
        }

        int assignedFrame = assignFrame();
        PageNode *newPage = new PageNode;
        newPage->pageNumber = i;
        newPage->frame = assignedFrame;
        newPage->status = (assignedFrame != -1);
        newPage->referenced = false;
        newPage->modified = false;
        newPage->next = NULL;

        if (lastPage == NULL)
        {
            newTask->pmt = newPage;
        }
        else
        {
            lastPage->next = newPage;
        }
        lastPage = newPage;
    }

    generateRandomSequence(newTask);
}

int assignFrame()
{
    MMTNode *frame = mmt;
    while (frame != NULL)
    {
        if (frame->status == 0)
        {
            frame->status = 1;
            assignedFrames++;
            return frame->frameNumber; // Return the frame number instead of location
        }
        frame = frame->next;
    }
    return -1;
}

void generateRandomSequence(TaskNode *task)
{
    task->sequence[0] = 0;
    for (int i = 1; i < PAGE_SEQUENCE_SIZE; ++i)
    {
        task->sequence[i] = rand() % MAX_FRAMES_PER_TASK;
    }
}

void printMMT()
{
    cout << "Tabla de Mapeo de Memoria (MMT):" << endl;
    cout << setw(10) << "Marco" << setw(15) << "Ubicación(KB)" << setw(10) << "Estado" << endl;
    MMTNode *frame = mmt;
    while (frame != NULL)
    {
        cout << setw(10) << frame->frameNumber << setw(15) << frame->location << setw(10) << frame->status << endl;
        frame = frame->next;
    }
    cout << endl;
}

void printEmptyMMT()
{
    cout << "Tabla de Mapeo de Memoria (MMT):" << endl;
    cout << setw(10) << "Marco" << setw(15) << "Ubicación(KB)" << setw(10) << "Estado" << endl;
    MMTNode *frame = mmt;
    while (frame != NULL)
    {
        cout << setw(10) << frame->frameNumber << setw(15) << frame->location << setw(10) << 0 << endl;
        frame = frame->next;
    }
    cout << endl;
}

void printJT()
{
    cout << "Tabla de Trabajos (JT) con Secuencia Aleatoria:" << endl;
    cout << setw(10) << "Tarea" << setw(10) << "Líneas" << setw(30) << "Secuencia de Páginas" << endl;
    TaskNode *task = jt;
    while (task != NULL)
    {
        cout << setw(10) << ("J" + toString(task->taskId)) << setw(10) << task->lines;
        cout << "{";
        for (int i = 0; i < PAGE_SEQUENCE_SIZE; ++i)
        {
            cout << "P" << task->sequence[i];
            if (i < PAGE_SEQUENCE_SIZE - 1)
                cout << ",";
        }
        cout << "}" << endl;
        task = task->next;
    }
    cout << endl;
}

void printAllPMT()
{
    TaskNode *task = jt;
    while (task != NULL)
    {
        printPMT(task->taskId);
        task = task->next;
    }
}

void printPMT(int taskId)
{
    TaskNode *task = findTask(taskId);
    if (task == NULL)
    {
        cout << "Tarea no encontrada." << endl;
        return;
    }

    cout << "Tabla de Mapeo de Páginas (PMT) para la Tarea J" << taskId << endl;
    cout << setw(10) << "Página" << setw(10) << "Marco(KB)" << setw(10) << "Estado" << setw(10) << "Referencia" << setw(10) << "Modificado" << endl;

    PageNode *page = task->pmt;
    while (page != NULL)
    {
        cout << setw(10) << page->pageNumber
             << setw(10) << (page->frame != -1 ? toString(page->frame) : string("-"))
             << setw(10) << "0" << setw(10) << "0" << setw(10) << "0" << endl;
        page = page->next;
    }
    cout << endl;
}

void printEmptyPMT(int taskId)
{
    cout << "Tabla de Mapeo de Páginas (PMT) para la Tarea J" << taskId << endl;
    cout << setw(10) << "Página" << setw(10) << "Marco(KB)" << setw(10) << "Estado" << setw(10) << "Referencia" << setw(10) << "Modificado" << endl;

    for (int i = 0; i < MAX_FRAMES_PER_TASK; ++i)
    {
        cout << setw(10) << i << setw(10) << "-" << setw(10) << "0" << setw(10) << "0" << setw(10) << "0" << endl;
    }
    cout << endl;
}

TaskNode *findTask(int taskId)
{
    TaskNode *task = jt;
    while (task != NULL)
    {
        if (task->taskId == taskId)
            return task;
        task = task->next;
    }
    return NULL;
}

void fifoPageReplacement(const vector<int> &pageSequence)
{
    Frame memory[3];                  // Exactly 3 frames: M1, M2, M3
    queue<int, deque<int>> fifoQueue; // Queue to track frame order
    int pageFaults = 0;

    // Initialize frames as empty
    for (int i = 0; i < 3; i++)
    {
        memory[i].page = -1;
        memory[i].isEmpty = true;
    }

    cout << "\nFIFO (First In First Out)" << endl;
    cout << "-------------------------" << endl;

    for (int time = 0; time < PAGE_SEQUENCE_SIZE; time++)
    {
        int currentPage = pageSequence[time];
        bool pageFault = true;

        cout << "\nTiempo T" << time << ":" << endl;
        cout << "Referencia a P" << currentPage << endl;

        // Check if page is already in memory
        for (int i = 0; i < 3; i++)
        {
            if (!memory[i].isEmpty && memory[i].page == currentPage)
            {
                pageFault = false;
                break;
            }
        }

        if (pageFault)
        {
            pageFaults++;
            cout << "* Fallo de página *" << endl;

            // Find empty frame or use FIFO replacement
            int frameToUse;
            if (fifoQueue.size() < 3)
            {
                // Find first empty frame
                for (int i = 0; i < 3; i++)
                {
                    if (memory[i].isEmpty)
                    {
                        frameToUse = i;
                        break;
                    }
                }
                cout << "Usando marco libre M" << (frameToUse + 1) << endl;
            }
            else
            {
                // Use FIFO replacement
                frameToUse = fifoQueue.front();
                cout << "Reemplazando P" << memory[frameToUse].page
                     << " en M" << (frameToUse + 1) << " con P" << currentPage << endl;
                fifoQueue.pop();
            }

            // Update memory and queue
            memory[frameToUse].page = currentPage;
            memory[frameToUse].isEmpty = false;
            fifoQueue.push(frameToUse);

            // Update MMT status
            MMTNode *current = mmt;
            for (int i = 0; i < frameToUse && current != NULL; i++)
            {
                current = current->next;
            }
            if (current != NULL)
            {
                current->status = 1;
            }
        }
        else
        {
            cout << "Página P" << currentPage << " ya está en memoria" << endl;
        }

        // Print current state
        cout << "\nEstado de Marcos:" << endl;
        for (int i = 0; i < 3; i++)
        {
            cout << "M" << (i + 1) << ": ";
            if (!memory[i].isEmpty)
            {
                cout << "P" << memory[i].page;
            }
            else
            {
                cout << "--";
            }
            cout << endl;
        }

        // Print FIFO Queue contents
        cout << "\nCola FIFO: ";
        queue<int, deque<int>> tempQueue = fifoQueue;
        while (!tempQueue.empty())
        {
            int frame = tempQueue.front();
            cout << "P" << memory[frame].page << " ";
            tempQueue.pop();
        }
        cout << endl;

        // Print page replacement information
        if (pageFault)
        {
            cout << "Reemplazo: ";
            if (fifoQueue.size() <= 3)
            {
                cout << "Se usó marco libre" << endl;
            }
            else
            {
                cout << "FIFO seleccionó M" << (fifoQueue.front() + 1) << endl;
            }
        }
    }

    cout << "\nResumen:" << endl;
    cout << "Total de fallos de página: " << pageFaults << endl;
}

void printMemoryState(const Frame* memory, MMTNode* mmt, int time, int pageFaults) {
    // Print MMT
    cout << "--TABLA DE MAPA DE MEMORIA--" << endl;
    cout << setw(8) << "Marco" << setw(12) << "Localidad" << setw(10) << "Estado" << endl;

    MMTNode* current = mmt;
    int count = 0;
    while (current != NULL && count < MAX_FRAMES_PER_TASK) {
        cout << setw(8) << current->frameNumber
             << setw(12) << current->location
             << setw(10) << current->status << endl;
        current = current->next;
        count++;
    }
    cout << endl;

    // Print PMT
    cout << "--TABLA DE MAPA DE PAGINAS--" << endl;
    cout << setw(8) << "Pagina" << setw(8) << "Marco" << setw(10) << "Estado"
         << setw(8) << "Refer." << setw(10) << "Modifi." << endl;

    // Find maximum page number in current memory
    int maxPage = -1;
    int i;
    for (i = 0; i < MAX_FRAMES_PER_TASK; i++) {
        if (!memory[i].isEmpty && memory[i].page > maxPage) {
            maxPage = memory[i].page;
        }
    }
    maxPage = (maxPage > 2) ? maxPage : 2; // Ensure we show at least 3 pages

    // For each page, check its current state in memory
    for (i = 0; i <= maxPage; i++) {
        int frameNum = -1;
        bool isPresent = false;

        // Find if this page is currently in memory and get its real frame number
        int j;
        for (j = 0; j < MAX_FRAMES_PER_TASK; j++) {
            if (!memory[j].isEmpty && memory[j].page == i) {
                // Get the real frame number from MMT
                MMTNode* temp = mmt;
                int count = 0;
                while (temp != NULL && count < j) {
                    temp = temp->next;
                    count++;
                }
                if (temp != NULL) {
                    frameNum = temp->frameNumber;
                    isPresent = true;
                }
                break;
            }
        }
    for (int i = 0; i <= 5; i++)
    {
        bool isPresent = false;
        int frameNum = -1;

        // ... (check if page is in memory)

        cout << setw(8) << i;

        // Correctly handle the frame number output:
        if (isPresent)
        {
            cout << setw(8) << frameNum;
        }
        else
        {
            cout << setw(8) << "-";
        }

        cout << setw(10) << (isPresent ? "1" : "0")
             << setw(8) << (isPresent ? "1" : "0")
             << setw(10) << "0" << endl;
    }
    cout << endl;
}

void printListaAux(queue<int, deque<int>> fifoQueue){
    queue<int, deque<int>> queueCopy = fifoQueue; // Create a copy of the queue for printing
    while (!queueCopy.empty())
    {
        cout << "P" << queueCopy.front() << " ";
        queueCopy.pop();
    }
}

string toString(int value)
{
    char buffer[20];
    sprintf(buffer, "%d", value);
    return string(buffer);
}
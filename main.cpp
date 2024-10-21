#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
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

struct Frame {
    int page;
    bool isEmpty;
    Frame() { page = -1; isEmpty = true; }
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
void fifoPageReplacement(const vector<int>& pageSequence);
void printMemoryState(const vector<Frame>& memory);


int main()
{
    srand(static_cast<unsigned int>(time(0)));

    initializeMMT();
    printMMT();

    for (int i = 1; i <= NUM_TASKS; ++i)
    {
        initializeTask(i, MAX_FRAMES_PER_TASK);
    }

    printJT();
    printAllPMT();
    cout << "\nDemonstrating FIFO Page Replacement for Task J1:" << endl;
    TaskNode *task1 = findTask(1);
    if (task1 != NULL) {
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
            return frame->location;
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

void fifoPageReplacement(const vector<int>& pageSequence) {
    vector<Frame> memory(MAX_FRAMES_PER_TASK);
    queue<int, deque<int> > fifoQueue;
    int pageFaults = 0;
    cout << "FIFO Remplazo de pagina" << endl;
    cout << "-------------------------------" << endl;
    for (int i = 0; i < pageSequence.size(); ++i) {
        int currentPage = pageSequence[i];
        bool pageFault = true;
        cout << "Tiempo " << i << ": Procesando P" << currentPage << endl;
        
        for (int j = 0; j < MAX_FRAMES_PER_TASK; ++j) {
            if (!memory[j].isEmpty && memory[j].page == currentPage) {
                pageFault = false;
                break;
            }
        }
        if (pageFault) {
            pageFaults++;
            cout << "Fallo de pagina (*)." << endl;
            
            int replaceIndex = -1;
            for (int j = 0; j < MAX_FRAMES_PER_TASK; ++j) {
                if (memory[j].isEmpty) {
                    replaceIndex = j;
                    break;
                }
            }
            if (replaceIndex == -1) {
                
                int oldestPage = fifoQueue.front();
                fifoQueue.pop();
                for (int j = 0; j < MAX_FRAMES_PER_TASK; ++j) {
                    if (memory[j].page == oldestPage) {
                        replaceIndex = j;
                        break;
                    }
                }
            }
            
            memory[replaceIndex].page = currentPage;
            memory[replaceIndex].isEmpty = false;
            fifoQueue.push(currentPage);
        } else {
            cout << "Pagina previamente cargada en memoria." << endl;
        }
        printMemoryState(memory);
        cout << endl;
    }
    cout << "Total de errores de pagina: " << pageFaults << endl;
}

void printMemoryState(const vector<Frame>& memory) {
    cout << "Estado de Memoria: ";
    for (int i = 0; i < MAX_FRAMES_PER_TASK; ++i) {
        if (memory[i].isEmpty) {
            cout << "[ ] ";
        } else {
            cout << "[P" << memory[i].page << "] ";
        }
    }
    cout << endl;
}

string toString(int value)
{
    char buffer[20];
    sprintf(buffer, "%d", value);
    return string(buffer);
}
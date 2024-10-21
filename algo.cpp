#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>

using namespace std;

const int MAX_FRAMES = 3; 
const int SEQUENCE_LENGTH = 12; 

struct Frame {
    int page;
    bool isEmpty;
    Frame() : page(-1), isEmpty(true) {}
};

void printMemoryState(const vector<Frame>& memory) {
    cout << "Memory state: ";
    for (const Frame& frame : memory) {
        if (frame.isEmpty) {
            cout << "[ ] ";
        } else {
            cout << "[P" << frame.page << "] ";
        }
    }
    cout << endl;
}

void fifoPageReplacement(const vector<int>& pageSequence) {
    vector<Frame> memory(MAX_FRAMES);
    queue<int> fifoQueue;
    int pageFaults = 0;

    cout << "FIFO Remplazo de pagina" << endl;
    cout << "-------------------------------" << endl;

    for (int i = 0; i < SEQUENCE_LENGTH; ++i) {
        int currentPage = pageSequence[i];
        bool pageFault = true;

        cout << "Tiempo " << i << ": Procesando P" << currentPage << endl;

       
        for (const Frame& frame : memory) {
            if (!frame.isEmpty && frame.page == currentPage) {
                pageFault = false;
                break;
            }
        }

        if (pageFault) {
            pageFaults++;
            cout << "Fallo de pagina (*)." << endl;

            int replaceIndex = -1;
            for (int j = 0; j < MAX_FRAMES; ++j) {
                if (memory[j].isEmpty) {
                    replaceIndex = j;
                    break;
                }
            }

            if (replaceIndex == -1) {
                
                int oldestPage = fifoQueue.front();
                fifoQueue.pop();
                for (int j = 0; j < MAX_FRAMES; ++j) {
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

int main() {
    vector<int> pageSequence = {0, 3, 0, 1, 5, 5, 5, 5, 3, 3, 3, 3};
    fifoPageReplacement(pageSequence);
    return 0;
}
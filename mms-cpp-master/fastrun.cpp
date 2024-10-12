#include <iostream>
#include <string>
#include "API.h"
#include <queue>

const int ROWS = 16;
const int COLUMNS = 16;

const int MAX_PATH_SIZE = 256;  // Set a constant size for the array

int pointer = 0;
int qptr = 0;
static char fwd_path[MAX_PATH_SIZE];  // Fixed-size array

void appendToForwdPath(std::queue<char> &pathQueue) {
    while (!pathQueue.empty() && qptr < MAX_PATH_SIZE) {
        fwd_path[qptr] = pathQueue.front();  // Append from queue to array
        pathQueue.pop();  // Remove the front element of the queue
        qptr++;  // Increment the pointer
    }
}


int main(){

    std::queue<char> pathQueue;

    // Example of adding directions to the queue
    pathQueue.push('F');
    pathQueue.push('F');
    pathQueue.push('R');
    pathQueue.push('F');
    pathQueue.push('L');
    pathQueue.push('F');

    // Append the queue content to the array
    appendToForwdPath(pathQueue);

    while (pointer < 4)
    {
        if (fwd_path[pointer] == 'F') {
        API::moveForward();
        pointer++;
    } else if (fwd_path[pointer] == 'L') {
        API::turnLeft();
        pointer++;
    } else if (fwd_path[pointer] == 'R') {
        API::turnRight();
        pointer++;
    } else if (fwd_path[pointer] == 'B') {
        API::turnLeft();
        API::turnLeft();
        pointer++;
    } 
    }
    return 0;
}
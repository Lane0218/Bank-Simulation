#ifndef PRIORITYQUEUE_CLASS
#define PRIORITYQUEUE_CLASS

#include <iostream>
#include <stdlib.h>
using namespace std;

// maximum size of the priority queue array
const int MaxPQSize = 50;

class PQueue
{
private:
    // priority queue array and count
    int count;
    DataType pqlist[MaxPQSize];

public:
    // constructor
    PQueue(void);

    // priority queue modification operations
    void PQInsert(const DataType &item);
    DataType PQDelete(void);
    void ClearPQ(void);

    // priority queue test methods
    int PQEmpty(void) const;
    int PQFull(void) const;
    int PQLength(void) const;
};

// initialize priority queue count
PQueue::PQueue(void) : count(0)
{
}

// insert item into the priority queue
void PQueue::PQInsert(const DataType &item)
{
    // if all elements of pqlist are used, terminate the program
    if (count == MaxPQSize)
    {
        cerr << "Priority queue overflow!" << endl;
        exit(1);
    }
    // place item at the rear of the list and increment count
    pqlist[count] = item;
    count++;
}

// delete an element from the priority queue and return its value
DataType PQueue::PQDelete(void)
{
    DataType min;
    int i, minindex = 0;

    if (count > 0)
    {
        // find the minimum value and its index in pqlist
        min = pqlist[0]; // assume pqlist[0] is the minimum

        // visit remaining elments, updating minimum and index
        for (i = 1; i < count; i++)
            if (pqlist[i] < min)
            {
                // new minimum is pqlist[i]. new minindex is i
                min = pqlist[i];
                minindex = i;
            }
        // move rear element to minindex and decrement count
        pqlist[minindex] = pqlist[count - 1];
        count--;
    }
    // qlist is empty, terminate the program
    else
    {
        cerr << "Deleting from an empty priority queue!" << endl;
        exit(1);
    }
    // return minimum value
    return min;
}

// return number of list elements
int PQueue::PQLength(void) const
{
    return count;
}

// test for an empty priority queue
int PQueue::PQEmpty(void) const
{
    return count == 0;
}

// test for a full priority queue
int PQueue::PQFull(void) const
{
    return count == MaxPQSize;
}

// clear the priority queue by resetting count to 0
void PQueue::ClearPQ(void)
{
    count = 0;
}

#endif // PRIORITYQUEUE_CLASS

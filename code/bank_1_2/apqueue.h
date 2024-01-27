#ifndef PRIORITYQUEUE_CLASS
#define PRIORITYQUEUE_CLASS

#include <iostream>
#include <stdlib.h>
using namespace std;

// maximum size of the priority queue array
const int MaxPQSize = 50;

// 判断该普通顾客能否被插队
int judge(DataType e, int tellerID, int arrive);

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

    // 修改被插队的普通顾客departure的信息
    void Change_Common_Departure(int tellerID, int arrive, int wait);
    // 计算通过插队，VIP顾客等待时间可以减少多少
    int VIP_WaitTime_Reduce(int tellerID, int arrive);
    // 计算VIP顾客插了多少个普通顾客的队
    int Jump_Cnt(int tellerID, int arrive);
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

void PQueue::Change_Common_Departure(int tellerID, int arrive, int wait)
{
    for (int i = 0; i < count; i++)
    {
        if (pqlist[i].GetTellerID() == tellerID && pqlist[i].GetEventType() == departure)
        {
            if (judge(pqlist[i], tellerID, arrive))
            {
                pqlist[i].SetTime(wait);
                pqlist[i].SetWaitTime(wait);
            }
        }
    }
}

int PQueue::VIP_WaitTime_Reduce(int tellerID, int arrive)
{
    int minus = 0;
    for (int i = 0; i < count; i++)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID)
        {
            if (judge(pqlist[i], tellerID, arrive))
            {
                minus += pqlist[i].GetServiceTime();
            }
        }
    }
    return minus;
}

int PQueue::Jump_Cnt(int tellerID, int arrive)
{
    int cnt = 0;
    for (int i = 0; i < count; i++)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID)
        {
            if (judge(pqlist[i], tellerID, arrive))
            {
                cnt++;
            }
        }
    }
    return cnt;
}

// 重点
// 对于某普通窗口的普通顾客，如果在VIP顾客到来时还没有开始服务；并且在VIP到来前已经到来，那么该普通顾客可以被VIP顾客插队
int judge(DataType e, int tellerID, int arrive)
{
    if (!(e.GetVip()) && e.GetTime() - e.GetServiceTime() > arrive && e.GetTime() - e.GetServiceTime() - e.GetWaitTime() < arrive)
        return 1;
    return 0;
}

#endif // PRIORITYQUEUE_CLASS

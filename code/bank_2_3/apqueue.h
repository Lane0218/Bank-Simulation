#ifndef PRIORITYQUEUE_CLASS
#define PRIORITYQUEUE_CLASS

#include <iostream>
#include <stdlib.h>
#include <algorithm>
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

    // VIP最多能插队到的位置：最后一个VIP之后，已在办理业务的顾客之后（取二者靠后的）
    // 返回最多能插队到的位置对应pqlist的下标
    int Max_Jump_Index(int tellerID);

    // 返回在该窗口要使得VIP等待时间不超过waitHigh需要插队的次数
    // 如果为-1，表示无法使等待时间降低到waitHigh
    int Jump_count(int waitHigh, int tellerID, int cur_finishService, int cur_time);

    // 更新被插队的人的departure信息，返回这个VIP的等待时间
    void Jump(int waitHigh, int tellerID, int vip_time, int minJumpCnt, int &cur_finishService);
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
    sort(pqlist, pqlist + count, cmp_greater_time);
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

int PQueue::Max_Jump_Index(int tellerID)
{
    // 从后往前找最后一个vip在pqlist的下标
    for (int i = count - 1; i >= 0; i--)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID && pqlist[i].GetVip())
            return i;
    }
    // 如果没有找到vip，那么不能插队到已经在服务的普通客户之前
    for (int i = 0; i < count; i++)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID)
            return i;
    }
    return 0;
}

int PQueue::Jump_count(int waitHigh, int tellerID, int cur_finishService, int cur_time)
{
    int tmp_wait_time = cur_finishService - cur_time;
    int max_jump_index = Max_Jump_Index(tellerID);
    int jump_cnt = 0;

    for (int i = count - 1; i > max_jump_index; i--)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID && !pqlist[i].GetVip())
        {
            tmp_wait_time -= pqlist[i].GetServiceTime();
            jump_cnt++;
            if (tmp_wait_time <= waitHigh)
                break;
        }
    }
    if (tmp_wait_time > waitHigh)
        return -1;
    return jump_cnt;
}

void PQueue::Jump(int waitHigh, int tellerID, int vip_time, int minJumpCnt, int &cur_finishService)
{
    int cnt = 0;
    for (int i = count - 1; i >= 0; i--)
    {
        if (pqlist[i].GetEventType() == departure && pqlist[i].GetTellerID() == tellerID && !pqlist[i].GetVip())
        {
            cnt++;
            // 更新VIP的等待时间
            cur_finishService -= pqlist[i].GetServiceTime();
            // 更新被插队的人的departure信息
            pqlist[i].SetTime(vip_time);
            pqlist[i].SetWaitTime(vip_time);

            if (cnt == minJumpCnt)
                return;
        }
    }
}

#endif // PRIORITYQUEUE_CLASS

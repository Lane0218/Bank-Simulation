#ifndef SIMULATION_CLASS
#define SIMULATION_CLASS

#include <iostream>
#include <iomanip>
using namespace std;
#pragma hdrstop
#define TIMES 2

#include "random.h" // include random number generator
#include <time.h>

int vipCustomers = 0, vipWait = 0;
int totalJumpCnt = 0;
// VIP顾客所占比例，默认为30%
const int VIP_RATIO = 30;

// specifies the two kinds of events
enum EventType
{
    arrival,
    departure
};

class Event
{
private:
    // members that identify both customer and teller, as
    // well as maintain information on time of the event,
    // the event type, the length of service required by
    // the customer, and the amount of time customer is
    // forced to wait for service
    int time;
    EventType etype;
    int customerID; // customers numbered 1, 2, 3,...
    int tellerID;   // tellers numbered 1, 2, 3,...
    int waittime;
    int servicetime;
    int vip;
    int jump; // 对于VIP，是否插队，主要用于优先级队列的排序
public:
    // constructors
    Event(void);
    Event(int t, EventType et, int v, int cn, int tn,
          int wt, int st, int j);

    // methods to retrieve private data
    int GetTime(void) const;
    EventType GetEventType(void) const;
    int GetCustomerID(void) const;
    int GetTellerID(void) const;
    int GetWaitTime(void) const;
    int GetServiceTime(void) const;
    int GetVip(void) const;

    int GetJump(void) const;

    // 修改被插队的普通顾客的departure信息：时间和等待时间
    void SetTime(int ntime);
    void SetWaitTime(int ntime);
};

// default constructor - data filled by assignment later
Event::Event(void)
{
}

// constructor that initializes all data members of the event
Event::Event(int t, EventType et, int v, int cn, int tn,
             int wt, int st, int j) : time(t), etype(et), vip(v), customerID(cn), tellerID(tn),
                                      waittime(wt), servicetime(st), jump(j)
{
}

// return the time the event occurs
int Event::GetTime(void) const
{
    return time;
}

// return the type of event (arrival, departure)
EventType Event::GetEventType(void) const
{
    return etype;
}

// return the customer number
int Event::GetCustomerID(void) const
{
    return customerID;
}

// return the teller number
int Event::GetTellerID(void) const
{
    return tellerID;
}

// return the time the customer waits for service
int Event::GetWaitTime(void) const
{
    return waittime;
}

// return the amount of teller time needed by customer
int Event::GetServiceTime(void) const
{
    return servicetime;
}

int Event::GetVip(void) const
{
    return vip;
}

int Event::GetJump(void) const
{
    return jump;
}

void Event::SetTime(int ntime)
{
    time += ntime;
}

void Event::SetWaitTime(int ntime)
{
    waittime += ntime;
}

// 用于优先级队列的比较
// 原则：e1与e2不同时发生，发生得早的在前；e1 与 e2 同时发生时，优先级为离开>到达且插队>到达不插队
int operator<(Event e1, Event e2)
{
    if (e1.GetTime() == e2.GetTime())
    {
        if (e1.GetEventType() == departure)
            return 1;
        if (e2.GetEventType() == departure)
            return 0;
        if (e1.GetJump())
            return 1;
        if (e2.GetJump())
            return 0;
    }
    return e1.GetTime() < e2.GetTime();
}

// 用于对优先级队列排序，原则同上
// 理论上完成排序后可以直接从前往后出队，不需要重载运算符；此处为简便起见，不再修改框架代码中的出队函数
bool cmp_greater_time(Event e1, Event e2)
{
    if (e1.GetTime() == e2.GetTime())
    {
        if (e1.GetEventType() == departure)
            return true;
        if (e2.GetEventType() == departure)
            return false;
        if (e1.GetJump())
            return true;
        if (e2.GetJump())
            return false;
    }
    return e1.GetTime() < e2.GetTime();
}

typedef Event DataType; // elements are Event objects

#include "apqueue.h"

// Structure for Teller Info
struct TellerStats
{
    int finishService;      // when teller available
    int totalCustomerCount; // total of customers serviced
    int totalCustomerWait;  // total customer waiting time
    int totalService;       // total time servicing customers
};

class Simulation
{
private:
    // data used to run the simulation
    int simulationLength;        // simulation length
    int numTellers_vip;          // number of vip tellers
    int numTellers_common;       // number of common tellers
    int nextCustomer;            // next customer ID
    int arrivalLow, arrivalHigh; // next arrival range
    int serviceLow, serviceHigh; // service range
    int waitHigh;                // VIP 用户最多等待 waitHigh 时间
    TellerStats tstat[11];       // max 10 tellers
    PQueue pq;                   // priority queue
    RandomNumber rnd;            // use for arrival
                                 // and service times
    int nextVip;

    // private methods used by RunSimulation
    int NextArrivalTime(void);
    int GetServiceTime(void);
    // 普通顾客选择窗口
    int NextAvailableTeller(void);
    // 重难点：VIP顾客选择窗口
    int NextAvailableTeller_vip(int arrive_time, int &isJump, int &minJumpCnt);

public:
    // constructor
    Simulation(void);

    void RunSimulation(void);          // execute study
    void PrintSimulationResults(void); // print stats
};

Simulation::Simulation(void)
{
    int i;
    Event firstevent;

    // 初始化窗口的参数
    for (i = 1; i <= 10; i++)
    {
        tstat[i].finishService = 0;
        tstat[i].totalService = 0;
        tstat[i].totalCustomerWait = 0;
        tstat[i].totalCustomerCount = 0;
    }

    cout << "Enter the simulation time in minutes: ";
    cin >> simulationLength;
    cout << "Enter the number of bank tellers for VIP: ";
    cin >> numTellers_vip;
    cout << "Enter the number of bank tellers for common: ";
    cin >> numTellers_common;
    cout << "Enter the range of arrival times in minutes: ";
    cin >> arrivalLow >> arrivalHigh;
    cout << "Enter the range of service times in minutes: ";
    cin >> serviceLow >> serviceHigh;
    cout << "Enter the longest waitting time the VIP can tolerate in minutes: ";
    cin >> waitHigh;
    cout << "\n********* Simulation Begin *********" << endl;

    nextCustomer = 1;

    // 随机生成下一个顾客是否为VIP（默认VIP占比为30%）
    int nextVip = (rand() % 100 - 1) > VIP_RATIO ? 0 : 1;

    // 生成第一个到达事件
    pq.PQInsert(Event(0, arrival, nextVip, 1, 0, 0, 0, 0));
}

// determine random time of next arrival
int Simulation::NextArrivalTime(void)
{
    return arrivalLow + rnd.Random(arrivalHigh - arrivalLow + 1);
}

// determine random time for customer service
int Simulation::GetServiceTime(void)
{
    return serviceLow + rnd.Random(serviceHigh - serviceLow + 1);
}

// 这个函数是解决本问题的关键
/*
    先找在不插队的情况下，即排在任意窗口（含vip窗口）的最后需要多少等待时间
    - 如果上述等待时间没有超过waitHigh，那就排这个窗口，不插队
    - 如果上述等待时间超过了
        - 遍历所有普通窗口，依次向前插队（不能超过最后一个vip，已经开始办业务的顾客也不能被插队）
        - 一直到等待时间低于最长时间或到了不能插队的时候停止，统计插了多少次队，取插队次数最少的（因为被插队的普通顾客越多，普通顾客越不满意）
        - 但如果在任何普通窗口插队都不能使等待时间降低到waitHigh以内，不插队
*/
int Simulation::NextAvailableTeller_vip(int arrive_time, int &isJump, int &minJumpCnt)
{
    int minfinish = tstat[2].finishService;
    int minfinishindex = 2;

    // 在任意窗口（含vip窗口）
    for (int i = 1; i <= numTellers_common + numTellers_vip; i++)
    {
        if (tstat[i].finishService < minfinish)
        {
            minfinish = tstat[i].finishService;
            minfinishindex = i;
        }
    }

    // 如果在等待时间最短的窗口等待时间低于waitHigh，就不插队，直接选择该窗口
    if (minfinish - arrive_time <= waitHigh)
        return minfinishindex;

    // 每个窗口被插队的普通顾客的数量，如果为-1说明在该窗口插队不能使等待时间低于waitHigh
    int jump_cnt[11] = {0};
    // 先初始化最少插队次数为某个较大值，这里取10000
    minJumpCnt = 10000;
    // 遍历得到在每个普通窗口插队最少需要插多少个人，才能使等待时间低于waitHigh
    for (int i = numTellers_vip + 1; i <= numTellers_common + numTellers_vip; i++)
    {
        jump_cnt[i] = pq.Jump_count(waitHigh, i, tstat[i].finishService, arrive_time);
    }
    // 遍历列表，找到插队次数最少的那个窗口
    for (int i = numTellers_vip + 1; i <= numTellers_common + numTellers_vip; i++)
    {
        if (jump_cnt[i] == -1)
            continue;
        if (jump_cnt[i] < minJumpCnt)
        {
            minJumpCnt = jump_cnt[i];
            minfinishindex = i;
        }
    }
    // 如果最少插队次数被修改，说明至少有一个窗口能通过插队使等待时间降到waitHigh以内
    if (minJumpCnt != 10000)
        isJump = 1;
    // 否则说明在任意普通窗口插队都不能使等待时间降到waitHigh以内，不插队，直接选择等待时间最短的窗口
    return minfinishindex;
}

int Simulation::NextAvailableTeller(void)
{
    int minfinish = tstat[numTellers_vip + 1].finishService;
    int minfinishindex = numTellers_vip + 1;

    // 在普通窗口中选择等待时间最短的窗口
    for (int i = numTellers_vip + 1; i <= numTellers_vip + numTellers_common; i++)
    {
        if (tstat[i].finishService < minfinish)
        {
            minfinish = tstat[i].finishService;
            minfinishindex = i;
        }
    }
    return minfinishindex;
}

// implements the simulation
void Simulation::RunSimulation(void)
{
    Event e, newevent;
    int nexttime;
    int tellerID;
    int servicetime;
    int waittime;
    int nextVip;

    // run till priority queue is empty
    while (!pq.PQEmpty())
    {
        // get next event (time measures the priority)
        e = pq.PQDelete();

        // handle an arrival event
        if (e.GetEventType() == arrival)
        {
            // compute time for next arrival.
            nexttime = e.GetTime() + NextArrivalTime();

            if (e.GetTime() > simulationLength)
                // process events but don't generate any more
                continue;
            else
            {
                // generate arrival for next customer. put in queue
                nextCustomer++;
                // 随机生成下一个顾客是否为VIP（默认VIP占比为30%）
                nextVip = (rand() % 100 - 1) > VIP_RATIO ? 0 : 1;
                newevent = Event(nexttime, arrival, nextVip, nextCustomer, 0, 0, 0, 0);
                pq.PQInsert(newevent);
            }

            cout << "Time: " << setw(2) << e.GetTime()
                 << "\t"
                 << "arrival   customer\t"
                 << e.GetCustomerID();
            if (e.GetVip())
                cout << "\t(V)";
            else
                cout << "\t(P)";

            // generate departure event for current customer

            servicetime = GetServiceTime();
            int isJump = 0;
            int minJumpCnt;
            if (e.GetVip())
                tellerID = NextAvailableTeller_vip(e.GetTime(), isJump, minJumpCnt);
            else
                tellerID = NextAvailableTeller();
            cout << "\t"
                 << " ==> Teller:\t" << tellerID;

            // if teller free, update sign to current time
            if (tstat[tellerID].finishService == 0)
                tstat[tellerID].finishService = e.GetTime();

            // 如果vip插队了
            if (isJump)
            {
                cout << "\tJumpCount:\t" << minJumpCnt << endl;
                // 统计总共插了多少个普通顾客的队，第3题可以用来反映普通顾客的不满意程度
                totalJumpCnt += minJumpCnt;
                // 在不插队的情况下的finishService，后面会在jump函数中进行修改，修改后就是插队后的finishService
                int cur_finishService = tstat[tellerID].finishService;

                // 进行插队
                // 得到插队后的finishService，并修改被插队的普通用户的departure信息
                pq.Jump(waitHigh, tellerID, servicetime, minJumpCnt, cur_finishService);

                // 插队后的等待时间：插队后的finishService-到达时间
                waittime = cur_finishService - e.GetTime();

                // 插队后该窗口总等待时间：该VIP等待时间+该VIP服务时间*被该VIP插队的人数
                tstat[tellerID].totalCustomerWait += waittime + servicetime * minJumpCnt;

                // 该VIP的离开时间：到达时间+等待时间+服务时间
                newevent = Event(e.GetTime() + waittime + servicetime,
                                 departure, 1, e.GetCustomerID(), tellerID,
                                 waittime, servicetime, 1);
            }
            else
            {
                cout << endl;
                // 未插队的等待时间：finishService-到达时间
                waittime = tstat[tellerID].finishService -
                           e.GetTime();
                tstat[tellerID].totalCustomerWait += waittime;
                newevent = Event(e.GetTime() + waittime + servicetime,
                                 departure, e.GetVip(), e.GetCustomerID(), tellerID,
                                 waittime, servicetime, 0);
            }

            tstat[tellerID].totalCustomerCount++;
            tstat[tellerID].totalService += servicetime;
            tstat[tellerID].finishService += servicetime;

            pq.PQInsert(newevent);
        }
        // handle a departure event
        else
        {
            cout << "Time: " << setw(2) << e.GetTime()
                 << "\t"
                 << "departure customer\t"
                 << e.GetCustomerID();
            if (e.GetVip())
            {
                cout << "\t(V)";
                vipCustomers++;
                vipWait += e.GetWaitTime();
            }
            else
                cout << "\t(P)";

            cout << "\t <== Wait:\t" << e.GetWaitTime()
                 << "\tService:\t" << e.GetServiceTime()
                 << endl;

            tellerID = e.GetTellerID();
            // if nobody waiting for teller, mark teller free
            if (tellerID != 0 && e.GetTime() == tstat[tellerID].finishService)
                tstat[tellerID].finishService = 0;
        }
    }

    // adjust simulation to account for overtime by tellers
    simulationLength = (e.GetTime() <= simulationLength)
                           ? simulationLength
                           : e.GetTime();
}

// summarize the simulation results
void Simulation::PrintSimulationResults(void)
{
    int i;
    int cumCustomers = 0, cumWait = 0;

    int avgComWait, avgVipWait;
    float tellerWork;
    int tellerWorkPercent;

    for (i = 1; i <= numTellers_common + numTellers_vip; i++)
    {
        cumCustomers += tstat[i].totalCustomerCount;
        cumWait += tstat[i].totalCustomerWait;
    }

    cout << "\n******** Simulation Summary ********" << endl;
    cout << "Simulation of " << simulationLength
         << " minutes" << endl;
    cout << "No. of Common Customers:  " << cumCustomers - vipCustomers << endl;
    cout << "No. of Vip Customers:  " << vipCustomers << endl;
    cout << "No. of Customers:  " << cumCustomers << endl;

    cout << "Average Common Customer Wait: ";
    avgComWait = int(float(cumWait-vipWait) / (cumCustomers - vipCustomers) + 0.5);
    cout << avgComWait << " minutes" << endl;

    cout << "Average Vip Customer Wait: ";
    avgVipWait = (vipCustomers != 0) ? int(float(vipWait) / vipCustomers + 0.5) : 0;
    cout << avgVipWait << " minutes" << endl;

    // VIP一共插了多少个普通顾客的队
    cout << "Total Vip Jump: " << totalJumpCnt << endl;

    cout << "\n********* Tellers Busyness *********" << endl;
    // 所有窗口的总服务时间所占的比例
    int total_tellerWorkPercent=0;
    for (i = 1; i <= numTellers_common + numTellers_vip; i++)
    {
        cout << "Teller " << i << " Working: ";
        // display percent rounded to nearest integer value
        tellerWork = float(tstat[i].totalService) / simulationLength;
        tellerWorkPercent = int(tellerWork * 100.0 + 0.5);
        cout << tellerWorkPercent << "%" << endl;
        total_tellerWorkPercent+=tellerWorkPercent;
    }

    cout << "\n****** Overall Dissatisfaction *******" << endl;
    float bank_dis=float(100*(numTellers_common+numTellers_vip)-total_tellerWorkPercent)/(100*(numTellers_common+numTellers_vip));
    float common_dis_1=float(avgComWait-2*waitHigh)/(2*waitHigh)>0?float(avgComWait-2*waitHigh)/(2*waitHigh):0;
    float common_dis_2=float(totalJumpCnt/(cumCustomers - vipCustomers)-0.1)/0.1>0?float(totalJumpCnt/(cumCustomers - vipCustomers)-0.1)/0.1:0;
    float common_dis=0.5*common_dis_1+0.5*common_dis_2;
    float vip_dis=float(avgVipWait-waitHigh)/waitHigh>0?float(avgVipWait-waitHigh)/waitHigh:0;
    float overall_dis=0.5*bank_dis+0.2*common_dis+0.3*vip_dis;

    printf("Bank Dissatisfaction: %.2f\n",bank_dis);
    printf("Common Customer Dissatisfaction: %.2f\n",common_dis);
    printf("VIP Customer Dissatisfaction: %.2f\n",vip_dis);
    printf("Overall Dissatisfaction: %.2f\n",overall_dis);
}   

#endif // SIMULATION_CLASS
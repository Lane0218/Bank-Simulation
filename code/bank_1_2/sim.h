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

public:
    // constructors
    Event(void);
    Event(int t, EventType et, int v, int cn, int tn,
          int wt, int st);

    // methods to retrieve private data
    int GetTime(void) const;
    EventType GetEventType(void) const;
    int GetCustomerID(void) const;
    int GetTellerID(void) const;
    int GetWaitTime(void) const;
    int GetServiceTime(void) const;
    int GetVip(void) const;

    // 修改被VIP顾客插队的普通顾客departure信息
    void SetTime(int vip_service_time);
    void SetWaitTime(int vip_service_time);
};

// default constructor - data filled by assignment later
Event::Event(void)
{
}

// constructor that initializes all data members of the event
Event::Event(int t, EventType et, int v, int cn, int tn,
             int wt, int st) : time(t), etype(et), vip(v), customerID(cn), tellerID(tn),
                               waittime(wt), servicetime(st)
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

void Event::SetTime(int vip_service_time)
{
    time += vip_service_time;
}

void Event::SetWaitTime(int vip_service_time)
{
    waittime += vip_service_time;
}

// compare two Event objects using the time at
// which the events occur. needed for the priority queue
int operator<(Event e1, Event e2)
{
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
    // 有多少顾客放弃排队，选择直接离开
    int lose;
    TellerStats tstat[11]; // max 10 tellers
    PQueue pq;             // priority queue
    RandomNumber rnd;      // use for arrival
                           // and service times
    int nextVip;

    // private methods used by RunSimulation
    int NextArrivalTime(void);
    int GetServiceTime(void);
    // 普通顾客选择窗口
    int NextAvailableTeller(void);
    // VIP顾客选择窗口
    int NextAvailableTeller_vip(int arrive_time);

public:
    // constructor
    Simulation(void);

    void RunSimulation(void);          // execute study
    void PrintSimulationResults(void); // print stats
};

// constructor initializes simulation data and prompts client
// for simulation parameters
Simulation::Simulation(void)
{
    int i;
    Event firstevent;

    // Initialize Teller Information Parameters
    for (i = 1; i <= 10; i++)
    {
        tstat[i].finishService = 0;
        tstat[i].totalService = 0;
        tstat[i].totalCustomerWait = 0;
        tstat[i].totalCustomerCount = 0;
    }
    nextCustomer = 1;
    // 随机生成下一个顾客是否为VIP（默认VIP占比为30%）
    int nextVip = (rand() % 100 - 1) > VIP_RATIO ? 0 : 1;

    // reads client input for the study
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
    cout << "\n********* Simulation Begin *********" << endl;

    // generate first arrival event
    // teller#/waittime/servicetime not used for arrival
    pq.PQInsert(Event(0, arrival, nextVip, 1, 0, 0, 0));
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

// return first available teller
int Simulation::NextAvailableTeller_vip(int arrive_time)
{
    int minfinish = tstat[1].finishService;
    int minfinishindex = 1;

    // 在vip窗口排队用时
    for (int i = 1; i <= numTellers_vip; i++)
    {
        if (tstat[i].finishService < minfinish)
        {
            minfinish = tstat[i].finishService;
            minfinishindex = i;
        }
    }

    // 在普通窗口插队
    for (int i = numTellers_vip + 1; i <= numTellers_common + numTellers_vip; i++)
    {
        int tmp_i_finishService = tstat[i].finishService - pq.VIP_WaitTime_Reduce(i, arrive_time);
        if (tmp_i_finishService < minfinish)
        {
            minfinishindex = i;
            minfinish = tmp_i_finishService;
        }
    }

    return minfinishindex;
}

// return first available teller
int Simulation::NextAvailableTeller(void)
{
    int minfinish = tstat[numTellers_vip + 1].finishService;
    int minfinishindex = numTellers_vip + 1;

    // find teller who is free first
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
                newevent = Event(nexttime, arrival, nextVip, nextCustomer, 0, 0, 0);
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

            // time the customer takes
            servicetime = GetServiceTime();
            // teller who services customer
            if (e.GetVip())
                tellerID = NextAvailableTeller_vip(e.GetTime());
            else
                tellerID = NextAvailableTeller();
            cout << "\t"
                 << " ==> Teller:\t" << tellerID << endl;

            // if teller free, update sign to current time
            if (tstat[tellerID].finishService == 0)
                tstat[tellerID].finishService = e.GetTime();

            // 如果VIP选择去普通窗口插队
            if (e.GetVip() && tellerID >= numTellers_vip + 1)
            {
                tstat[tellerID].finishService -= pq.VIP_WaitTime_Reduce(tellerID, e.GetTime());
                pq.Change_Common_Departure(tellerID, e.GetTime(), servicetime);
            }
            waittime = tstat[tellerID].finishService -
                       e.GetTime();

            // update teller statistics
            // 如果VIP选择去普通窗口插队
            if (e.GetVip() && tellerID >= numTellers_vip + 1)
                tstat[tellerID].totalCustomerWait += (waittime + servicetime * pq.Jump_Cnt(tellerID, e.GetTime()));
            else
                tstat[tellerID].totalCustomerWait += waittime;

            tstat[tellerID].totalCustomerCount++;
            tstat[tellerID].totalService += servicetime;
            tstat[tellerID].finishService += servicetime;

            // 因等待时间大于服务时间上限的TIMES倍，顾客离开
            // 但已经排队因被插队而等待时间超过的不会离开
            if (waittime > serviceHigh * TIMES)
            {
                if (e.GetVip() && tellerID >= numTellers_vip + 1)
                    tstat[tellerID].totalCustomerWait -= (waittime + servicetime * pq.Jump_Cnt(tellerID, e.GetTime()));
                else
                    tstat[tellerID].totalCustomerWait -= waittime;
                tstat[tellerID].totalCustomerCount--;
                tstat[tellerID].totalService -= servicetime;
                tstat[tellerID].finishService -= servicetime;
                newevent = Event(e.GetTime(),
                                 departure, e.GetVip(), e.GetCustomerID(), -1,
                                 0, 0);
                if (e.GetVip() && tellerID >= numTellers_vip + 1)
                {
                    pq.Change_Common_Departure(tellerID, e.GetTime(), servicetime * (-1));
                    tstat[tellerID].finishService += pq.VIP_WaitTime_Reduce(tellerID, e.GetTime());
                }
                lose++;
                if (e.GetVip())
                    vipCustomers--;
            }
            else
            {
                if (e.GetVip() && tellerID >= numTellers_vip + 1)
                {
                    newevent = Event(tstat[tellerID].finishService,
                                     departure, e.GetVip(), e.GetCustomerID(), tellerID,
                                     waittime, servicetime);
                    tstat[tellerID].finishService += pq.VIP_WaitTime_Reduce(tellerID, e.GetTime());
                }
                else
                    newevent = Event(tstat[tellerID].finishService,
                                     departure, e.GetVip(), e.GetCustomerID(), tellerID,
                                     waittime, servicetime);
            }
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
            // lose
            if (e.GetTellerID() == -1)
                cout << "\t     The customer gave up and left!" << endl;
            else
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
    avgComWait = int(float(cumWait - vipWait) / (cumCustomers - vipCustomers) + 0.5);
    cout << avgComWait << " minutes" << endl;

    cout << "Average Vip Customer Wait: ";
    avgVipWait = (vipCustomers != 0) ? int(float(vipWait) / vipCustomers + 0.5) : 0;
    cout << avgVipWait << " minutes" << endl;

    cout << "\n********* Tellers Busyness *********" << endl;
    for (i = 1; i <= numTellers_common + numTellers_vip; i++)
    {
        cout << "Teller " << i << " Working: ";
        // display percent rounded to nearest integer value
        tellerWork = float(tstat[i].totalService) / simulationLength;
        tellerWorkPercent = int(tellerWork * 100.0 + 0.5);
        cout << tellerWorkPercent << "%" << endl;
    }
}

#endif // SIMULATION_CLASS
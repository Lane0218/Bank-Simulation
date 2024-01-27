#ifndef SIMULATION_CLASS
#define SIMULATION_CLASS

#include <iostream>
#include <iomanip>
using namespace std;
#pragma hdrstop

#include "random.h" // include random number generator

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

public:
    // constructors
    Event(void);
    Event(int t, EventType et, int cn, int tn,
          int wt, int st);

    // methods to retrieve private data
    int GetTime(void) const;
    EventType GetEventType(void) const;
    int GetCustomerID(void) const;
    int GetTellerID(void) const;
    int GetWaitTime(void) const;
    int GetServiceTime(void) const;
};

// default constructor - data filled by assignment later
Event::Event(void)
{
}

// constructor that initializes all data members of the event
Event::Event(int t, EventType et, int cn, int tn,
             int wt, int st) : time(t), etype(et), customerID(cn), tellerID(tn),
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
    int numTellers;              // number of tellers
    int nextCustomer;            // next customer ID
    int arrivalLow, arrivalHigh; // next arrival range
    int serviceLow, serviceHigh; // service range
    TellerStats tstat[11];       // max 10 tellers
    PQueue pq;                   // priority queue
    RandomNumber rnd;            // use for arrival
                                 // and service times

    // private methods used by RunSimulation
    int NextArrivalTime(void);
    int GetServiceTime(void);
    int NextAvailableTeller(void);

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

    // 默认使用这组数据进行验证
    // simulationLength=10;
    // numTellers=2;
    // arrivalLow =4;
    // arrivalHigh =4;
    // serviceLow=9;
    // serviceHigh=9;

    // reads client input for the study
    cout << "Enter the simulation time in minutes: ";
    cin >> simulationLength;
    cout << "Enter the number of bank tellers: ";
    cin >> numTellers;
    cout << "Enter the range of arrival times in minutes: ";
    cin >> arrivalLow >> arrivalHigh;
    cout << "Enter the range of service times in minutes: ";
    cin >> serviceLow >> serviceHigh;

    // generate first arrival event
    // teller#/waittime/servicetime not used for arrival
    pq.PQInsert(Event(0, arrival, 1, 0, 0, 0));
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
int Simulation::NextAvailableTeller(void)
{
    // initially assume all tellers finish at closing time
    int minfinish = simulationLength;

    // assign random teller to customer who arrives
    // before closing but obtains service after closing
    int minfinishindex = rnd.Random(numTellers) + 1;

    // find teller who is free first
    for (int i = 1; i <= numTellers; i++)
        if (tstat[i].finishService < minfinish)
        {
            minfinish = tstat[i].finishService;
            minfinishindex = i;
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
                newevent = Event(nexttime, arrival,
                                 nextCustomer, 0, 0, 0);
                pq.PQInsert(newevent);
            }

            cout << "Time: " << setw(2) << e.GetTime()
                 << "  "
                 << "arrival of customer "
                 << e.GetCustomerID() << endl;

            // generate departure event for current customer

            // time the customer takes
            servicetime = GetServiceTime();
            // teller who services customer
            tellerID = NextAvailableTeller();

            // if teller free, update sign to current time
            if (tstat[tellerID].finishService == 0)
                tstat[tellerID].finishService = e.GetTime();

            // compute time customer waits by subtracting the
            // current time from time on the teller's sign
            waittime = tstat[tellerID].finishService -
                       e.GetTime();

            // update teller statistics
            tstat[tellerID].totalCustomerWait += waittime;
            tstat[tellerID].totalCustomerCount++;
            tstat[tellerID].totalService += servicetime;
            tstat[tellerID].finishService += servicetime;

            // create a departure object and put in the queue
            newevent = Event(tstat[tellerID].finishService,
                             departure, e.GetCustomerID(), tellerID,
                             waittime, servicetime);
            pq.PQInsert(newevent);
        }
        // handle a departure event
        else
        {
            cout << "Time: " << setw(2) << e.GetTime()
                 << "  "
                 << "departure of customer "
                 << e.GetCustomerID() << endl;
            cout << "       Teller " << e.GetTellerID()
                 << "  Wait " << e.GetWaitTime()
                 << "  Service " << e.GetServiceTime()
                 << endl;
            tellerID = e.GetTellerID();
            // if nobody waiting for teller, mark teller free
            if (e.GetTime() == tstat[tellerID].finishService)
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
    int cumCustomers = 0, cumWait = 0, i;
    int avgCustWait, tellerWorkPercent;
    float tellerWork;

    for (i = 1; i <= numTellers; i++)
    {
        cumCustomers += tstat[i].totalCustomerCount;
        cumWait += tstat[i].totalCustomerWait;
    }

    cout << endl;
    cout << "******** Simulation Summary ********" << endl;
    cout << "Simulation of " << simulationLength
         << " minutes" << endl;
    cout << "   No. of Customers:  " << cumCustomers << endl;
    cout << "   Average Customer Wait: ";

    avgCustWait = int(float(cumWait) / cumCustomers + 0.5);
    cout << avgCustWait << " minutes" << endl;
    for (i = 1; i <= numTellers; i++)
    {
        cout << "    Teller #" << i << "  % Working: ";
        // display percent rounded to nearest integer value
        tellerWork = float(tstat[i].totalService) / simulationLength;
        tellerWorkPercent = int(tellerWork * 100.0 + 0.5);
        cout << tellerWorkPercent << endl;
    }
}

#endif // SIMULATION_CLASS
#include "sim.h"
// #include "sim_vip.h"

int main()
{
    // 不设置VIP窗口
    cout << "******** NO VIP TELLERS ********" << endl;
    // declare an object S for our simulation
    Simulation S;

    // run the simulation
    S.RunSimulation();

    // print the results
    S.PrintSimulationResults();

    return 0;
}

/*
<Run #1 of Program 5.7>

Enter the simulation time in minutes: 30
Enter the number of bank tellers: 2
Enter the range of arrival times in minutes: 6 10
Enter the range of service times in minutes: 18 20
Time:  0  arrival of customer 1
Time:  7  arrival of customer 2
Time: 16  arrival of customer 3
Time: 19  departure of customer 1
       Teller 1  Wait 0  Service 19
Time: 25  departure of customer 2
       Teller 2  Wait 0  Service 18
Time: 37  departure of customer 3
       Teller 1  Wait 3  Service 18

******** Simulation Summary ********
Simulation of 37 minutes
    No. of Customers:   3
    Average Customer Wait: 1 minutes
    Teller #1  % Working: 100
    Teller #2  % Working: 49

<Run #2 of Program 5.7>

Enter the simulation time in minutes  480
Enter the number of bank tellers  4
Enter the range of arrival times in minutes  2 5
Enter the range of service times in minutes  6 20

<arrival and departure of 137 customers>

******** Simulation Summary ********
Simulation of 521 minutes
    No. of Customers:  137
    Average Customer Wait: 2 minutes
    Teller #1  % Working: 89
    Teller #2  % Working: 86
    Teller #3  % Working: 83
    Teller #4  % Working: 86
*/
#include "sim.h"

int main()
{
    // 设置1个VIP窗口
    cout << "******** ONE VIP TELLER ********" << endl;

    // declare an object S for our simulation
    Simulation S;

    // run the simulation
    S.RunSimulation();

    // print the results
    S.PrintSimulationResults();

    return 0;
}

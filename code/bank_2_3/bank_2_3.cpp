#include "sim.h"

int main(void)
{
    srand(time(NULL));
    // declare an object S for our simulation
    Simulation S;

    // run the simulation
    S.RunSimulation();

    // print the results
    S.PrintSimulationResults();

    return 1;
}
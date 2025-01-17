// NT4.cpp : Defines the entry point for the application.
//


using namespace std;

#include <algorithm> // Needed for std::sort
#include "MapSimTextServ.h"




int main(int argc, char** argv)
{
    c_MapSim_Text_Server MapSim;

    MapSim.run();

    return 1;
}

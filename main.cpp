#include "CitySystem.h"
#include <ctime>
#include <cstdlib>

int main() {
    // Seed the random number generator (used by CitySystem for event generation)
    srand(static_cast<unsigned int>(time(nullptr)));

    // Instantiate and run the city survival system
    CitySystem city;
    city.run();

    return 0;
}

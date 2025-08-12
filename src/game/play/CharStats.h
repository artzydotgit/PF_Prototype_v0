#include <algorithm>

struct CharStats
{
    int Strength = 5;
    int Speed = 5;
    int Defense = 5;
    int Dexterity = 5;
    static constexpr int MAX_POINTS_TOTAL = 22;
    
    void ClampStats();
};

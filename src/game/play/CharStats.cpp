#include "CharStats.h"

void CharStats::ClampStats()
{
    Strength  = std::clamp(Strength,  0, 10);
    Speed     = std::clamp(Speed,     0, 10);
    Defense   = std::clamp(Defense,   0, 10);
    Dexterity = std::clamp(Dexterity, 0, 10);
}
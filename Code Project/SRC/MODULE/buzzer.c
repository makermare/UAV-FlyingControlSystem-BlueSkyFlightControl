#include "buzzer.h"

void buzzerRing() {
    BuzzerPWMSet(1000);
}

void buzzerStop() {
    BuzzerPWMSet(0);
}

#include "fade_effect.h"


//void Fade(uint8_t pin, uint8_t start, uint8_t finish, int8_t inc) :
//    pin(pin), start(start), level(start), finish(finish) inc(inc) {
//    analogWrite(pin, level);
//}
Fade::Fade(uint8_t pin) :
    pin(pin)
{ }

void Fade::start (void) {
    level = 0;
    analogWrite(pin, level);
}

void Fade::step (void) {
    level += 1;
    analogWrite(pin, level);
}
bool Fade::is_done (void) {
    return (255 == level);
}


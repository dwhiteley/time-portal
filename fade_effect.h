#include <stdint.h>
#include "wiring_private.h"

class Fade {
private:
    uint8_t level;
    uint8_t pin;
    //uint8_t start;
    //uint8_t finish;
    //int8_t  inc;

public :
    //void Fade(uint8_t pin, uint8_t start, uint8_t finish, int8_t inc);
    //void settings(uint8_t start, uint8_t finish, int8_t inc);
    Fade(uint8_t pin);
    void start (void);
    void step (void);
    bool is_done (void);
    //void reverse (void);
};


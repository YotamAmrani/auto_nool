#ifndef DRAWINGS_H
#define DRAWINGS_H
#include "Settings.h"
#include <Arduino.h>
// #include <String.h>

class Drawing
{
private:
public:
    bool is_random_;
    const int16_t (*segments_)[N_INSTRUCTIONS];
    const int drawing_size_;
    const unsigned long drawing_speed_;
    Drawing(const int16_t segments[][N_INSTRUCTIONS], int drawing_size, const unsigned long drawing_speed,bool is_true = false);
    // void print_name();
};

#endif

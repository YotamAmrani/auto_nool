#include "Drawings.h"

Drawing::Drawing(const int16_t segments[][N_INSTRUCTIONS], int drawing_size, const unsigned long drawing_speed, bool is_true = false)
    : segments_(segments), drawing_size_(drawing_size), drawing_speed_(drawing_speed), is_random_(is_true)
{
    // drawing_name_ = String(drawing_name);
    // Serial.println("name");
    // Serial.println(drawing_name_.length());
}

#ifndef SETTINGS_H
#define SETTINGS_H

// Instructions indices
#define X_AXIS 0
#define Y_AXIS 1

#define N_AXIS 2
#define N_INSTRUCTIONS 3
#define bit_istrue(x, mask) ((x & mask) != 0)
#define bit_to_sign(x, mask) (((x & mask) != 0) ? -1 : 1)
#define DIRECTION_INVERT_MASK 3


// STEPERS CONFIGURATIONS
#define X_STEP_PIN 2
#define Y_STEP_PIN 3
#define X_DIR_PIN 5
#define Y_DIR_PIN 6
#define EN_PIN 8

// LIMIT SWITCHES
#define X_LIMIT_SW_PIN 9
#define Y_LIMIT_SW_PIN 10
#define BUTTON_PIN 11

// STEPERS CONFIGURATIONS
#define SOUND_SENSOR_PIN 12


// SYSTEM CONFIGURATIONS
#define STEP_PULSE_LENGTH 20
#define STEPS_RATE 700
#define X_STEPS_PER_MM (79)
#define Y_STEPS_PER_MM (53)



// SOFT LIMITS SETTINGS
#define ENABLE_SOFT_LIMIT 1 // uncomment to disable soft limits
#define AUTO_HOME_STEPS_RATE 700 //the bigger the slower

#define X_MM_RAIL_LENGTH  1060
#define Y_MM_RAIL_LENGTH 150

#define X_MM_HOMING_OFFSET (0)
#define Y_MM_HOMING_OFFSET (3)

// NOOL ELEMENTS
#define ELEMENTS_COUNT (263)
#define MAX_ELEMENTS_SEQ (3)
#define X_OFFSET_MM (2)
#define X_ELEMNT_SPACING_MM (4)
#define Y_CENTER_MM (75)
#define Y_RADIUS_MM (75)
#define PENDING_TIME_BETWEEN_ELEMENTS ((unsigned long)1000 * 500) //milli * seconds

// AUTO PRINTING
#define steps_to_mm(steps, ratio) (steps / ratio)
#define mm_to_steps(mm, ratio) ((unsigned long)mm * ratio)

template <typename T>
int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

enum State
{
  IDLE,
  PRINT,
  LISTEN
};

enum Mode
{
  TEST,
  TEST_NEG,
  CROSS,
  RANDOM,
  SOUND
};

struct sys_state
{
  State sys_mode;
  Mode movement_mode;
  long unsigned last_move_time_stamp;
};


#endif

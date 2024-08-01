#include "StepperController.h"

StepperController::StepperController() : step_pin_{X_STEP_PIN, Y_STEP_PIN},
                                         dir_pin_{X_DIR_PIN, Y_DIR_PIN},
                                         en_pin_(EN_PIN), steps_counter_{0, 0},
                                         max_steps_{mm_to_steps(X_MM_RAIL_LENGTH, X_STEPS_PER_MM), mm_to_steps(Y_MM_RAIL_LENGTH, Y_STEPS_PER_MM)},
                                         min_steps_{mm_to_steps((0), X_STEPS_PER_MM), mm_to_steps((0), Y_STEPS_PER_MM)}

{
  // Initializing values
  move_time_stamp_ = micros();
  steps_counter_[X_AXIS] = max_steps_[X_AXIS];
  steps_counter_[Y_AXIS] = max_steps_[Y_AXIS];
  steps_rate_ = STEPS_RATE;

  // Declare pins as Outputs
  pinMode(step_pin_[X_AXIS], OUTPUT);
  pinMode(dir_pin_[X_AXIS], OUTPUT);
  pinMode(step_pin_[Y_AXIS], OUTPUT);
  pinMode(dir_pin_[Y_AXIS], OUTPUT);
  pinMode(en_pin_, OUTPUT);
  this->set_enable(false);


}

/*    SETTERS    **/
void StepperController::set_steps_rate(unsigned long steps_rate)
{
  if (steps_rate >= 0)
  {
    steps_rate_ = steps_rate;
  }
}

void StepperController::set_enable(bool isEnabled)
{
  digitalWrite(EN_PIN, !isEnabled);
}

void StepperController::set_direction(int current_direction_mask)
{
  // set by the digit bits - if a bit is turned on, flip direction
  current_direction_mask = current_direction_mask ^ DIRECTION_INVERT_MASK;
  digitalWrite(dir_pin_[X_AXIS], bit_istrue(current_direction_mask, 1 << X_AXIS));
  digitalWrite(dir_pin_[Y_AXIS], bit_istrue(current_direction_mask, 1 << Y_AXIS));
}

void StepperController::set_steps_count(unsigned long x_steps,unsigned long y_steps)
{
  steps_counter_[X_AXIS] = x_steps;
  steps_counter_[Y_AXIS] = y_steps;
}


void StepperController::set_limits(int x_steps_max, int y_steps_max, int x_steps_min, int y_steps_min){
  max_steps_[X_AXIS] = mm_to_steps(x_steps_max, X_STEPS_PER_MM);
  max_steps_[Y_AXIS] = mm_to_steps(y_steps_max, Y_STEPS_PER_MM);
  min_steps_[X_AXIS] = mm_to_steps(x_steps_min, X_STEPS_PER_MM);
  min_steps_[Y_AXIS] = mm_to_steps(y_steps_min, Y_STEPS_PER_MM);
  }

/*    GETTERS    **/
const unsigned long *StepperController::get_steps_count() const
{
  return steps_counter_;
}

/*    MOVEMENT METHODS    **/
void StepperController::step(int current_step_mask, int current_direction_mask)
{
#ifdef ENABLE_SOFT_LIMIT
  for (int i = 0; i < N_AXIS; ++i)
  {
    int current_step = bit_to_sign(current_direction_mask, 1 << i) * bit_istrue(current_step_mask, 1 << i);

    if (current_step + steps_counter_[i] <= max_steps_[i] && current_step + steps_counter_[i] >= min_steps_[i])
    {
      digitalWrite(step_pin_[i], bit_istrue(current_step_mask, 1 << i));
      steps_counter_[i] += bit_to_sign(current_direction_mask, 1 << i) * bit_istrue(current_step_mask, 1 << i);
    }
    // if (steps_counter_[i] + 100 > max_steps_[i])
    // {
    //   steps_rate_ = steps_rate_ * (100 - (max_steps_[i] - steps_counter_[i]) + 1);
    // }
  }

#else
  // start of pulse
  digitalWrite(step_pin_[X_AXIS], bit_istrue(current_step_mask, 1 << X_AXIS));
  digitalWrite(step_pin_[Y_AXIS], bit_istrue(current_step_mask, 1 << Y_AXIS));
  // count motors steps, flip when direction bit is on
  steps_counter_[X_AXIS] += bit_to_sign(current_direction_mask, 1 << X_AXIS) * bit_istrue(current_step_mask, 1 << X_AXIS);
  steps_counter_[Y_AXIS] += bit_to_sign(current_direction_mask, 1 << Y_AXIS) * bit_istrue(current_step_mask, 1 << Y_AXIS);

#endif
  delayMicroseconds(STEP_PULSE_LENGTH);
  // end pulse
  digitalWrite(step_pin_[X_AXIS], LOW);
  digitalWrite(step_pin_[Y_AXIS], LOW);
}

void StepperController::move_step(int steps_mask, int current_direction_mask)
{

  // turn sed pulse
  if (steps_mask && (micros() - move_time_stamp_ > steps_rate_))
  {
    set_direction(current_direction_mask);
    this->step(steps_mask, current_direction_mask); // Send step
    move_time_stamp_ = micros();                    // reset timer
    
  }
}

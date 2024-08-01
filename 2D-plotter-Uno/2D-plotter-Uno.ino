#include <Arduino.h>
#include "StepperController.h"
#include "Settings.h"
#include "element_movement.h"

/* SHIR! - update the value here:
  CROSS - shti va erev 
  RANDOM - random
  TEST - all elements are pushed farwrd
  TEST_NEG - same as test, but to the other direction
*/
sys_state state = {IDLE, RANDOM , micros()};



StepperController stepper_c = StepperController();

// ELEMENTS STATE
int ELEMENT_MOVES[ELEMENTS_COUNT] = {0};
int current_element_index = 0;
unsigned long tune_rate = 0;
int x_direction = 1;
int y_direction = 1;
int cross_state = 0;
int random_val_was_chosen = false;
int mic_value = digitalRead(SOUND_SENSOR_PIN);


void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_c.get_steps_count()[X_AXIS]);
    Serial.print(",");
    Serial.println(stepper_c.get_steps_count()[Y_AXIS]);
}


void auto_homing(StepperController *stepper_c)
{
  Serial.println("Auto homing! ");
  stepper_c->set_steps_rate(AUTO_HOME_STEPS_RATE);
  stepper_c->set_enable(true);

  // Move X to 0    

  stepper_c->set_steps_count(mm_to_steps((X_MM_RAIL_LENGTH), X_STEPS_PER_MM), 0);  
  
  Serial.println("------");
  while ( digitalRead(X_LIMIT_SW_PIN) && stepper_c->get_steps_count()[X_AXIS] > 0 ) 
  {
      stepper_c->move_step(1, 1); // move backwards
      unsigned long bla = stepper_c->get_steps_count()[X_AXIS];
      if(bla % 7900 == 0){
        Serial.println(bla);
      }
  }
  
  stepper_c->set_steps_count(0, 0);  
  while (stepper_c->get_steps_count()[X_AXIS] < mm_to_steps(X_MM_HOMING_OFFSET, X_STEPS_PER_MM))
  {
      stepper_c->move_step(1, 0);
  }
  stepper_c->set_steps_count(0, 0);  
  
  Serial.println("Moved X axis to place!");

  
  stepper_c->set_steps_count(0, mm_to_steps((Y_MM_RAIL_LENGTH), Y_STEPS_PER_MM));  
  while (stepper_c->get_steps_count()[Y_AXIS] > 0 && digitalRead(Y_LIMIT_SW_PIN))
  {
      stepper_c->move_step(2, 2); // move backwards
  }
  
  while ( stepper_c->get_steps_count()[Y_AXIS] < mm_to_steps(Y_MM_HOMING_OFFSET, Y_STEPS_PER_MM))
  {
      stepper_c->move_step(2, 0);
  }
  stepper_c->set_steps_count(0, 0);
  Serial.println("Moved Y axis to place.");
  
  stepper_c->set_steps_rate(STEPS_RATE);
  Serial.println("Auto homing completed successfully! ");
  print_current_position();  
  
  // move to the center of Y axis 
  while ( stepper_c->get_steps_count()[Y_AXIS] < mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM))
  {
      stepper_c->move_step(2, 0);
  }
  Serial.println("Moved Y to center");
  // move to The first element 
  while ( stepper_c->get_steps_count()[X_AXIS] < mm_to_steps(X_OFFSET_MM, X_STEPS_PER_MM))
  {
      stepper_c->move_step(1, 0);
  }
  Serial.println("Moved to Element 0");
  Serial.println("-------------------------");

}


void update_next(int* current_element_index, int* x_direction){
  if ((*current_element_index == ELEMENTS_COUNT-1 && *x_direction > 0)){
    // case it is the last element - enter IDLE state
    state.sys_mode = IDLE;
    stepper_c.set_enable(false);
    Serial.println("Enter IDLE mode");
  }
  else{
    *current_element_index += *x_direction;
  }
}


void move_to_next(StepperController *stepper_c, int current_element_index){
  // Move X to the next element
  int direction_mask = 0;

  if (current_element_index % 30 == 0 && current_element_index != 0){
    // steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM) - X_STEPS_PER_MM;
    tune_rate += X_STEPS_PER_MM;
  }
  unsigned long steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM);

  while ( stepper_c->get_steps_count()[X_AXIS] != steps_to_move-tune_rate) 
  {
    stepper_c->move_step(1, direction_mask);
  }
  
  Serial.print("--moved to element: ");
  Serial.println(current_element_index);
}


void move_element(StepperController *stepper_c, int y_direction){
  // assuming element is at the Y center coordinate, x on the next element
  // Move Y to desired direction 
  int direction_mask = 0;
  if(y_direction < 0){
    direction_mask = 2;
  }
  while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps((Y_CENTER_MM + (y_direction*Y_RADIUS_MM)), Y_STEPS_PER_MM))
  {
    stepper_c->move_step(2, direction_mask);
  }
  Serial.print("--Pushed element: ");
  Serial.print(current_element_index);
  Serial.print(" to dir: ");
  Serial.println(y_direction);
  
  while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM))
  {
    stepper_c->move_step(2, (2-direction_mask));
  }
  Serial.println("--moved Y to center");
  
}


void move_to_first_element(StepperController *stepper_c, int* current_element_index){
  int direction_mask = 1;
  stepper_c->set_enable(true);
  while ( digitalRead(X_LIMIT_SW_PIN) && stepper_c->get_steps_count()[X_AXIS] != mm_to_steps((X_OFFSET_MM), X_STEPS_PER_MM)) 
  {
    stepper_c->move_step(1, direction_mask);
  }
  Serial.println("--moved to first element");
  *current_element_index = 0;
}


void print_elements_move(int ELEMENT_MOVES[ELEMENTS_COUNT]){
  Serial.println("Elements moves:");
  for(int i = 0; i< ELEMENTS_COUNT; i++ ){
    Serial.print(i);
    Serial.print(":");
    Serial.print(ELEMENT_MOVES[i]);
    Serial.print(", ");
  }
  Serial.println();

}


bool is_pressed(int button_pin){
  bool is_pressed = false;
  if (!digitalRead(button_pin)){
    is_pressed = true;
    delay(100);
    if (is_pressed && !digitalRead(button_pin)){
      Serial.println("Pressed!");
      return true;
    }
  }
  return false;
}


bool is_movement_valid(int ELEMENT_MOVES[ELEMENTS_COUNT], int current_element_index, int value){
  bool result = true;
  if (current_element_index >= MAX_ELEMENTS_SEQ){
    int sum = 0;
    for(int i = current_element_index-1; i >= current_element_index - MAX_ELEMENTS_SEQ ;  i-- ){
      sum += ELEMENT_MOVES[i] == value;
    }
    result =  !(sum == MAX_ELEMENTS_SEQ);
  }
  
  return result;
}


void random_direction(int current_element_index, int* y_direction){
  int random_val = random(2);
  if(!random_val_was_chosen){
    *y_direction = (random_val > 0) - (random_val == 0);
  }
  
  random_val_was_chosen = true;

}


void cross_direction(int current_element_index, int* y_direction){ 
  // Serial.println("CROSS");
  if(current_element_index %2 == cross_state ){
    *y_direction = 1;
  }
  else{
    *y_direction = -1;
  }
  // Serial.println("CROSS");
}


void test_direction(int current_element_index, int* y_direction){ 
  *y_direction = 1;
}


void test_negative_direction(int current_element_index, int* y_direction){ 
  *y_direction = -1;
}


void detect_direction(int current_element_index,int* y_direction, int *mic_value){
  // Serial.print("Max val: ");
  // Serial.println(*micValue);

  int current_val = (digitalRead(SOUND_SENSOR_PIN));
  *mic_value =  (current_val > *mic_value) ? current_val: *mic_value;
  *y_direction = (*mic_value > 0) - (*mic_value == 0);

}


void configure_y_direction(int ELEMENT_MOVES[ELEMENTS_COUNT], int current_element_index, int* y_direction, int *micValue){
  switch (state.movement_mode)
  {
    case TEST:
      test_direction(current_element_index, y_direction);
      break;
    case TEST_NEG:
      test_negative_direction(current_element_index, y_direction);
      break;
    case CROSS:
      cross_direction(current_element_index, y_direction);
      break;
    case RANDOM:
      random_direction(current_element_index, y_direction);
      break;
    case SOUND:
      detect_direction(current_element_index, y_direction, micValue);
      break;
  }
}


void correct_y_seq(int ELEMENT_MOVES[ELEMENTS_COUNT], int current_element_index, int* y_direction){
  // update the next value
  ELEMENT_MOVES[current_element_index] = *y_direction;
  if(!is_movement_valid(ELEMENT_MOVES, current_element_index, *y_direction)){
    // flip direction
    *y_direction = *y_direction == 1 ? -1:1;
    ELEMENT_MOVES[current_element_index] = *y_direction;
  }
}

void setup()
{

  Serial.begin(115200);
  randomSeed(analogRead(0));

  /** INIT PINS **/
  pinMode(X_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(Y_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  /** AUTO HOME**/
  auto_homing(&stepper_c);
  
  Serial.println("Entered Idle mode");
  state.sys_mode = IDLE;
  state.last_move_time_stamp = micros();

}

void loop()
{
  
  switch (state.sys_mode)
  {
  case LISTEN:
     if((micros() - state.last_move_time_stamp) > (PENDING_TIME_BETWEEN_ELEMENTS)){
        Serial.println("Enter PRINT mode");
        state.sys_mode = PRINT;
        // update current movement
        correct_y_seq(ELEMENT_MOVES, current_element_index, &y_direction);
        random_val_was_chosen = false;
        mic_value = 0;
         
      }
      else {
        configure_y_direction(ELEMENT_MOVES, current_element_index, &y_direction, &mic_value);
      }
    break;
  case PRINT:      
      move_to_next(&stepper_c, current_element_index); // get skipped on element 0 and last element
      // print_current_position();
      move_element(&stepper_c, y_direction);
      update_next(&current_element_index, &x_direction);
      // print_current_position();
      Serial.println("Enter LISTEN mode");
      state.sys_mode = LISTEN;
      state.last_move_time_stamp = micros();
    break;
  case IDLE:
      if (is_pressed(BUTTON_PIN)){
        print_elements_move(ELEMENT_MOVES);
        stepper_c.set_enable(true);
        move_to_first_element(&stepper_c,&current_element_index);
        cross_state = cross_state ? 0:1;
        
        // Enter listen mode
        Serial.println("Enter LISTEN mode");
        state.sys_mode = LISTEN;
        state.last_move_time_stamp = micros();
      }
      break;
  default:
      break;
  }
 
}

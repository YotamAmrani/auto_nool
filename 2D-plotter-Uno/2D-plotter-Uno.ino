#include <Arduino.h>
#include "StepperController.h"
#include "Settings.h"

// DEFINITIONS:
void print_current_position();

sys_state state = {IDLE, micros()};
Servo pen_controller;
int pen_state = PEN_OFF;
StepperController stepper_c = StepperController(&pen_controller);
int micValue = digitalRead(SOUND_SENSOR_PIN);

const unsigned long *current_position = stepper_c.get_steps_count();

// ELEMENTS STATE
int ELEMENT_MOVES[ELEMENTS_COUNT] = {0};
int current_element_index = 0;
int x_direction = 1;
int y_direction = 1;


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

void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_c.get_steps_count()[X_AXIS]);
    Serial.print(",");
    Serial.println(stepper_c.get_steps_count()[Y_AXIS]);

}


void update_next(int* current_element_index, int* x_direction, int* y_direction){
  if ((*current_element_index == ELEMENTS_COUNT-1 && *x_direction > 0)){
    state.sys_mode = IDLE;
    stepper_c.set_enable(false);
    Serial.println("Enter IDLE mode");
  }
  else{
    *current_element_index += *x_direction;
  }
//  random_direction(y_direction);

  // Serial.print("next index:");
  // Serial.println(*current_element_index);
  // Serial.print("next x direction:");
  // Serial.println(*x_direction);
}


void move_to_next(StepperController *stepper_c, int current_element_index, int x_direction){
  // Move X to the next element
  int direction_mask = 0;
  unsigned long steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM);
  if (current_element_index >= 100){
    steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM) - X_STEPS_PER_MM;
  }

  while ( stepper_c->get_steps_count()[X_AXIS] != steps_to_move) 
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

bool is_movement_valid(int ELEMENT_MOVES[ELEMENTS_COUNT], int current_element_index, int micValue){
  bool result = true;
  if (current_element_index >= MAX_ELEMENTS_SEQ){
    int sum = 0;
    // Serial.print("--");
    // Serial.println(micValue);
    for(int i = current_element_index-1; i >= current_element_index - MAX_ELEMENTS_SEQ ;  i-- ){
      sum += ELEMENT_MOVES[i] == micValue;
      // Serial.print(i);
      // Serial.print(":");
      // Serial.println(ELEMENT_MOVES[i]);
    }
    result =  !(sum == MAX_ELEMENTS_SEQ);
    // Serial.print("--sum: ");
    // Serial.println(sum);
    // Serial.print("--result: ");
    Serial.println(result);
  }
  
  return result;
}


void random_direction(int* y_direction){
  int random_val = random(2);
  *y_direction = (random_val > 0) - (random_val == 0);
  Serial.print("move:");
  Serial.println(*y_direction);
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
       delay(200);
        // Serial.println(PENDING_TIME_BETWEEN_ELEMENTS);
    //  if((micros() - state.last_move_time_stamp) > (PENDING_TIME_BETWEEN_ELEMENTS)){
        state.sys_mode = PRINT;
        Serial.println("Enter LISTEN mode");
        // Serial.print("Max val: ");
        // Serial.println(micValue);
        // if(is_movement_valid(ELEMENT_MOVES, current_element_index, micValue)){
          // y_direction = micValue;
        // }
        // else{
          // y_direction = micValue > 0 ? 0:1;
        // }
        
        ELEMENT_MOVES[current_element_index] = y_direction;
        micValue = 0;
        state.last_move_time_stamp = micros();
    //  }
    //  else
    //  {
        // int current_val = (digitalRead(SOUND_SENSOR_PIN));
        // micValue =  (current_val > micValue) ? current_val:micValue;
    //  }
      
      break;
  case PRINT:
      // print_current_position();
      
      move_to_next(&stepper_c,current_element_index, x_direction); // get skipped on element 0 and last element
      // print_current_position();
      // if(current_element_index %2 == 0 ){
      //   y_direction = 1;
      // }
      // else{
      //   y_direction = -1;
      // }
      move_element(&stepper_c, y_direction);
      
      
      state.sys_mode = LISTEN;
      Serial.println("Enter LISTEN mode");
      update_next(&current_element_index, &x_direction, &y_direction);
      // print_current_position();
      
      break;
  case IDLE:

      if (is_pressed(BUTTON_PIN)){
        print_elements_move(ELEMENT_MOVES);
        stepper_c.set_enable(true);
        move_to_first_element(&stepper_c,&current_element_index);
        state.sys_mode = LISTEN;
        Serial.println("Enter LISTEN mode");
        state.last_move_time_stamp = micros();
      }
      break;
  default:
      break;
  }
}

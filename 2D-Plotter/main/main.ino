#include <Arduino.h>
#include "StepperController.h"
#include "Planner.h"
#include "DrawingObjects.h"
#include "Settings.h"

// DEFINITIONS:
void print_current_position();

sys_state state = {IDLE, micros()};
Servo pen_controller;
int pen_state = PEN_OFF;
StepperController stepper_c = StepperController(&pen_controller);
int current_steps_mask = 0;
int current_direction_mask = 0;

const int *current_position = stepper_c.get_steps_count();

// ELEMENTS STATE
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
  while ( digitalRead(X_LIMIT_SW_PIN) && stepper_c->get_steps_count()[X_AXIS] > 0 ) 
  {
      stepper_c->move_step(1, 1); // move backwards
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
  while ( stepper_c->get_steps_count()[X_AXIS] < mm_to_steps(X_OFFSET_MM, Y_STEPS_PER_MM))
  {
      stepper_c->move_step(1, 0);
  }
  Serial.println("Moved to Element 0");

  stepper_c->set_enable(false);
  Serial.println("-------------------------");

  //  stepper_c->set_limits(Y_MM_MAX_LIMIT,Y_MM_MAX_LIMIT,X_MM_MIN_LIMIT, Y_MM_MIN_LIMIT);
}


void print_current_position()
{
    Serial.println("Position: ");
    Serial.print(stepper_c.get_steps_count()[X_AXIS]);
    Serial.print(",");
    Serial.println(stepper_c.get_steps_count()[Y_AXIS]);

}


void update_next(int* current_element_index, int* x_direction, int* y_direction){
  if ((*current_element_index == ELEMENTS_COUNT-1 && *x_direction > 0 )|| (*current_element_index == 0 && *x_direction < 0)){
    *x_direction = (*x_direction)*(-1);
    state.sys_mode = IDLE;
    Serial.println("Enter IDLE mode");
  }
  else{
    *current_element_index += *x_direction;
  }
  
  // Serial.print("next index:");
  // Serial.println(*current_element_index);
  // Serial.print("next x direction:");
  // Serial.println(*x_direction);
}

void move_to_next(StepperController *stepper_c, int current_element_index, int x_direction){
  // Move X to the next element
  int direction_mask = 0;
  if (x_direction < 0){
    direction_mask = 1;
  }
  stepper_c->set_enable(true);
  while ( stepper_c->get_steps_count()[X_AXIS] != mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM)) 
  {
    stepper_c->move_step(1, direction_mask);
  }
  stepper_c->set_enable(false);
  Serial.println("--moved to next");
}

void move_element(StepperController *stepper_c, int y_direction){
  // assuming element is at the Y center coordinate, x on the next element
  stepper_c->set_enable(true);
  // Move Y to desired direction 
  int direction_mask = 0;
  if(y_direction < 0){
    direction_mask = 2;
  }
  while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps((Y_CENTER_MM + (y_direction*Y_RADIUS_MM)), Y_STEPS_PER_MM))
  {
    stepper_c->move_step(2, direction_mask);
  }
  Serial.println("--Pushed element");
  while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM))
  {
    stepper_c->move_step(2, (2-direction_mask));
  }
  stepper_c->set_enable(false);
  Serial.println("--moved Y to center");
  
}


bool is_pressed(int button_pin){
  bool is_pressed = false;
  if (!digitalRead(button_pin)){
    is_pressed = true;
    delay(100);
    if (is_pressed && !digitalRead(button_pin)){
      return true;
    }
  }
  return false;
}


void setup()
{

  Serial.begin(115200);
  /** Init Joystick input pins **/
  /** AUTO HOME**/
  pinMode(X_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(Y_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  auto_homing(&stepper_c);
  // Serial.println("Entered Idle mode");
  stepper_c.set_enable(false);
  state.sys_mode = PRINT;

}

void loop()
{
  /** GET INPUT MASK **/
  current_steps_mask = 0;
  current_direction_mask = 0;

  // state_handler(current_steps_mask, pen_state, &stepper_c);

  switch (state.sys_mode)
  {
  case PRINT:
      delay(PENDING_TIME_BETWEEN_ELEMENTS);
      // print_current_position();
      move_to_next(&stepper_c,current_element_index, x_direction); // for element 0 won't move
      // print_current_position();
      move_element(&stepper_c, y_direction);
      update_next(&current_element_index, &x_direction, &y_direction);
      // print_current_position();
      break;

  case IDLE:
      if (is_pressed(BUTTON_PIN)){
        state.sys_mode = PRINT;
        Serial.println("Enter PRINNT mode");
      }
      break;
  default:
      break;
  }
}





// -------------------------------
/* LEGACY CODE
  // segment_plan seg_p = {0};
  // Planner pl = Planner(&stepper_c, &seg_p);
  // int target[N_INSTRUCTIONS] = {0, 0, 0};
  // int current_drawing = 0;

  void state_handler(int current_steps_mask, int pen_state, StepperController *stepper_c)
  {
      // // if movement was deteced
      // if (current_steps_mask ||  pen_state)
      // {

      //     stepper_c->set_enable(true);
      //     if (state.sys_mode == IDLE)
      //     {
      //         state.sys_mode = MOVE;
      //         // toggle_led(true); // turn led on
      //     }
      //     else if (state.sys_mode == PRINT)
      //     {
      //         state.sys_mode = MOVE;
      //         // toggle_led(true); // turn led on
      //         // reset the current state
      //         // change to move state
      //     }
      //     state.last_move_time_stamp = micros();
      // }
      // else
      // {
      //     if (state.sys_mode == MOVE && (micros() - state.last_move_time_stamp) > PEN_DEBOUNCE_TIME)
      //     {
      //         state.sys_mode = IDLE;
      //         // stepper_c->set_enable(false);
      //         // toggle_led(false); // turn led off
      //     }
      //     else if (state.sys_mode == PRINT && pl.is_drawing_finished())
      //     {
      //         Serial.println("--LOG: Changing state to IDLE");
      //         state.sys_mode = IDLE;
      //         // toggle_led(false);
      //         state.last_move_time_stamp = micros();
      //     }
      // }
  }

  void initialize_auto_print(int *current_drawing)
  {
      int element_direction = -1;
      // update_next_move(element_move, &current_element, &current_direction, element_direction);
      
      // pl.reset_drawing();
      // pl.load_drawing(&element_drawing);
      // stepper_c.set_enable(true);
      // state.sys_mode = PRINT;
      // Serial.println("--LOG: Changing state to PRINT");
  }

  int16_t element_move[3][N_INSTRUCTIONS] = {{0, 0, PEN_OFF}, // move to center
                                {0, 0, PEN_OFF}, // move left/right
                                {0, 0, PEN_OFF} // push the current element
                                };
                                
  Drawing element_drawing = Drawing(element_move, 3, STEPS_RATE);

  void update_next_move(int16_t element_move[3][N_INSTRUCTIONS], int *current_element, int* current_direction, int element_direction){
    int current_x_coord = X_OFFSET_MM + X_ELEMNT_SPACING_MM * (*current_element);
    if (*current_element == ELEMENTS_COUNT-1 || *current_element == 0){
      *current_direction = (*current_direction)*(-1);
    }
    // calc x axis - change to relative..
    *current_element += *current_direction;
    int x_coord = X_OFFSET_MM + X_ELEMNT_SPACING_MM * (*current_element);
    int y_coord = Y_CENTER_MM + element_direction * Y_RADIUS_MM;
    element_move[0][0] =  current_x_coord;
    element_move[0][1] =  Y_CENTER_MM;
    element_move[1][0] =  x_coord;
    element_move[1][1] =  Y_CENTER_MM;
    element_move[2][0] =  x_coord;
    element_move[2][1] =  y_coord;
    // for(int i=0; i< 3; i++){
    //   for (int j=0; j<2; j++){
    //     Serial.print(element_move[i][j]);
    //     Serial.print(",");
    //   }
    //   Serial.println("");
    //   delay(300);
    // }
    // Serial.println("");
  }
*/

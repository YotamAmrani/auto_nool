
// #include "element_movement.h"

// void update_next(int* current_element_index, int* x_direction, int* y_direction){
//   if ((*current_element_index == ELEMENTS_COUNT-1 && *x_direction > 0)){
//     state.sys_mode = IDLE;
//     stepper_c.set_enable(false);
//     Serial.println("Enter IDLE mode");
//   }
//   else{
//     *current_element_index += *x_direction;
//   }
// //  random_direction(y_direction);

//   // Serial.print("next index:");
//   // Serial.println(*current_element_index);
//   // Serial.print("next x direction:");
//   // Serial.println(*x_direction);
// }


// void move_to_next(StepperController *stepper_c, int current_element_index, int x_direction){
//   // Move X to the next element
//   int direction_mask = 0;
//   unsigned long steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM);
//   if (current_element_index >= 100){
//     steps_to_move = mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * current_element_index)), X_STEPS_PER_MM) - X_STEPS_PER_MM;
//   }

//   while ( stepper_c->get_steps_count()[X_AXIS] != steps_to_move) 
//   {
//     stepper_c->move_step(1, direction_mask);
//   }
  
//   Serial.print("--moved to element: ");
//   Serial.println(current_element_index);
// }


// void move_element(StepperController *stepper_c, int y_direction){
//   // assuming element is at the Y center coordinate, x on the next element
//   // Move Y to desired direction 
//   int direction_mask = 0;
//   if(y_direction < 0){
//     direction_mask = 2;
//   }
//   while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps((Y_CENTER_MM + (y_direction*Y_RADIUS_MM)), Y_STEPS_PER_MM))
//   {
//     stepper_c->move_step(2, direction_mask);
//   }
//   Serial.print("--Pushed element: ");
//   Serial.print(current_element_index);
//   Serial.print(" to dir: ");
//   Serial.println(y_direction);
  
//   while ( stepper_c->get_steps_count()[Y_AXIS] != mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM))
//   {
//     stepper_c->move_step(2, (2-direction_mask));
//   }
//   Serial.println("--moved Y to center");
  
// }

// void move_to_first_element(StepperController *stepper_c, int* current_element_index){
//   int direction_mask = 1;
//   stepper_c->set_enable(true);
//   while ( digitalRead(X_LIMIT_SW_PIN) && stepper_c->get_steps_count()[X_AXIS] != mm_to_steps((X_OFFSET_MM), X_STEPS_PER_MM)) 
//   {
//     stepper_c->move_step(1, direction_mask);
//   }
//   Serial.println("--moved to first element");
//   *current_element_index = 0;
// }

// void print_elements_move(int ELEMENT_MOVES[ELEMENTS_COUNT]){
//   Serial.println("Elements moves:");
//   for(int i = 0; i< ELEMENTS_COUNT; i++ ){
//     Serial.print(i);
//     Serial.print(":");
//     Serial.print(ELEMENT_MOVES[i]);
//     Serial.print(", ");
//   }
//   Serial.println();

// }

// bool is_pressed(int button_pin){
//   bool is_pressed = false;
//   if (!digitalRead(button_pin)){
//     is_pressed = true;
//     delay(100);
//     if (is_pressed && !digitalRead(button_pin)){
//       Serial.println("Pressed!");
//       return true;
//     }
//   }
//   return false;
// }

// bool is_movement_valid(int ELEMENT_MOVES[ELEMENTS_COUNT], int current_element_index, int micValue){
//   bool result = true;
//   if (current_element_index >= MAX_ELEMENTS_SEQ){
//     int sum = 0;
//     // Serial.print("--");
//     // Serial.println(micValue);
//     for(int i = current_element_index-1; i >= current_element_index - MAX_ELEMENTS_SEQ ;  i-- ){
//       sum += ELEMENT_MOVES[i] == micValue;
//       // Serial.print(i);
//       // Serial.print(":");
//       // Serial.println(ELEMENT_MOVES[i]);
//     }
//     result =  !(sum == MAX_ELEMENTS_SEQ);
//     // Serial.print("--sum: ");
//     // Serial.println(sum);
//     // Serial.print("--result: ");
//     Serial.println(result);
//   }
  
//   return result;
// }


// void random_direction(int* y_direction){
//   int random_val = random(2);
//   *y_direction = (random_val > 0) - (random_val == 0);
//   Serial.print("move:");
//   Serial.println(*y_direction);
// }

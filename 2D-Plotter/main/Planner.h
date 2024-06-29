#ifndef PLANNER_H
#define PLANNER_H
#include "StepperController.h"
#include "Drawings.h"

/** segment -represents the current line to plot **/
struct segment_plan
{
    const int *current_position;
    int target_position[N_AXIS];
    int current_step_mask;
    int current_direction_mask;
    int dx;
    int dy;
    int dominant_axis;
    int x_step_value;
    int y_step_value;
    int led_pwm_value;
};

/** A Planner for handling line plotting in a Non-Blocking manner **/
class Planner
{
private:
    StepperController *stepper_c_;
    Drawing *current_drawing_;
    struct segment_plan *segment_plan_;
    // https://stackoverflow.com/questions/22975633/reference-to-a-static-2d-array
    int current_segment_;
    bool finished_drawing_;

    // random x,y values to add in MM
    int random_x_val = 0;
    int random_y_val = 0;

public:
    bool is_segment_printing_;
   /**
    * Planner constructor.
    */
    Planner(StepperController *stepper_c, struct segment_plan *seg_pl);


    void test_print();
    
   /**
    * initialize a new segment plan, where segnebt states for the line cunnecting from the head current
    * Position to the next coordinate position.
    * @param target_pos - the next coordinate in the drawing plan.
    */
    void init_segment_plan(const int *target_pos);

    /**
    * Load a given drawings' properties to the planner.
    * @param current_drawing - the drawing object which will be loaded to the planner.
    */
    void load_drawing(Drawing *current_drawing);

    /**
    * A non-blocking method, moving the relevant Axis twards the current target coordinate.
    * the method will be invoked as long as the head didn't reached it's target position.
    * 
    */
    void move_to_position();

   /**
    * Given two coordinates, return direction mask for the segment starting at point1, and ending at point2
    * @param point1 starting coordinate of the semgment 
    * @param point2 end coordinate of the semgment
    * @return direction mask
    */
    int get_line_direction_mask(const int *point1, const int *point2);

   /**
    * Start plotting the drawing that was lastly loaded to the planner.
    */
    void plot_drawing();

   /**
    * returns true the planner finished plotting the current drawing.
    * @retun true if drawing is done.
    */
    bool is_drawing_finished();

   /**
    * reset the planner current drawing state.
    */
    void reset_drawing();

    // help methods - printing methods for current state
    void print_stepper();
    void print_segment();
    void print_steps();
    void print_segment_positions();
};

#endif

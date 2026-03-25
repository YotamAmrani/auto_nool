#include <Arduino.h>
#include "Settings.h"
#include "StepperController.h"

// ====== מצב בדיקה ======
static const unsigned long MOVE_INTERVAL_MS = 6000; // 6 שניות בין אלמנטים
static const int TEST_Y_DIRECTION = 1;              // "קדימה" ב-Y (אם יוצא הפוך תשני ל- -1)
// =======================

StepperController stepper_c;

int current_element_index = 0;
unsigned long tune_rate = 0;
unsigned long last_move_ms = 0;

// --- דיבוג אופציונלי ---
static void print_xy() {
  Serial.print("Xsteps=");
  Serial.print(stepper_c.get_steps_count()[X_AXIS]);
  Serial.print("  Ysteps=");
  Serial.println(stepper_c.get_steps_count()[Y_AXIS]);
}

// Move X to 0 (limit) then offset
static void move_x_to_zero() {
  // מתחילים "כאילו" בקצה כדי לוודא שנגיע ללימיט
  stepper_c.set_steps_count(mm_to_steps(X_MM_RAIL_LENGTH, X_STEPS_PER_MM),
                            mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM));

  // זוז אחורה עד הלימיט
  while (digitalRead(X_LIMIT_SW_PIN) && stepper_c.get_steps_count()[X_AXIS] > 0) {
    stepper_c.move_step(1, 1); // X שלילי
  }

  // קובעים X=0 בנגיעה
  stepper_c.set_steps_count(0, stepper_c.get_steps_count()[Y_AXIS]);

  // זזים קדימה X_MM_HOMING_OFFSET
  while (stepper_c.get_steps_count()[X_AXIS] < mm_to_steps(X_MM_HOMING_OFFSET, X_STEPS_PER_MM)) {
    stepper_c.move_step(1, 0); // X חיובי
  }

  // מאפסים שוב (כמו אצלך)
  stepper_c.set_steps_count(0, stepper_c.get_steps_count()[Y_AXIS]);
}

// Move Y to 0 (limit) then to center
static void move_y_to_center() {
  stepper_c.set_steps_count(stepper_c.get_steps_count()[X_AXIS],
                            mm_to_steps(Y_MM_RAIL_LENGTH, Y_STEPS_PER_MM));

  // זוז אחורה עד הלימיט
  while (stepper_c.get_steps_count()[Y_AXIS] > 0 && digitalRead(Y_LIMIT_SW_PIN)) {
    stepper_c.move_step(2, 2); // Y שלילי
  }

  // יוצאים מהסוויץ' Y_MM_HOMING_OFFSET
  while (stepper_c.get_steps_count()[Y_AXIS] < mm_to_steps(Y_MM_HOMING_OFFSET, Y_STEPS_PER_MM)) {
    stepper_c.move_step(2, 0); // Y חיובי
  }

  // אפס Y
  stepper_c.set_steps_count(stepper_c.get_steps_count()[X_AXIS], 0);

  // זוז למרכז
  while (stepper_c.get_steps_count()[Y_AXIS] < mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM)) {
    stepper_c.move_step(2, 0); // Y חיובי
  }
}

// Auto homing ואז מעבר לאלמנט הראשון
static void auto_homing_and_goto_first() {
  Serial.println("Auto homing...");
  stepper_c.set_steps_rate(AUTO_HOME_STEPS_RATE);
  stepper_c.set_enable(true);

  move_y_to_center();
  move_x_to_zero();

  // חזרה למהירות עבודה
  stepper_c.set_steps_rate(STEPS_RATE);

  // מעבר לאלמנט הראשון (X_OFFSET_MM)
  while (stepper_c.get_steps_count()[X_AXIS] < mm_to_steps(X_OFFSET_MM, X_STEPS_PER_MM)) {
    stepper_c.move_step(1, 0);
  }

  current_element_index = 0;
  tune_rate = 0;
  Serial.println("Homing done. At element 0.");
  print_xy();

  last_move_ms = millis();
}

// Move X to target element (כולל פיצוי כל 50 אלמנטים כמו אצלך)
static void move_to_element_x(int element_idx) {
  // פיצוי כל CALIBRATION_RATE
  if (element_idx % CALIBRATION_RATE == 0 && element_idx != 0) {
    tune_rate += X_STEPS_PER_MM;
  }

  unsigned long target_steps =
      mm_to_steps((X_OFFSET_MM + (X_ELEMNT_SPACING_MM * element_idx)), X_STEPS_PER_MM);

  // כיוון חיובי (0) כי אנחנו רק קדימה בבדיקה
  while (stepper_c.get_steps_count()[X_AXIS] != (target_steps - tune_rate)) {
    stepper_c.move_step(1, 0);
  }
}

// Push element in Y and return to center (כמו בקוד שלך)
static void push_element_y(int y_direction) {
  int direction_mask = 0;
  if (y_direction < 0) direction_mask = 2; // Y שלילי

  unsigned long target =
      mm_to_steps((Y_CENTER_MM + (y_direction * Y_RADIUS_MM)), Y_STEPS_PER_MM);

  while (stepper_c.get_steps_count()[Y_AXIS] != target) {
    stepper_c.move_step(2, direction_mask);
  }

  // חזרה למרכז
  unsigned long center = mm_to_steps(Y_CENTER_MM, Y_STEPS_PER_MM);
  while (stepper_c.get_steps_count()[Y_AXIS] != center) {
    stepper_c.move_step(2, (2 - direction_mask));
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(X_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(Y_LIMIT_SW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SOUND_SENSOR_PIN, INPUT);

  stepper_c.set_steps_rate(STEPS_RATE);
  stepper_c.set_enable(true);

  delay(300);
  auto_homing_and_goto_first();
}

void loop() {
  unsigned long now = millis();

  // כל 6 שניות: זז לאלמנט הבא + דוחף ב-Y
  if (now - last_move_ms >= MOVE_INTERVAL_MS) {
    // אם הגענו לסוף — לעצור
    if (current_element_index >= (ELEMENTS_COUNT - 1)) {
      Serial.println("Reached last element. Motors OFF.");
      stepper_c.set_enable(false);
      while (true) { delay(1000); }
    }

    current_element_index++;

    stepper_c.set_enable(true);

    // זז ל-X של האלמנט
    move_to_element_x(current_element_index);

    // דוחף את האלמנט ב-Y וחוזר למרכז
    push_element_y(TEST_Y_DIRECTION);

    Serial.print("IDX=");
    Serial.print(current_element_index);
    Serial.print("  tune_rate=");
    Serial.println(tune_rate);

    stepper_c.set_enable(false);

    last_move_ms = now;
  }
}

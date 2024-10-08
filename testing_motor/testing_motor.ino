/*   
 *   Basic example code for controlling a stepper without library
 *      
 *   by Dejan, https://howtomechatronics.com
 */

// defines pins
#define stepPin 2
#define dirPin 5
#define stepPin2 3
#define dirPin2 6
 
void setup() {
  Serial.begin(115200);
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stepPin2,OUTPUT); 
  pinMode(dirPin2,OUTPUT);
  pinMode(8,OUTPUT);

}
void loop() {
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  digitalWrite(dirPin2,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 800; x++) {
    digitalWrite(stepPin,HIGH); 
    digitalWrite(stepPin2,HIGH); 
    delayMicroseconds(700);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin,LOW); 
    digitalWrite(stepPin2,LOW); 
    delayMicroseconds(700); 
  }
  delay(1000); // One second delay
  Serial.println("one");

  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  digitalWrite(dirPin2,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 800; x++) {
    digitalWrite(stepPin,HIGH);
    digitalWrite(stepPin2,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(500);
  }
  
  digitalWrite(8,HIGH); 
  delay(2000);
  Serial.println("two");
  digitalWrite(8,LOW); 

}

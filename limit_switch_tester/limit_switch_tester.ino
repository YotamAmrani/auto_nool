
// limit switch tester
const int buttonPin = 9;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  digitalRead(buttonPin);
  digitalWrite(8, false);

  Serial.begin(115200);
  Serial.println("Hello World!!");

}

void loop() {
    buttonState = digitalRead(buttonPin);
    if(!buttonState){
      Serial.println(buttonState);
    }
      
    

    
    
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
//  if (buttonState == HIGH) {
//    Serial.println("H");
//  } else {
//    Serial.println("L");
//  }
}

// Define the pins used
int micPin = 8;
// int ledPin = 3;

// Variables for holding the mic value and led state
int micValue;
// int ledState

void setup() 
{
  Serial.begin(115200);
  Serial.println("start");
  pinMode(micPin, INPUT); // Configures the sound sensor pin as input
  // pinMode(ledPin, OUTPUT); // Configures the LED pin as output
}

void loop() 
{
  // Read the sound sensor value
  micValue = digitalRead(micPin);
  // micValue = analogRead(micPin);
  
  // Check if the sound sensor has detected noise
  if (micValue)
  {
    // ledState = digitalRead(ledPin); // Retrieves the current LED state
    // digitalWrite(ledPin, !ledState); // Toggles the LED state
      Serial.println("yo");
  }
      // Serial.println(micValue);
    delay(10); // Pauses for 0.5 seconds
}
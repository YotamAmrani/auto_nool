const byte xLimitPin = 9;
const byte yLimitPin = 10;
const byte zLimitPin = 11;


void setup()
{
   Serial.begin(115200);
   pinMode(xLimitPin, INPUT_PULLUP);
   pinMode(yLimitPin, INPUT_PULLUP);
   pinMode(zLimitPin, INPUT_PULLUP);
}

void loop()
{
   static unsigned long timer = 0;
   unsigned long interval = 200;
   if (millis() - timer >= interval)
   {
      timer = millis();
      Serial.print("x limit = ");
      Serial.print(digitalRead(xLimitPin));
      Serial.print("   y limit = ");
      Serial.print(digitalRead(yLimitPin));
      Serial.print("   z limit = ");
      Serial.println(digitalRead(zLimitPin));
   }
}
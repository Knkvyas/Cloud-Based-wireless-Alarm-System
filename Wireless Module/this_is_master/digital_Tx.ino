
#define op 8
int buttonState = 0;

void setup() {
  pinMode(op, INPUT);
  Serial.begin(38400); // Default communication rate of the Bluetooth module
}

void loop() {
 delay(100);
 buttonState = digitalRead(op);
 if (buttonState == 1) {
   Serial.print('1'); // Sends '1' to serialout
 }
 else {
   Serial.print('0');
 }  
}

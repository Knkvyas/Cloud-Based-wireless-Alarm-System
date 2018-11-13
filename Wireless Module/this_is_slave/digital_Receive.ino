#define ledPin 9
int state = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(38400); // Default communication rate of the Bluetooth module
}

void loop() {
 if(Serial.available() > 0){ // if data available at serial port
    state = Serial.read(); // assign incoming data to variable state
    Serial.write(state);
 }
 // Controlling the ALARAM
 if (state == '1') {
  digitalWrite(ledPin, HIGH);
  Serial.println("HIGH");// ALARA ON
  state = 0;
 }
 else if (state == '0') {
  digitalWrite(ledPin, LOW);
  Serial.println("LOW");// ALARM ON
  state = 0;
 }
 
}

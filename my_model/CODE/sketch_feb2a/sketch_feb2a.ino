void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for Serial (needed for some boards like Leonardo)
  }
  Serial.println("Arduino ready");
}

void loop() {
  if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n'); // read until newline
    msg.trim(); // remove \r or whitespace

    Serial.print("Received: ");
    Serial.println(msg);

    // Example action
    if (msg == "ON") {
      digitalWrite(LED_BUILTIN, HIGH);
    } 
    else if (msg == "OFF") {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

#define BATT_LVL A1

void setup() {
  // put your setup code here, to run once:
  pinMode(BATT_LVL, INPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(BATT_LVL));
  delay(100);  
} 

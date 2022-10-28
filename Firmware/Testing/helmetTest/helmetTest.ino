int sensorVal[2]; 

int currentTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT); 
  pinMode(A1, INPUT);
}

void loop() {
  if(millis() - currentTime >= 1) {
    sensorVal[0] = analogRead(A0);
    sensorVal[1] = analogRead(A1); 

    Serial.print(sensorVal[0]);
    Serial.print(", "); 
    Serial.println(sensorVal[1]);  

    currentTime = millis(); 
  }
}

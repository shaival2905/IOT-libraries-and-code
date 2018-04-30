const int trigPin = 5;  //D1
const int echoPin = 4;  //D2
int ledpin=13;
long duration;
float distance;
void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(9600); // Starts the serial communication
  pinMode(ledpin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
if(distance<15){
  digitalWrite(ledpin, HIGH);
  delay(distance*50);
  digitalWrite(ledpin, LOW);
  delay(distance*50);
}
else
  digitalWrite(ledpin, LOW);
}

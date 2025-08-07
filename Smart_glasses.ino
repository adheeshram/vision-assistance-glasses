#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySerial(10, 9);
DFRobotDFPlayerMini myDFPlayer;

const int trigPin = 6;
const int echoPin = 5;

unsigned long lastPlayTime = 0;
const unsigned long cooldown = 3000; // 3 seconds cooldown
int lastPlayedFile = 0;

long readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // no echo
  long distance = duration * 0.034 / 2; // cm
  return distance;
}

void playFileIfNeeded(int file) {
  if (file != lastPlayedFile && millis() - lastPlayTime > cooldown) {
    myDFPlayer.play(file);
    lastPlayTime = millis();
    lastPlayedFile = file;
  }
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer init failed!");
    while (true);
  }

  myDFPlayer.volume(25);
  Serial.println("System Ready.");
}

void loop() {
  long distance = readDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= 0 || distance > 150) {  // no obstacle detected or out of range
    lastPlayedFile = 0; // reset
    Serial.println("No obstacle detected.");
    // Do not play anything
  } else if (distance <= 30) {
    playFileIfNeeded(3);  // closest alert
  } else if (distance <= 100) {
    playFileIfNeeded(2);  // medium alert
  } else {  // 101 - 150 cm
    playFileIfNeeded(1);  // farthest alert
  }

  delay(200);
}

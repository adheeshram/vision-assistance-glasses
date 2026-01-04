#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

/* =======================
   HARDWARE CONFIGURATION
   ======================= */
SoftwareSerial dfSerial(10, 9);   // DFPlayer RX, TX
DFRobotDFPlayerMini dfPlayer;

const int TRIG_PIN = 6;
const int ECHO_PIN = 5;

/* =======================
   SYSTEM PARAMETERS
   ======================= */
const unsigned long SENSOR_INTERVAL = 80;      // ms between sensor reads
const unsigned long AUDIO_COOLDOWN  = 3000;    // ms between audio alerts
const int MAX_DISTANCE = 150;                  // cm
const int MIN_DISTANCE = 2;                    // cm

/* =======================
   DISTANCE SMOOTHING
   ======================= */
const int SMOOTHING_WINDOW = 5;
long distanceBuffer[SMOOTHING_WINDOW];
int bufferIndex = 0;
bool bufferFilled = false;

/* =======================
   STATE TRACKING
   ======================= */
unsigned long lastSensorTime = 0;
unsigned long lastAudioTime  = 0;
int currentAlertState = 0;

/*
 Alert States:
 0 = No obstacle
 1 = Far
 2 = Medium
 3 = Near
*/

/* =======================
   FUNCTION PROTOTYPES
   ======================= */
long readUltrasonic();
long getSmoothedDistance(long newReading);
int  determineAlertState(long distance);
void handleAudioAlert(int newState, long distance);
int  mapVolume(long distance);

/* =======================
   SETUP
   ======================= */
void setup() {
  Serial.begin(9600);
  dfSerial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if (!dfPlayer.begin(dfSerial)) {
    Serial.println("DFPlayer initialization failed!");
    while (true); // fail-safe halt
  }

  dfPlayer.volume(20); // default safe volume
  Serial.println("Vision Assistance System Initialized.");
}

/* =======================
   MAIN LOOP
   ======================= */
void loop() {
  unsigned long now = millis();

  if (now - lastSensorTime >= SENSOR_INTERVAL) {
    lastSensorTime = now;

    long rawDistance = readUltrasonic();
    long distance = getSmoothedDistance(rawDistance);

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    int newAlertState = determineAlertState(distance);
    handleAudioAlert(newAlertState, distance);
  }
}

/* =======================
   ULTRASONIC SENSOR LOGIC
   ======================= */
long readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1;

  long distance = duration * 0.034 / 2;
  return distance;
}

/* =======================
   MOVING AVERAGE FILTER
   ======================= */
long getSmoothedDistance(long newReading) {
  if (newReading < MIN_DISTANCE || newReading > MAX_DISTANCE) {
    return -1;
  }

  distanceBuffer[bufferIndex] = newReading;
  bufferIndex = (bufferIndex + 1) % SMOOTHING_WINDOW;

  if (bufferIndex == 0) bufferFilled = true;

  int count = bufferFilled ? SMOOTHING_WINDOW : bufferIndex;
  long sum = 0;

  for (int i = 0; i < count; i++) {
    sum += distanceBuffer[i];
  }

  return sum / count;
}

/* =======================
   ALERT STATE LOGIC
   ======================= */
int determineAlertState(long distance) {
  if (distance == -1 || distance > MAX_DISTANCE) return 0;
  if (distance <= 30)  return 3;
  if (distance <= 100) return 2;
  return 1;
}

/* =======================
   AUDIO MANAGEMENT
   ======================= */
void handleAudioAlert(int newState, long distance) {
  unsigned long now = millis();

  if (newState == 0) {
    currentAlertState = 0;
    return;
  }

  if (newState != currentAlertState && now - lastAudioTime > AUDIO_COOLDOWN) {
    int volume = mapVolume(distance);
    dfPlayer.volume(volume);
    dfPlayer.play(newState); // audio files 1–3

    lastAudioTime = now;
    currentAlertState = newState;

    Serial.print("Playing alert ");
    Serial.print(newState);
    Serial.print(" at volume ");
    Serial.println(volume);
  }
}

/* =======================
   DYNAMIC VOLUME CONTROL
   ======================= */
int mapVolume(long distance) {
  if (distance <= 30) return 28;
  if (distance <= 60) return 24;
  if (distance <= 100) return 20;
  return 16;
}

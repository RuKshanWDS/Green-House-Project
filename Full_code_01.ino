#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ------------ Hardware setup ------------
#define DHTPIN 1               // DHT22 data on D7
#define DHTTYPE DHT22
#define RELAY_FAN_PIN 8        // Fan relay on D8
#define RELAY_PUMP_PIN 4       // Pump relay on D9
#define SOIL_PIN 5             // Soil sensor digital output on D5

// Sonar (HC-SR04)
#define SONAR_TRIG_PIN 2       // TRIG on D2
#define SONAR_ECHO_PIN 3       // ECHO on D3

// Buzzer
#define BUZZER_PIN 12          // Buzzer connected to D12

// Relay logic: many modules are active-LOW; adjust if yours is active-HIGH
const uint8_t RELAY_ON_STATE  = LOW;
const uint8_t RELAY_OFF_STATE = (RELAY_ON_STATE == LOW) ? HIGH : LOW;

// Soil logic: many digital soil sensors read LOW when WET, HIGH when DRY
const uint8_t SOIL_WET_STATE = LOW;

// Buzzer logic: many active buzzers sound when pin is HIGH
const uint8_t BUZZER_ON_STATE  = HIGH;
const uint8_t BUZZER_OFF_STATE = LOW;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// ------------ Behavior config ------------
const int FAN_ON_THRESHOLD_C = 29;              // fan ON when temp > 30°C
const unsigned long READ_INTERVAL_MS = 2000;    // DHT22 needs ~2s between reads
const int LEVEL_CUTOFF_CM = 10;                 // pump OFF when distance <= 10 cm

// ------------ Function declarations ------------
int   readTemperatureC();                       // rounded °C, INT16_MIN on error
bool  readSoilWet();                            // true = wet, false = dry
long  readDistanceCm();                         // returns >=0 cm, or -1 on error/timeout
void  controlFan(int tempC);                    // fan by temperature
void  controlPump(bool soilWet, long distanceCm); // pump by soil + level
void  displayStatus(int tempC, bool soilWet, bool fanOn, bool pumpOn);
void  setRelay(uint8_t pin, bool on);           // drive any relay
bool  isRelayOn(uint8_t pin);                   // read relay state
void  updateBuzzer(bool fanOn, bool pumpOn);    // buzzer pattern when fan or pump is on
void  heartbeatTone();                          // Heartbeat tone generator
void  highNoiseTone();                          // High-pitched tone when fan or pump is on

// ------------ Setup ------------
void setup() {
  pinMode(RELAY_FAN_PIN, OUTPUT);
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  setRelay(RELAY_FAN_PIN, false);
  setRelay(RELAY_PUMP_PIN, false);

  pinMode(SOIL_PIN, INPUT);

  pinMode(SONAR_TRIG_PIN, OUTPUT);
  pinMode(SONAR_ECHO_PIN, INPUT);
  digitalWrite(SONAR_TRIG_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, BUZZER_OFF_STATE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Fan/Pump/Soil/DHT");
  lcd.setCursor(0, 1); lcd.print("Init...");
  dht.begin();
  delay(1500);
  lcd.clear();
}

// ------------ Main loop ------------
void loop() {
  static unsigned long lastRead = 0;
  unsigned long now = millis();

  if (now - lastRead >= READ_INTERVAL_MS) {
    lastRead = now;

    int   tempC      = readTemperatureC();
    bool  soilWet    = readSoilWet();
    long  distanceCm = readDistanceCm();   // not displayed, only used to override pump

    controlFan(tempC);                       // temperature rule for fan
    controlPump(soilWet, distanceCm);        // soil + level rule for pump

    bool fanOn  = isRelayOn(RELAY_FAN_PIN);
    bool pumpOn = isRelayOn(RELAY_PUMP_PIN);

    updateBuzzer(fanOn, pumpOn);             // buzzer pattern when any is ON
    displayStatus(tempC, soilWet, fanOn, pumpOn);
  }
}

// ------------ Implementations ------------
int readTemperatureC() {
  float t = dht.readTemperature(); // Celsius
  if (isnan(t)) return INT16_MIN;
  return (int)roundf(t);
}

bool readSoilWet() {
  int v = digitalRead(SOIL_PIN);
  return (v == SOIL_WET_STATE); // true if WET
}

// Basic HC-SR04 distance read (cm). Returns -1 on timeout/error.
long readDistanceCm() {
  // Ensure clean trigger
  digitalWrite(SONAR_TRIG_PIN, LOW);
  delayMicroseconds(3);
  // 10us pulse
  digitalWrite(SONAR_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SONAR_TRIG_PIN, LOW);

  // Measure echo. Timeout ~25ms (~4+ meters) to avoid blocking forever.
  unsigned long duration = pulseIn(SONAR_ECHO_PIN, HIGH, 25000UL);
  if (duration == 0) return -1; // timeout

  // Convert to cm: sound speed ~343 m/s => 29.1 us per cm round-trip; Arduino shorthand: /58
  long cm = duration / 58UL;
  return cm;
}

void controlFan(int tempC) {
  if (tempC == INT16_MIN) { setRelay(RELAY_FAN_PIN, false); return; }
  setRelay(RELAY_FAN_PIN, tempC > FAN_ON_THRESHOLD_C);
}

// Soil dry => request pump ON, but if water level (distance) is <= LEVEL_CUTOFF_CM, force OFF
void controlPump(bool soilWet, long distanceCm) {
  bool wantPumpOn = !soilWet; // dry if NOT wet

  // Sonar override: if a valid reading says distance <= cutoff, force pump OFF
  if (distanceCm >= 0 && distanceCm <= LEVEL_CUTOFF_CM) {
    setRelay(RELAY_PUMP_PIN, false);
    return;
  }

  setRelay(RELAY_PUMP_PIN, wantPumpOn);
}

// Line 1: "Tem:<val>  sl:<D|W>"
// Line 2: "Fan:<ON|OFF>  pm:<ON|OFF>"
void displayStatus(int tempC, bool soilWet, bool fanOn, bool pumpOn) {
  // Line 1
  lcd.setCursor(0, 0);
  lcd.print("Tem:            ");
  lcd.setCursor(4, 0);
  if (tempC == INT16_MIN) lcd.print("--");
  else                    lcd.print(tempC);

  lcd.setCursor(8, 0);
  lcd.print("sl:");
  lcd.print(soilWet ? "W " : "D ");

  // Line 2
  lcd.setCursor(0, 1);
  lcd.print("Fan:            ");
  lcd.setCursor(4, 1);
  lcd.print(fanOn ? "ON " : "OFF");

  lcd.setCursor(10, 1);
  lcd.print("pm:");
  lcd.print(pumpOn ? "ON " : "OFF");
}

void setRelay(uint8_t pin, bool on) {
  digitalWrite(pin, on ? RELAY_ON_STATE : RELAY_OFF_STATE);
}

bool isRelayOn(uint8_t pin) {
  return (digitalRead(pin) == RELAY_ON_STATE);
}

// ------------ Buzzer pattern when fan or pump is on ------------
void updateBuzzer(bool fanOn, bool pumpOn) {
  if (fanOn || pumpOn) {
    // If any device is on, play the high-pitched noise
    //highNoiseTone();
    heartbeatTone();
  } else {
    // System is ON but neither fan nor pump, play heartbeat tone
    //heartbeatTone();
    highNoiseTone();
  }
}

// Heartbeat pattern: two short beeps followed by a pause
void heartbeatTone() {
  // First short beep
  tone(BUZZER_PIN, 1000);  // 1000 Hz tone
  delay(100);              // Duration of first beep
  noTone(BUZZER_PIN);      // Stop tone
  delay(50);               // Short pause between beeps
  
  // Second short beep
  tone(BUZZER_PIN, 1000);  // 1000 Hz tone
  delay(100);              // Duration of second beep
  noTone(BUZZER_PIN);      // Stop tone
  delay(1000);             // Longer pause before next heartbeat
}

// High-pitched tone when fan or pump is on
void highNoiseTone() {
  tone(BUZZER_PIN, 5000);  // 3000 Hz tone for high-pitched sound
  delay(80);              // Duration of high noise tone
  noTone(BUZZER_PIN);      // Stop tone
  delay(3000);              // Silence before next tone
}

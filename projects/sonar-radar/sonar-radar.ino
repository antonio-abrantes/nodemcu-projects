/*
 * ============================================================
 *  RADAR ULTRASSÔNICO - ESP8266 (v8)
 *
 *  Pinos (confirmados):
 *    OLED SDA     → D2 (GPIO4)
 *    OLED SCL     → D1 (GPIO5)
 *    HC-SR04 VCC  → 3.3V
 *    HC-SR04 TRIG → D5 (GPIO14)
 *    HC-SR04 ECHO → D6 (GPIO12)
 *    SERVO Sinal  → D7 (GPIO13)
 *    BUZZER       → D8 (GPIO15)
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define TRIG_PIN   D5
#define ECHO_PIN   D6
#define SERVO_PIN  D7
#define BUZZER_PIN D8

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- RADAR ---
const int MIN_ANGLE  = 45;
const int MAX_ANGLE  = 135;
const int ALERT_CM   = 30;   // distância máxima de alerta
const int ALERT_MIN  = 5;    // distância mínima (bip mais rápido)
const int BIP_MAX_MS = 600;  // intervalo bip na distância máxima de alerta
const int BIP_MIN_MS = 200;  // intervalo bip na distância mínima

// --- ESTADO ---
int  angle     = MIN_ANGLE;
int  direction = 1;
int  distance  = 999;

// --- SERVO ---
void servoSetup() {
  pinMode(SERVO_PIN, OUTPUT);
  analogWriteFreq(50);
  analogWriteRange(20000);
}
void servoEscrever(int graus) {
  int pulso = map(graus, 0, 180, 500, 2500);
  analogWrite(SERVO_PIN, pulso);
}

// --- BUZZER progressivo ---
unsigned long ultimoBip    = 0;
bool          buzzerLigado = false;
const int     BIP_DURACAO  = 80;

int calcularIntervalo() {
  // Quanto mais perto, menor o intervalo
  int dist = constrain(distance, ALERT_MIN, ALERT_CM);
  return map(dist, ALERT_MIN, ALERT_CM, BIP_MIN_MS, BIP_MAX_MS);
}

void atualizarBuzzer() {
  if (distance > 0 && distance <= ALERT_CM) {
    unsigned long agora   = millis();
    int           intervalo = calcularIntervalo();

    if (buzzerLigado) {
      if (agora - ultimoBip >= BIP_DURACAO) {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerLigado = false;
        ultimoBip = agora;
      }
    } else {
      if (agora - ultimoBip >= (unsigned long)intervalo) {
        digitalWrite(BUZZER_PIN, HIGH);
        buzzerLigado = true;
        ultimoBip = agora;
      }
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerLigado = false;
  }
}

// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n========================================");
  Serial.println("  RADAR v8");
  Serial.println("========================================");

  Wire.begin(D2, D1);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Teste sensor
  Serial.println("[SENSOR] Teste:");
  for (int i = 1; i <= 3; i++) {
    delay(60);
    int d = lerDistancia();
    Serial.print("  "); Serial.print(i); Serial.print(": ");
    if (d == 999) Serial.println("sem eco");
    else { Serial.print(d); Serial.println(" cm"); }
  }

  // Servo
  servoSetup();
  Serial.println("[SERVO] Teste 45->135->90");
  servoEscrever(45);  delay(800);
  servoEscrever(135); delay(800);
  servoEscrever(90);  delay(600);
  Serial.println("[SERVO] OK");

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[ERRO] OLED!");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  Serial.println("[OLED] OK");

  // Bip boot
  digitalWrite(BUZZER_PIN, HIGH); delay(80);
  digitalWrite(BUZZER_PIN, LOW);

  angle = MIN_ANGLE;
  Serial.println("  Varrendo...\n");
}

// ============================================================
void loop() {
  servoEscrever(angle);
  delay(40);

  distance = lerDistancia();

  Serial.print("["); Serial.print(angle); Serial.print("g] ");
  if (distance == 999) Serial.println("sem eco");
  else {
    Serial.print(distance); Serial.print(" cm");
    if (distance <= ALERT_CM) {
      Serial.print("  ALERTA | intervalo bip: ");
      Serial.print(calcularIntervalo());
      Serial.print("ms");
    }
    Serial.println();
  }

  desenharRadar();
  atualizarBuzzer();

  angle += 2 * direction;
  if (angle >= MAX_ANGLE) { angle = MAX_ANGLE; direction = -1; }
  else if (angle <= MIN_ANGLE) { angle = MIN_ANGLE; direction =  1; }

  yield();
}

// ============================================================
int lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (dur == 0) return 999;
  return (int)(dur * 0.034f / 2.0f);
}

void desenharLinha(int ang, int len) {
  float rad = ang * PI / 180.0f;
  int x = 64 - (int)(len * cos(rad));
  int y = 64 - (int)(len * sin(rad));
  display.drawLine(64, 64, x, y, WHITE);
}

void desenharObjeto(int ang, int dist) {
  float rad = ang * PI / 180.0f;
  int x = 64 - (int)(dist * cos(rad));
  int y = 64 - (int)(dist * sin(rad));
  display.fillCircle(x, y, 3, WHITE);
}

void desenharRadar() {
  display.clearDisplay();
  display.drawCircle(64, 64, 20, WHITE);
  display.drawCircle(64, 64, 40, WHITE);
  display.drawCircle(64, 64, 60, WHITE);
  desenharLinha(MIN_ANGLE, 60);
  desenharLinha(MAX_ANGLE, 60);
  desenharLinha(angle, 60);

  if (distance > 0 && distance < 40) {
    int px = map(distance, 0, 40, 0, 60);
    desenharObjeto(angle, px);
    display.setCursor(0, 0);
    display.print(distance);
    display.print("cm");
    if (distance <= ALERT_CM) {
      display.setCursor(0, 10);
      display.print("! ALERTA !");
    }
  }

  display.setCursor(98, 0);
  display.print(angle);
  display.print((char)247);
  display.display();
}

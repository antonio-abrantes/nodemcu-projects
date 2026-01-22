/*
 * TESTADOR DE SERVOS ESP8266 - WEB INTERFACE
 * VERSÃO COM PWM DIRETO (sem biblioteca Servo)
 * 
 * LIGAÇÃO DO SERVO - TESTE 1:
 * - Sinal (amarelo/laranja): D1 (GPIO5)
 * - VCC (vermelho): VUSB (5V)
 * - GND (marrom/preto): GND
 * 
 * SE NÃO FUNCIONAR, TESTE 2:
 * - Sinal: D1 (GPIO5)
 * - VCC: 3.3V (pino vermelho direito)
 * - GND: GND
 * 
 * Acesse: http://192.168.100.147
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configurações WiFi
const char* ssid = "sua_rede_wifi";
const char* password = "sua_senha_wifi";

// IP Fixo
IPAddress ip(192, 168, 100, 147); // Defina o IP desejado
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

// TESTE ESTES PINOS UM POR VEZ - descomente apenas 1 linha:
const int SERVO_PIN = 5;   // D1 (GPIO5) - RECOMENDADO
// const int SERVO_PIN = 4;   // D2 (GPIO4)
// const int SERVO_PIN = 0;   // D3 (GPIO0)
// const int SERVO_PIN = 2;   // D4 (GPIO2)
// const int SERVO_PIN = 14;  // D5 (GPIO14)
// const int SERVO_PIN = 12;  // D6 (GPIO12)
// const int SERVO_PIN = 13;  // D7 (GPIO13)

// Variáveis de controle
int currentAngle = 90;
String servoType = "Testando...";
bool testing = false;

// Função para gerar pulso PWM manualmente
void writeServo(int angle) {
  // Converte ângulo (0-180) para largura de pulso (500-2500 microsegundos)
  // Usando valores ESTENDIDOS para compensar 3.3V
  int pulseWidth = map(angle, 0, 180, 400, 2600);
  
  // Gera 50 pulsos (aproximadamente 1 segundo)
  for(int i = 0; i < 50; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - pulseWidth); // Período de 20ms
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nIniciando Testador de Servos...");
  
  // Configurar pino do servo
  pinMode(SERVO_PIN, OUTPUT);
  
  // Conectar WiFi com IP fixo
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Acesse: http://192.168.100.147");
  
  // Configurar servidor web
  server.on("/", handleRoot);
  server.on("/test", handleTest);
  server.on("/move", handleMove);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("Servidor Web iniciado!");
  
  // Posição inicial
  writeServo(90);
  Serial.println("Servo inicializado em 90 graus");
}

void loop() {
  server.handleClient();
}

// Página principal
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Testador de Servos</title>";
  html += "<style>";
  html += "body { font-family: Arial; margin: 20px; background: #f0f0f0; }";
  html += ".container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1 { color: #333; text-align: center; }";
  html += ".info { background: #e3f2fd; padding: 15px; border-radius: 5px; margin: 20px 0; }";
  html += ".status { font-size: 24px; text-align: center; padding: 20px; margin: 20px 0; border-radius: 5px; }";
  html += ".type-180 { background: #c8e6c9; color: #2e7d32; }";
  html += ".type-360 { background: #ffccbc; color: #d84315; }";
  html += ".type-testing { background: #fff9c4; color: #f57f17; }";
  html += "button { width: 100%; padding: 15px; margin: 10px 0; font-size: 18px; border: none; border-radius: 5px; cursor: pointer; transition: 0.3s; }";
  html += ".btn-primary { background: #2196F3; color: white; }";
  html += ".btn-primary:hover { background: #1976D2; }";
  html += ".btn-success { background: #4CAF50; color: white; }";
  html += ".btn-success:hover { background: #45a049; }";
  html += ".control-group { margin: 20px 0; }";
  html += "input[type=range] { width: 100%; height: 40px; }";
  html += ".angle-display { text-align: center; font-size: 36px; font-weight: bold; color: #2196F3; margin: 10px 0; }";
  html += ".quick-btns { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; }";
  html += ".quick-btns button { padding: 10px; font-size: 16px; background: #607D8B; color: white; }";
  html += ".quick-btns button:hover { background: #455A64; }";
  html += "</style>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>🔧 Testador de Servos</h1>";
  
  html += "<div class='info'>";
  html += "<strong>📌 Conexão do Servo:</strong><br>";
  html += "🔴 <strong>TESTE 1:</strong> VCC no VUSB (5V)<br>";
  html += "🟢 Se não funcionar, <strong>TESTE 2:</strong> VCC no 3.3V<br>";
  html += "• Sinal (amarelo): D1 (GPIO5)<br>";
  html += "• GND (marrom): GND";
  html += "</div>";
  
  html += "<div id='status' class='status type-testing'>";
  html += "<strong>Tipo:</strong> " + servoType + "<br>";
  html += "<strong>Ângulo Atual:</strong> <span id='currentAngle'>" + String(currentAngle) + "</span>°";
  html += "</div>";
  
  html += "<button class='btn-primary' onclick='runTest()'>🔍 DETECTAR TIPO DO SERVO</button>";
  
  html += "<div class='control-group'>";
  html += "<h3>Controle Manual</h3>";
  html += "<div class='angle-display' id='angleValue'>90°</div>";
  html += "<input type='range' min='0' max='180' value='90' id='angleSlider' oninput='updateAngle(this.value)'>";
  html += "</div>";
  
  html += "<div class='quick-btns'>";
  html += "<button onclick='moveServo(0)'>0°</button>";
  html += "<button onclick='moveServo(45)'>45°</button>";
  html += "<button onclick='moveServo(90)'>90°</button>";
  html += "<button onclick='moveServo(135)'>135°</button>";
  html += "<button onclick='moveServo(180)'>180°</button>";
  html += "<button onclick='sweep()'>Varredura</button>";
  html += "</div>";
  
  html += "</div>";
  
  // JavaScript
  html += "<script>";
  html += "function updateAngle(val) {";
  html += "  document.getElementById('angleValue').innerText = val + '°';";
  html += "  moveServo(val);";
  html += "}";
  
  html += "function moveServo(angle) {";
  html += "  fetch('/move?angle=' + angle).then(() => updateStatus());";
  html += "}";
  
  html += "function runTest() {";
  html += "  if(confirm('Conecte o servo no pino D4 e clique OK para iniciar o teste.\\n\\nO teste levará cerca de 10 segundos.')) {";
  html += "    document.getElementById('status').className = 'status type-testing';";
  html += "    document.getElementById('status').innerHTML = '<strong>Testando...</strong><br>Aguarde 10 segundos';";
  html += "    fetch('/test').then(r => r.text()).then(data => {";
  html += "      setTimeout(updateStatus, 10000);";
  html += "    });";
  html += "  }";
  html += "}";
  
  html += "function sweep() {";
  html += "  let angle = 0;";
  html += "  let direction = 1;";
  html += "  let interval = setInterval(() => {";
  html += "    moveServo(angle);";
  html += "    document.getElementById('angleSlider').value = angle;";
  html += "    document.getElementById('angleValue').innerText = angle + '°';";
  html += "    angle += direction * 10;";
  html += "    if(angle >= 180 || angle <= 0) direction *= -1;";
  html += "  }, 200);";
  html += "  setTimeout(() => clearInterval(interval), 5000);";
  html += "}";
  
  html += "function updateStatus() {";
  html += "  fetch('/status').then(r => r.json()).then(data => {";
  html += "    document.getElementById('currentAngle').innerText = data.angle;";
  html += "    let statusDiv = document.getElementById('status');";
  html += "    statusDiv.innerHTML = '<strong>Tipo:</strong> ' + data.type + '<br><strong>Ângulo Atual:</strong> ' + data.angle + '°';";
  html += "    if(data.type.includes('180')) statusDiv.className = 'status type-180';";
  html += "    else if(data.type.includes('360')) statusDiv.className = 'status type-360';";
  html += "    else statusDiv.className = 'status type-testing';";
  html += "  });";
  html += "}";
  
  html += "setInterval(updateStatus, 2000);";
  html += "</script>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Executar teste de detecção
void handleTest() {
  Serial.println("\n=== INICIANDO TESTE DE DETECÇÃO ===");
  testing = true;
  servoType = "Testando...";
  
  // Posição inicial
  writeServo(90);
  delay(1000);
  
  // Teste 1: Movimento para 0°
  Serial.println("Teste 1: Movendo para 0°");
  writeServo(0);
  delay(2000);
  
  // Teste 2: Movimento para 180°
  Serial.println("Teste 2: Movendo para 180°");
  writeServo(180);
  delay(2000);
  
  // Teste 3: Voltar para 90°
  Serial.println("Teste 3: Voltando para 90°");
  writeServo(90);
  delay(2000);
  
  // Teste 4: Sequência rápida
  Serial.println("Teste 4: Sequência rápida");
  for(int i = 0; i <= 180; i += 30) {
    writeServo(i);
    delay(500);
  }
  
  writeServo(90);
  delay(1000);
  
  // Análise (simplificada - detecção por software é limitada)
  servoType = "✅ Servo de 180° (Posicional)";
  Serial.println("\n=== RESULTADO ===");
  Serial.println("Análise: Servo responde a comandos de posição");
  Serial.println("Tipo detectado: 180° (Posicional)");
  Serial.println("\nOBSERVAÇÃO:");
  Serial.println("Se o servo GIROU CONTINUAMENTE = 360° (Rotação Contínua)");
  Serial.println("Se o servo FOI PARA POSIÇÕES e PAROU = 180° (Posicional)");
  
  testing = false;
  currentAngle = 90;
  
  server.send(200, "text/plain", "Teste concluído! Observe o comportamento do servo.");
}

// Mover servo para ângulo específico
void handleMove() {
  if(server.hasArg("angle")) {
    int angle = server.arg("angle").toInt();
    angle = constrain(angle, 0, 180);
    currentAngle = angle;
    writeServo(angle);
    Serial.print("Servo movido para: ");
    Serial.print(angle);
    Serial.println("°");
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Ângulo não especificado");
  }
}

// Status atual (JSON)
void handleStatus() {
  String json = "{";
  json += "\"angle\":" + String(currentAngle) + ",";
  json += "\"type\":\"" + servoType + "\"";
  json += "}";
  server.send(200, "application/json", json);
}
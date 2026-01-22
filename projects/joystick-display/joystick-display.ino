#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick pins
#define JOY_X A0
#define JOY_Y D5
#define JOY_SW D6

// Configurações WiFi
const char* ssid = "sua_rede_wifi";
const char* password = "sua_senha_wifi";

// IP Fixo
IPAddress ip(192, 168, 100, 147); // Defina o IP fixo desejado
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

// Variáveis do joystick
int xVal = 512;
int yVal = 512;
bool buttonPressed = false;
String direction = "CENTER";

void setup() {
  Serial.begin(115200);
  
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(JOY_Y, INPUT);
  
  // Configurar I2C
  Wire.begin(D2, D1);
  
  // Inicializar OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Conectando WiFi...");
  display.display();
  
  // Configurar IP fixo
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  // Aguardar conexão
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    display.println("WiFi OK!");
    display.print("IP: ");
    display.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha na conexão!");
    display.println("WiFi ERRO!");
  }
  
  display.display();
  delay(2000);
  
  // Configurar rotas do servidor
  server.on("/", handleRoot);
  server.on("/data", handleData);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
  
  // Ler joystick
  xVal = analogRead(JOY_X);
  int yDigital = digitalRead(JOY_Y);
  yVal = yDigital * 1023;
  buttonPressed = (digitalRead(JOY_SW) == LOW);
  direction = getJoystickDirection(xVal, yVal);
  
  // Atualizar display
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("IP:");
  display.println(WiFi.localIP());
  
  display.print("X:");
  display.print(xVal);
  display.print(" Y:");
  display.println(yVal);
  
  display.print("Dir: ");
  display.println(direction);
  
  display.print("BTN: ");
  display.println(buttonPressed ? "ON" : "OFF");
  
  display.display();
  delay(50);
}

String getJoystickDirection(int x, int y) {
  const int centerMin = 400;
  const int centerMax = 600;
  
  if (x < centerMin) {
    if (y < centerMin) return "UL";
    else if (y > centerMax) return "DL";
    else return "LEFT";
  } else if (x > centerMax) {
    if (y < centerMin) return "UR";
    else if (y > centerMax) return "DR";
    else return "RIGHT";
  } else {
    if (y < centerMin) return "UP";
    else if (y > centerMax) return "DOWN";
    else return "CENTER";
  }
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Joystick Web Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    
    body {
      font-family: 'Arial', sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
      color: white;
    }
    
    h1 {
      margin-bottom: 20px;
      text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
    }
    
    #canvas-container {
      background: white;
      border-radius: 15px;
      padding: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.3);
      margin-bottom: 20px;
    }
    
    #gameCanvas {
      border: 3px solid #333;
      background: #f0f0f0;
      display: block;
    }
    
    #info {
      background: rgba(255,255,255,0.2);
      backdrop-filter: blur(10px);
      padding: 15px 30px;
      border-radius: 10px;
      text-align: center;
      min-width: 300px;
    }
    
    #info p {
      margin: 8px 0;
      font-size: 16px;
    }
    
    .value {
      font-weight: bold;
      color: #ffd700;
    }
  </style>
</head>
<body>
  <h1>🎮 Joystick Web Control</h1>
  
  <div id="canvas-container">
    <canvas id="gameCanvas" width="600" height="400"></canvas>
  </div>
  
  <div id="info">
    <p>Direção: <span class="value" id="direction">CENTER</span></p>
    <p>X: <span class="value" id="xValue">512</span> | Y: <span class="value" id="yValue">512</span></p>
    <p>Botão: <span class="value" id="button">OFF</span></p>
  </div>
  
  <script>
    const canvas = document.getElementById('gameCanvas');
    const ctx = canvas.getContext('2d');
    
    let squareX = canvas.width / 2 - 25;
    let squareY = canvas.height / 2 - 25;
    const squareSize = 50;
    const speed = 3;
    
    let currentDirection = 'CENTER';
    
    function drawSquare() {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      
      // Desenhar grade
      ctx.strokeStyle = '#ddd';
      ctx.lineWidth = 1;
      for (let i = 0; i < canvas.width; i += 50) {
        ctx.beginPath();
        ctx.moveTo(i, 0);
        ctx.lineTo(i, canvas.height);
        ctx.stroke();
      }
      for (let i = 0; i < canvas.height; i += 50) {
        ctx.beginPath();
        ctx.moveTo(0, i);
        ctx.lineTo(canvas.width, i);
        ctx.stroke();
      }
      
      // Desenhar quadrado
      ctx.fillStyle = '#667eea';
      ctx.strokeStyle = '#764ba2';
      ctx.lineWidth = 3;
      ctx.fillRect(squareX, squareY, squareSize, squareSize);
      ctx.strokeRect(squareX, squareY, squareSize, squareSize);
      
      // Sombra
      ctx.fillStyle = 'rgba(0,0,0,0.2)';
      ctx.fillRect(squareX + 5, squareY + 5, squareSize, squareSize);
    }
    
    function moveSquare() {
      switch(currentDirection) {
        case 'UP':
          squareY = Math.max(0, squareY - speed);
          break;
        case 'DOWN':
          squareY = Math.min(canvas.height - squareSize, squareY + speed);
          break;
        case 'LEFT':
          squareX = Math.max(0, squareX - speed);
          break;
        case 'RIGHT':
          squareX = Math.min(canvas.width - squareSize, squareX + speed);
          break;
        case 'UL':
          squareX = Math.max(0, squareX - speed);
          squareY = Math.max(0, squareY - speed);
          break;
        case 'UR':
          squareX = Math.min(canvas.width - squareSize, squareX + speed);
          squareY = Math.max(0, squareY - speed);
          break;
        case 'DL':
          squareX = Math.max(0, squareX - speed);
          squareY = Math.min(canvas.height - squareSize, squareY + speed);
          break;
        case 'DR':
          squareX = Math.min(canvas.width - squareSize, squareX + speed);
          squareY = Math.min(canvas.height - squareSize, squareY + speed);
          break;
      }
      drawSquare();
    }
    
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          currentDirection = data.direction;
          document.getElementById('direction').textContent = data.direction;
          document.getElementById('xValue').textContent = data.x;
          document.getElementById('yValue').textContent = data.y;
          document.getElementById('button').textContent = data.button ? 'PRESSED' : 'OFF';
        })
        .catch(err => console.error('Erro:', err));
    }
    
    // Atualizar dados a cada 100ms
    setInterval(updateData, 100);
    
    // Animar movimento
    setInterval(moveSquare, 16);
    
    // Desenho inicial
    drawSquare();
  </script>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"x\":" + String(xVal) + ",";
  json += "\"y\":" + String(yVal) + ",";
  json += "\"direction\":\"" + direction + "\",";
  json += "\"button\":" + String(buttonPressed ? "true" : "false");
  json += "}";
  
  server.send(200, "application/json", json);
}
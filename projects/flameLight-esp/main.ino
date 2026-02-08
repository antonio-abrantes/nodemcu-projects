/*
 * ========== CONTROLE RGB + LED VELA COM ESP-01 VIA WEB ==========
 * 
 * CONFIGURAÇÃO DE HARDWARE:
 * 
 * Transistores RGB:
 * - 3x 2N2222 (NPN)
 * 
 * Resistores:
 * - 3x 470Ω (bases - entre GPIO e base do transistor)
 * - 3x 220Ω (emissores - entre emissor e LEDs RGB)
 * - 1x 220Ω (para LED branco vela)
 * 
 * CONEXÕES ESP-01 → TRANSISTORES RGB:
 * ==========================================
 * ESP-01 Pino 3 (GPIO 0)    → 470Ω → Base Q1 (Canal VERMELHO)
 * ESP-01 Pino 2 (GPIO 2)    → 470Ω → Base Q2 (Canal VERDE)
 * ESP-01 Pino 4 (GPIO 3/RXD) → 470Ω → Base Q3 (Canal AZUL)
 * 
 * CONEXÃO LED BRANCO (EFEITO VELA):
 * ==========================================
 * ESP-01 Pino 5 (GPIO 1/TXD) → 220Ω → LED branco (+) → LED (-) → GND
 * 
 * ALIMENTAÇÃO:
 * ==========================================
 * ESP-01 VCC → 3.3V
 * Coletores Q1, Q2, Q3 → 9V (fonte externa)
 * GND → Comum
 * 
 * ⚠️ CONFIGURAÇÃO WIFI DINÂMICA:
 * ==========================================
 * PRIMEIRA VEZ:
 * 1. Liga o ESP-01
 * 2. Conecta no WiFi "ESP-LED-Config" (sem senha)
 * 3. Abre navegador em http://192.168.4.1
 * 4. Escolhe sua rede e coloca senha
 * 5. ESP salva e reinicia
 * 
 * PRÓXIMAS VEZES:
 * - ESP conecta automaticamente na rede salva
 * - Acessa pela interface web ou procure "esp-led-rgb" na rede
 * 
 * ⚠️ RESET CONFIGURAÇÃO:
 * - Use o botão "Reconfigurar WiFi" na interface web
 * - Ou faça upload do código novamente
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  // Biblioteca WiFiManager
#include <EEPROM.h>

// ========== PINOS RGB ==========
const int pinVermelho = 0;  // GPIO 0 (Pino 3 do ESP-01)
const int pinVerde = 2;     // GPIO 2 (Pino 2 do ESP-01)
const int pinAzul = 3;      // GPIO 3 / RXD (Pino 4 do ESP-01)

// ========== PINO LED VELA ==========
const int pinVela = 1;      // GPIO 1 / TXD (Pino 5 do ESP-01)

// Servidor Web na porta 80
ESP8266WebServer server(80);

// Valores RGB atuais (0-255)
int valorR = 0;
int valorG = 0;
int valorB = 0;

// Controle do efeito vela
unsigned long ultimoUpdateVela = 0;
int brilhoVela = 200;
bool rgbLigado = false;
bool velaHabilitada = true;

// Variáveis para sistema de efeitos múltiplos
int efeitoAtual = 0;
unsigned long tempoEfeito = 0;
unsigned long inicioEfeito = 0;
int brilhoAlvo = 200;
int velocidadeMudanca = 10;

// Tipos de efeitos
#define EFEITO_RAJADA_EXTREMA 0
#define EFEITO_VENTO_FORTE 1
#define EFEITO_VENTO_MEDIO 2
#define EFEITO_VENTO_FRACO 3
#define EFEITO_CHAMA_ESTAVEL 4
#define EFEITO_CHAMA_CRESCENDO 5
#define EFEITO_PISCADAS_RAPIDAS 6

// ========== HTML DA INTERFACE WEB ==========
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Controle RGB + Vela - ESP-01</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            background: white;
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            max-width: 500px;
            width: 100%;
        }
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 30px;
            font-size: 28px;
        }
        .color-preview {
            width: 100%;
            height: 150px;
            border-radius: 10px;
            margin-bottom: 30px;
            border: 3px solid #ddd;
            transition: background-color 0.3s;
        }
        .slider-group {
            margin-bottom: 25px;
        }
        .slider-label {
            display: flex;
            justify-content: space-between;
            margin-bottom: 10px;
            font-weight: bold;
            font-size: 16px;
        }
        .slider-label.red { color: #e74c3c; }
        .slider-label.green { color: #27ae60; }
        .slider-label.blue { color: #3498db; }
        input[type="range"] {
            width: 100%;
            height: 8px;
            border-radius: 5px;
            outline: none;
            -webkit-appearance: none;
        }
        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: white;
            border: 2px solid #333;
            cursor: pointer;
        }
        .red-slider {
            background: linear-gradient(to right, #000, #e74c3c);
        }
        .green-slider {
            background: linear-gradient(to right, #000, #27ae60);
        }
        .blue-slider {
            background: linear-gradient(to right, #000, #3498db);
        }
        .preset-buttons {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin-top: 30px;
        }
        .preset-btn {
            padding: 15px;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: bold;
            cursor: pointer;
            transition: transform 0.2s;
            color: white;
        }
        .preset-btn:active {
            transform: scale(0.95);
        }
        .btn-red { background: #e74c3c; }
        .btn-green { background: #27ae60; }
        .btn-blue { background: #3498db; }
        .btn-yellow { background: #f39c12; }
        .btn-cyan { background: #1abc9c; }
        .btn-magenta { background: #9b59b6; }
        .btn-white { background: #ecf0f1; color: #333; }
        .vela-controls {
            margin-top: 20px;
            padding: 15px;
            background: #fff3cd;
            border-radius: 10px;
            border: 2px solid #f39c12;
        }
        .vela-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-bottom: 10px;
        }
        .vela-title {
            display: flex;
            align-items: center;
            gap: 10px;
            font-weight: bold;
            color: #f39c12;
        }
        .vela-icon {
            font-size: 24px;
            animation: flicker 2s infinite;
        }
        @keyframes flicker {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }
        .toggle-btn {
            padding: 8px 20px;
            border: none;
            border-radius: 20px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
            font-size: 13px;
        }
        .toggle-btn.on {
            background: #27ae60;
            color: white;
        }
        .toggle-btn.off {
            background: #e74c3c;
            color: white;
        }
        .vela-status {
            text-align: center;
            font-size: 12px;
            color: #666;
            margin-top: 8px;
        }
        .info {
            text-align: center;
            margin-top: 20px;
            padding: 15px;
            background: #ecf0f1;
            border-radius: 10px;
            font-size: 12px;
            color: #555;
        }
        .power-btn {
            width: 100%;
            padding: 18px;
            margin-top: 20px;
            border: none;
            border-radius: 12px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
            box-shadow: 0 4px 8px rgba(0,0,0,0.2);
        }
        .power-btn.ligar {
            background: #27ae60;
            color: white;
        }
        .power-btn.desligar {
            background: #e74c3c;
            color: white;
        }
        .power-btn:active {
            transform: scale(0.98);
        }
        .reset-wifi {
            width: 100%;
            padding: 12px;
            margin-top: 10px;
            border: 2px solid #e67e22;
            background: white;
            border-radius: 8px;
            font-size: 13px;
            font-weight: bold;
            color: #e67e22;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎨 Controle LED RGB + Vela</h1>
        
        <div class="color-preview" id="preview"></div>
        
        <div class="slider-group">
            <div class="slider-label red">
                <span>Vermelho</span>
                <span id="valueR">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="red-slider" id="sliderR">
        </div>
        
        <div class="slider-group">
            <div class="slider-label green">
                <span>Verde</span>
                <span id="valueG">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="green-slider" id="sliderG">
        </div>
        
        <div class="slider-group">
            <div class="slider-label blue">
                <span>Azul</span>
                <span id="valueB">0</span>
            </div>
            <input type="range" min="0" max="255" value="0" class="blue-slider" id="sliderB">
        </div>
        
        <div class="preset-buttons">
            <button class="preset-btn btn-red" onclick="setColor(255,0,0)">Vermelho</button>
            <button class="preset-btn btn-green" onclick="setColor(0,255,0)">Verde</button>
            <button class="preset-btn btn-blue" onclick="setColor(0,0,255)">Azul</button>
            <button class="preset-btn btn-yellow" onclick="setColor(255,255,0)">Amarelo</button>
            <button class="preset-btn btn-cyan" onclick="setColor(0,255,255)">Ciano</button>
            <button class="preset-btn btn-magenta" onclick="setColor(255,0,255)">Magenta</button>
            <button class="preset-btn btn-white" onclick="setColor(255,255,255)">Branco</button>
        </div>
        
        <div class="vela-controls">
            <div class="vela-header">
                <div class="vela-title">
                    <span class="vela-icon">🕯️</span>
                    <span>Efeito Vela</span>
                </div>
                <button class="toggle-btn on" id="velaBtn" onclick="toggleVela()">Ligado</button>
            </div>
            <div class="vela-status" id="velaStatus">
                Ativo quando RGB estiver ligado
            </div>
        </div>
        
        <button class="power-btn desligar" id="powerBtn" onclick="togglePower()">
            🔴 Desligar Tudo
        </button>
        
        <button class="reset-wifi" onclick="resetWiFi()">
            📶 Reconfigurar WiFi
        </button>
        
        <div class="info">
            ESP-01 controlando LED RGB + Vela<br>
            IP: <span id="ipAddr">Carregando...</span>
        </div>
    </div>
    
    <script>
        const sliderR = document.getElementById('sliderR');
        const sliderG = document.getElementById('sliderG');
        const sliderB = document.getElementById('sliderB');
        const valueR = document.getElementById('valueR');
        const valueG = document.getElementById('valueG');
        const valueB = document.getElementById('valueB');
        const preview = document.getElementById('preview');
        const velaBtn = document.getElementById('velaBtn');
        const velaStatus = document.getElementById('velaStatus');
        const powerBtn = document.getElementById('powerBtn');
        
        let velaLigada = true;
        let sistemaLigado = false;
        
        // Pega IP do ESP
        fetch('/getIP').then(r => r.text()).then(ip => {
            document.getElementById('ipAddr').textContent = ip;
        });
        
        function updatePreview() {
            const r = sliderR.value;
            const g = sliderG.value;
            const b = sliderB.value;
            preview.style.backgroundColor = `rgb(${r},${g},${b})`;
            valueR.textContent = r;
            valueG.textContent = g;
            valueB.textContent = b;
            
            sistemaLigado = (r > 0 || g > 0 || b > 0);
            updatePowerButton();
        }
        
        function updatePowerButton() {
            if (sistemaLigado) {
                powerBtn.className = 'power-btn desligar';
                powerBtn.innerHTML = '🔴 Desligar Tudo';
            } else {
                powerBtn.className = 'power-btn ligar';
                powerBtn.innerHTML = '🟢 Ligar';
            }
        }
        
        function sendColor() {
            const r = sliderR.value;
            const g = sliderG.value;
            const b = sliderB.value;
            fetch(`/setRGB?r=${r}&g=${g}&b=${b}`);
        }
        
        function setColor(r, g, b) {
            sliderR.value = r;
            sliderG.value = g;
            sliderB.value = b;
            updatePreview();
            sendColor();
        }
        
        function toggleVela() {
            velaLigada = !velaLigada;
            
            if (velaLigada) {
                velaBtn.className = 'toggle-btn on';
                velaBtn.textContent = 'Ligado';
                velaStatus.textContent = 'Ativo quando RGB estiver ligado';
            } else {
                velaBtn.className = 'toggle-btn off';
                velaBtn.textContent = 'Desligado';
                velaStatus.textContent = 'Efeito de vela desativado';
            }
            
            fetch(`/toggleVela?estado=${velaLigada ? '1' : '0'}`);
        }
        
        function togglePower() {
            if (sistemaLigado) {
                setColor(0, 0, 0);
            } else {
                setColor(255, 255, 255);
            }
        }
        
        function resetWiFi() {
            if (confirm('Deseja realmente resetar as configurações WiFi? O ESP vai reiniciar.')) {
                fetch('/resetWiFi');
                alert('ESP reiniciando... Conecte no WiFi "ESP-LED-Config" para reconfigurar!');
            }
        }
        
        sliderR.addEventListener('input', () => {
            updatePreview();
            sendColor();
        });
        
        sliderG.addEventListener('input', () => {
            updatePreview();
            sendColor();
        });
        
        sliderB.addEventListener('input', () => {
            updatePreview();
            sendColor();
        });
        
        updatePreview();
    </script>
</body>
</html>
)rawliteral";

// ========== FUNÇÕES ==========

void setRGB(int r, int g, int b) {
    // Se TUDO for zero, desliga COM digitalWrite (força LOW)
    if (r == 0 && g == 0 && b == 0) {
        valorR = 0;
        valorG = 0;
        valorB = 0;
        
        digitalWrite(pinVermelho, LOW);
        digitalWrite(pinVerde, LOW);
        digitalWrite(pinAzul, LOW);
        
        rgbLigado = false;
        return;
    }
    
    // Caso contrário, usa PWM normal
    valorR = constrain(r, 0, 255);
    valorG = constrain(g, 0, 255);
    valorB = constrain(b, 0, 255);
    
    analogWrite(pinVermelho, valorR);
    analogWrite(pinVerde, valorG);
    analogWrite(pinAzul, valorB);
    
    rgbLigado = (valorR > 0 || valorG > 0 || valorB > 0);
}

void escolherNovoEfeito() {
    efeitoAtual = random(0, 7);
    
    switch(efeitoAtual) {
        case EFEITO_RAJADA_EXTREMA:
            tempoEfeito = random(800, 1500);
            break;
        case EFEITO_VENTO_FORTE:
            tempoEfeito = random(2000, 4000);
            break;
        case EFEITO_VENTO_MEDIO:
            tempoEfeito = random(3000, 6000);
            break;
        case EFEITO_VENTO_FRACO:
            tempoEfeito = random(4000, 7000);
            break;
        case EFEITO_CHAMA_ESTAVEL:
            tempoEfeito = random(5000, 10000);
            break;
        case EFEITO_CHAMA_CRESCENDO:
            tempoEfeito = random(2000, 4000);
            break;
        case EFEITO_PISCADAS_RAPIDAS:
            tempoEfeito = random(1000, 2000);
            break;
    }
    
    inicioEfeito = millis();
}

void executarEfeito() {
    switch(efeitoAtual) {
        case EFEITO_RAJADA_EXTREMA:
            if (random(0, 100) < 60) {
                brilhoAlvo = random(50, 100);
                velocidadeMudanca = random(30, 50);
            } else {
                brilhoAlvo = random(100, 160);
                velocidadeMudanca = random(20, 35);
            }
            break;
            
        case EFEITO_VENTO_FORTE:
            if (random(0, 100) < 50) {
                brilhoAlvo = random(80, 140);
                velocidadeMudanca = random(20, 35);
            } else {
                brilhoAlvo = random(160, 210);
                velocidadeMudanca = random(15, 25);
            }
            break;
            
        case EFEITO_VENTO_MEDIO:
            if (random(0, 100) < 40) {
                brilhoAlvo = random(140, 180);
                velocidadeMudanca = random(12, 20);
            } else {
                brilhoAlvo = random(200, 240);
                velocidadeMudanca = random(8, 15);
            }
            break;
            
        case EFEITO_VENTO_FRACO:
            if (random(0, 100) < 70) {
                brilhoAlvo = random(200, 245);
                velocidadeMudanca = random(5, 10);
            } else {
                brilhoAlvo = random(170, 200);
                velocidadeMudanca = random(8, 12);
            }
            break;
            
        case EFEITO_CHAMA_ESTAVEL:
            brilhoAlvo = random(230, 255);
            velocidadeMudanca = random(2, 5);
            break;
            
        case EFEITO_CHAMA_CRESCENDO:
            if (brilhoVela < 240) {
                brilhoAlvo = brilhoVela + random(20, 40);
                if (brilhoAlvo > 255) brilhoAlvo = 255;
                velocidadeMudanca = random(5, 10);
            } else {
                brilhoAlvo = random(230, 255);
                velocidadeMudanca = random(3, 6);
            }
            break;
            
        case EFEITO_PISCADAS_RAPIDAS:
            if (random(0, 100) < 50) {
                brilhoAlvo = random(60, 120);
                velocidadeMudanca = random(40, 60);
            } else {
                brilhoAlvo = random(200, 255);
                velocidadeMudanca = random(40, 60);
            }
            break;
    }
}

void efeitoVela() {
    unsigned long agora = millis();
    
    if (agora - ultimoUpdateVela >= 20) {
        ultimoUpdateVela = agora;
        
        if (rgbLigado) {
            if (velaHabilitada) {
                if (agora - inicioEfeito >= tempoEfeito) {
                    escolherNovoEfeito();
                }
                
                executarEfeito();
                
                if (brilhoVela < brilhoAlvo) {
                    brilhoVela += velocidadeMudanca;
                    if (brilhoVela > brilhoAlvo) brilhoVela = brilhoAlvo;
                } else if (brilhoVela > brilhoAlvo) {
                    brilhoVela -= velocidadeMudanca;
                    if (brilhoVela < brilhoAlvo) brilhoVela = brilhoAlvo;
                }
                
                int tremulacao = random(-10, 11);
                int brilhoFinal = constrain(brilhoVela + tremulacao, 40, 255);
                
                analogWrite(pinVela, brilhoFinal);
            } else {
                analogWrite(pinVela, 255);
            }
        } else {
            analogWrite(pinVela, 0);
            brilhoVela = 200;
            brilhoAlvo = 200;
            escolherNovoEfeito();
        }
    }
}

void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void handleSetRGB() {
    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
        int r = server.arg("r").toInt();
        int g = server.arg("g").toInt();
        int b = server.arg("b").toInt();
        
        setRGB(r, g, b);
        
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Parametros invalidos");
    }
}

void handleToggleVela() {
    if (server.hasArg("estado")) {
        velaHabilitada = (server.arg("estado") == "1");
        server.send(200, "text/plain", velaHabilitada ? "Vela ON" : "Vela OFF");
    } else {
        server.send(400, "text/plain", "Parametro invalido");
    }
}

void handleGetIP() {
    server.send(200, "text/plain", WiFi.localIP().toString());
}

void handleResetWiFi() {
    server.send(200, "text/plain", "Resetando WiFi...");
    delay(1000);
    WiFiManager wm;
    wm.resetSettings();
    ESP.restart();
}

// ========== SETUP ==========

void setup() {
    delay(100);
    
    // Configura os pinos como saída ANTES de tudo
    pinMode(pinVermelho, OUTPUT);
    pinMode(pinVerde, OUTPUT);
    pinMode(pinAzul, OUTPUT);
    pinMode(pinVela, OUTPUT);
    
    // Força TUDO desligado no início
    digitalWrite(pinVermelho, LOW);
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAzul, LOW);
    digitalWrite(pinVela, LOW);
    
    valorR = 0;
    valorG = 0;
    valorB = 0;
    rgbLigado = false;
    
    // WiFiManager - Portal de configuração
    WiFiManager wm;
    
    // Define nome do AP e timeout
    wm.setConfigPortalTimeout(180);
    
    // Tenta conectar. Se falhar, cria AP "ESP-LED-Config"
    if (!wm.autoConnect("ESP-LED-Config")) {
        ESP.restart();
        delay(1000);
    }
    
    // Configura hostname
    WiFi.hostname("esp-led-rgb");
    
    // Configura rotas do servidor
    server.on("/", handleRoot);
    server.on("/setRGB", handleSetRGB);
    server.on("/toggleVela", handleToggleVela);
    server.on("/getIP", handleGetIP);
    server.on("/resetWiFi", handleResetWiFi);
    
    server.begin();
    
    escolherNovoEfeito();
}

// ========== LOOP ==========

void loop() {
    server.handleClient();
    efeitoVela();
}
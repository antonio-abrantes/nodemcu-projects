//===================================================
// SISTEMA DE ALARME COM HC-SR04 + WEB SERVER
// NodeMCU ESP8266
// Placa recomenda na IDE: NodeMCU 0.9 (ESP-12 Module)
//===================================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Configurações WiFi
const char* ssid = "sua-rede-wifi";
const char* password = "sua-senha-wifi";

// IP Fixo
IPAddress ip(192, 168, 100, 147); // Altere para o IP desejado, conforme a faixa do seu roteador
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);

// Servidor Web na porta 80
ESP8266WebServer server(80);

// Variáveis do alarme
int counter;
int alarme_disparado = 0;
volatile int ligado = 0;
volatile int estado = 0;
float distancia = 0;
int disparo = 20; // Distância configurável via web

// Pinos (usando GPIO direto para compatibilidade)
#define pino_trig 14    // D5 = GPIO14
#define pino_echo 12    // D6 = GPIO12
#define pino_led 4      // D2 = GPIO4
#define pino_buzzer 5   // D1 = GPIO5
#define pino_botao 13   // D7 = GPIO13

// Estatísticas
unsigned long ultima_deteccao = 0;
int total_disparos = 0;

//===================================================
// PÁGINA HTML (Design Moderno e Responsivo)
//===================================================
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Alarme Inteligente</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            color: #333;
        }
        
        .container {
            max-width: 600px;
            margin: 0 auto;
        }
        
        .header {
            text-align: center;
            color: white;
            margin-bottom: 30px;
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 5px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }
        
        .header p {
            opacity: 0.9;
            font-size: 1.1em;
        }
        
        .card {
            background: white;
            border-radius: 20px;
            padding: 25px;
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.3);
        }
        
        .status-display {
            text-align: center;
            padding: 30px;
            border-radius: 15px;
            margin-bottom: 20px;
            transition: all 0.3s ease;
        }
        
        .status-display.ativo {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            color: white;
        }
        
        .status-display.desativado {
            background: linear-gradient(135deg, #485563 0%, #29323c 100%);
            color: white;
        }
        
        .status-display.disparado {
            background: linear-gradient(135deg, #ee0979 0%, #ff6a00 100%);
            color: white;
            animation: pulse 1s infinite;
        }
        
        @keyframes pulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.05); }
        }
        
        .status-display h2 {
            font-size: 2em;
            margin-bottom: 10px;
        }
        
        .status-display .icon {
            font-size: 3em;
            margin-bottom: 15px;
        }
        
        .btn {
            width: 100%;
            padding: 18px;
            border: none;
            border-radius: 12px;
            font-size: 1.2em;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-bottom: 15px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.2);
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .btn:active {
            transform: scale(0.98);
        }
        
        .btn-ativar {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            color: white;
        }
        
        .btn-ativar:hover {
            box-shadow: 0 6px 20px rgba(17, 153, 142, 0.4);
        }
        
        .btn-desativar {
            background: linear-gradient(135deg, #ee0979 0%, #ff6a00 100%);
            color: white;
        }
        
        .btn-desativar:hover {
            box-shadow: 0 6px 20px rgba(238, 9, 121, 0.4);
        }
        
        .info-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-top: 20px;
        }
        
        .info-box {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 12px;
            text-align: center;
        }
        
        .info-box .label {
            font-size: 0.9em;
            color: #666;
            margin-bottom: 8px;
        }
        
        .info-box .value {
            font-size: 1.8em;
            font-weight: bold;
            color: #667eea;
        }
        
        .config-section {
            margin-top: 20px;
        }
        
        .config-section h3 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 1.3em;
        }
        
        .input-group {
            display: flex;
            gap: 10px;
            align-items: center;
            margin-bottom: 15px;
        }
        
        .input-group label {
            flex: 1;
            font-weight: 500;
        }
        
        .input-group input {
            flex: 1;
            padding: 12px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 1.1em;
            text-align: center;
        }
        
        .input-group input:focus {
            outline: none;
            border-color: #667eea;
        }
        
        .slider {
            width: 100%;
            height: 8px;
            border-radius: 5px;
            background: #d3d3d3;
            outline: none;
            -webkit-appearance: none;
        }
        
        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
        }
        
        .slider::-moz-range-thumb {
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #667eea;
            cursor: pointer;
        }
        
        .alert {
            background: #fff3cd;
            border-left: 4px solid #ffc107;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
        }
        
        .footer {
            text-align: center;
            color: white;
            margin-top: 30px;
            opacity: 0.8;
            font-size: 0.9em;
        }
        
        @media (max-width: 480px) {
            .header h1 {
                font-size: 2em;
            }
            
            .info-grid {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🔐 Alarme Inteligente</h1>
            <p>Sistema HC-SR04 + ESP8266</p>
        </div>
        
        <div class="card">
            <div id="statusDisplay" class="status-display desativado">
                <div class="icon">⚪</div>
                <h2>DESATIVADO</h2>
                <p>Sistema em espera</p>
            </div>
            
            <button class="btn btn-ativar" onclick="ativarAlarme()">
                🔓 Ativar Alarme
            </button>
            
            <button class="btn btn-desativar" onclick="desativarAlarme()">
                🔒 Desativar Alarme
            </button>
            
            <div class="info-grid">
                <div class="info-box">
                    <div class="label">Distância Atual</div>
                    <div class="value" id="distancia">-- cm</div>
                </div>
                <div class="info-box">
                    <div class="label">Total Disparos</div>
                    <div class="value" id="disparos">0</div>
                </div>
            </div>
        </div>
        
        <div class="card">
            <div class="config-section">
                <h3>⚙️ Configurações</h3>
                
                <div class="input-group">
                    <label>Distância de Disparo:</label>
                    <input type="number" id="distanciaInput" value="20" min="5" max="200">
                    <span>cm</span>
                </div>
                
                <input type="range" class="slider" id="distanciaSlider" 
                       min="5" max="200" value="20" 
                       oninput="document.getElementById('distanciaInput').value = this.value">
                
                <button class="btn btn-ativar" onclick="salvarConfig()" style="margin-top: 15px;">
                    💾 Salvar Configuração
                </button>
            </div>
        </div>
        
        <div class="footer">
            <p>IP: 192.168.100.147 | Atualização automática a cada 2s</p>
        </div>
    </div>
    
    <script>
        let ajustandoConfig = false; // Flag para pausar atualização
        let intervaloAtualizacao; // Variável do intervalo
        
        // Sincroniza slider e input
        document.getElementById('distanciaInput').addEventListener('input', function() {
            document.getElementById('distanciaSlider').value = this.value;
            ajustandoConfig = true; // Pausa atualização automática
        });
        
        document.getElementById('distanciaSlider').addEventListener('input', function() {
            document.getElementById('distanciaInput').value = this.value;
            ajustandoConfig = true; // Pausa atualização automática
        });
        
        // Detecta quando para de ajustar
        document.getElementById('distanciaSlider').addEventListener('mouseup', function() {
            setTimeout(() => { ajustandoConfig = false; }, 1000);
        });
        
        document.getElementById('distanciaSlider').addEventListener('touchend', function() {
            setTimeout(() => { ajustandoConfig = false; }, 1000);
        });
        
        function ativarAlarme() {
            fetch('/ativar')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                    atualizarStatus();
                });
        }
        
        function desativarAlarme() {
            fetch('/desativar')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                    atualizarStatus();
                });
        }
        
        function salvarConfig() {
            const dist = document.getElementById('distanciaInput').value;
            fetch('/config?distancia=' + dist)
                .then(response => response.text())
                .then(data => {
                    // Feedback visual melhorado
                    const btn = event.target;
                    const textoOriginal = btn.innerHTML;
                    btn.innerHTML = '✅ Salvo!';
                    btn.style.background = 'linear-gradient(135deg, #11998e 0%, #38ef7d 100%)';
                    
                    setTimeout(() => {
                        btn.innerHTML = textoOriginal;
                    }, 2000);
                    
                    ajustandoConfig = false; // Retoma atualização
                });
        }
        
        function atualizarStatus() {
            // NÃO atualiza se estiver ajustando configuração
            if (ajustandoConfig) {
                console.log('Atualização pausada - ajustando config');
                return;
            }
            
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    const statusDiv = document.getElementById('statusDisplay');
                    
                    // Atualiza status visual
                    if (data.disparado == 1) {
                        statusDiv.className = 'status-display disparado';
                        statusDiv.innerHTML = `
                            <div class="icon">🚨</div>
                            <h2>ALARME DISPARADO!</h2>
                            <p>Objeto detectado a ${data.distancia} cm</p>
                        `;
                    } else if (data.ligado == 2) {
                        statusDiv.className = 'status-display ativo';
                        statusDiv.innerHTML = `
                            <div class="icon">✅</div>
                            <h2>ATIVO</h2>
                            <p>Monitorando... ${data.distancia} cm</p>
                        `;
                    } else if (data.ligado == 1) {
                        statusDiv.className = 'status-display ativo';
                        statusDiv.innerHTML = `
                            <div class="icon">⏳</div>
                            <h2>ATIVANDO...</h2>
                            <p>Aguarde os 3 bips</p>
                        `;
                    } else {
                        statusDiv.className = 'status-display desativado';
                        statusDiv.innerHTML = `
                            <div class="icon">⚪</div>
                            <h2>DESATIVADO</h2>
                            <p>Sistema em espera</p>
                        `;
                    }
                    
                    // Atualiza informações
                    document.getElementById('distancia').textContent = data.distancia + ' cm';
                    document.getElementById('disparos').textContent = data.total_disparos;
                    
                    // SÓ atualiza slider se NÃO estiver ajustando
                    if (!ajustandoConfig) {
                        document.getElementById('distanciaInput').value = data.config_distancia;
                        document.getElementById('distanciaSlider').value = data.config_distancia;
                    }
                });
        }
        
        // Atualiza a cada 2 segundos
        intervaloAtualizacao = setInterval(atualizarStatus, 2000);
        atualizarStatus(); // Primeira atualização imediata
    </script>
</body>
</html>
)=====";

//===================================================
// FUNÇÕES DO SERVIDOR WEB
//===================================================

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleAtivar() {
  if (ligado == 0) {
    ligado = 1;
    Serial.println(">>> [WEB] Ativando alarme...");
    server.send(200, "text/plain", "Alarme ativando...");
  } else {
    server.send(200, "text/plain", "Alarme ja esta ativo!");
  }
}

void handleDesativar() {
  ligado = 0;
  alarme_disparado = 0;
  estado = 0;
  digitalWrite(pino_led, LOW);
  digitalWrite(pino_buzzer, LOW);
  Serial.println(">>> [WEB] Alarme desativado!");
  server.send(200, "text/plain", "Alarme desativado!");
}

void handleStatus() {
  String json = "{";
  json += "\"ligado\":" + String(ligado) + ",";
  json += "\"disparado\":" + String(alarme_disparado) + ",";
  json += "\"distancia\":" + String((int)distancia) + ",";
  json += "\"config_distancia\":" + String(disparo) + ",";
  json += "\"total_disparos\":" + String(total_disparos);
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleConfig() {
  if (server.hasArg("distancia")) {
    disparo = server.arg("distancia").toInt();
    Serial.print(">>> [WEB] Nova distancia configurada: ");
    Serial.print(disparo);
    Serial.println(" cm");
    server.send(200, "text/plain", "Configuracao salva!");
  } else {
    server.send(400, "text/plain", "Parametro invalido!");
  }
}

//===================================================
// FUNÇÕES DO ALARME
//===================================================

float medirDistancia() {
  digitalWrite(pino_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(pino_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_trig, LOW);
  
  long duracao = pulseIn(pino_echo, HIGH, 30000);
  
  if (duracao > 0) {
    return duracao * 0.034 / 2;
  }
  return -1;
}

void alarmetoca(int tempo) {
  digitalWrite(pino_led, HIGH);
  tone(pino_buzzer, 523, tempo);
  delay(tempo);
  digitalWrite(pino_led, LOW);
  delay(tempo);
}

ICACHE_RAM_ATTR void ativa_desativa() {
  static unsigned long millisatual = millis();
  static unsigned long millisanterior = 0;
  static unsigned long delayEstado;
  
  if ((millis() - delayEstado) > 500) {
    millisatual = millis();
    if (estado == 0) {
      estado = estado + 1;
      millisanterior = millisatual;
      Serial.println("Clique 1/3");
    }
    else if (estado > 0) {
      millisatual = millis();
      if (millisatual - millisanterior <= 1000) {
        estado = estado + 1;
        millisanterior = millisatual;
        Serial.print("Clique ");
        Serial.print(estado);
        Serial.println("/3");
      }
      else {
        estado = 1;
        millisanterior = millisatual;
        Serial.println("Tempo excedido! Reiniciando contagem...");
        Serial.println("Clique 1/3");
      }
    }
    if (estado > 2) {
      estado = 0;
      millisanterior = 0;
      ligado = 0;
      Serial.println("\n>>> ALARME DESATIVADO!\n");
      alarmetoca(100);
      delay(100);
      alarmetoca(100);
    }
    delayEstado = millis();
  }
}

//===================================================
// SETUP
//===================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=========================");
  Serial.println("ALARME HC-SR04 + WEB");
  Serial.println("=========================\n");
  
  // Configura pinos
  pinMode(pino_trig, OUTPUT);
  pinMode(pino_echo, INPUT);
  pinMode(pino_led, OUTPUT);
  pinMode(pino_buzzer, OUTPUT);
  pinMode(pino_botao, INPUT_PULLUP);
  
  digitalWrite(pino_led, LOW);
  digitalWrite(pino_buzzer, LOW);
  
  // Conecta ao WiFi com IP fixo
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n\n✅ WiFi conectado!");
  Serial.print("📡 IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("🌐 Acesse: http://");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  // Configura rotas do servidor
  server.on("/", handleRoot);
  server.on("/ativar", handleAtivar);
  server.on("/desativar", handleDesativar);
  server.on("/status", handleStatus);
  server.on("/config", handleConfig);
  
  server.begin();
  Serial.println("🚀 Servidor Web iniciado!\n");
  
  // Bip de inicialização
  alarmetoca(100);
  delay(100);
  alarmetoca(100);
}

//===================================================
// LOOP PRINCIPAL
//===================================================

void loop() {
  server.handleClient(); // Processa requisições web
  
  // Lógica do alarme (igual ao anterior)
  if (ligado == 0) {
    detachInterrupt(digitalPinToInterrupt(pino_botao));
    alarme_disparado = 0;
    digitalWrite(pino_led, LOW);
    digitalWrite(pino_buzzer, LOW);
    delay(200);
    
    if (digitalRead(pino_botao) == LOW) {
      ligado = 1;
      Serial.println(">>> [BOTÃO] Ativando alarme...");
    }
  }
  
  else if (ligado == 1) {
    Serial.println(">>> Alarme ativando em 3 toques...");
    for (counter = 0; counter < 3; ++counter) {
      alarmetoca(1000);
      delay(1000);
    }
    ligado = 2;
    Serial.println(">>> ALARME ATIVO! Monitorando...\n");
    
    attachInterrupt(digitalPinToInterrupt(pino_botao), ativa_desativa, RISING);
    estado = 0;
  }
  
  else if (ligado == 2) {
    distancia = medirDistancia();
    
    if (distancia > 0 && distancia <= disparo) {
      if (alarme_disparado == 0) {
        Serial.print("!!! ALARME DISPARADO !!! Objeto a ");
        Serial.print(distancia);
        Serial.println(" cm");
        total_disparos++;
        ultima_deteccao = millis();
      }
      alarme_disparado = 1;
    }
    
    if (alarme_disparado == 1) {
      alarmetoca(100);
    }
    else {
      if (distancia > 0) {
        Serial.print("Monitorando... Distância: ");
        Serial.print(distancia);
        Serial.println(" cm");
      }
      delay(200);
    }
  }
}
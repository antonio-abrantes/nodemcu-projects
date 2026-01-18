# 🌐 Alarme v2 - Detecção de Presença com Interface Web

## 📝 Descrição

Sistema de alarme inteligente com detecção de presença utilizando sensor ultrassônico **HC-SR04** e **interface web completa** para controle remoto. Este projeto é uma **evolução do Alarme v1**, aproveitando os recursos de **Wi-Fi do ESP8266** para criar um **Web Server** com controle via navegador.

Além dos componentes físicos (botão, LED, buzzer), você pode **ativar/desativar o alarme, monitorar em tempo real e configurar parâmetros** através de uma interface web moderna e responsiva, acessível de qualquer dispositivo na mesma rede.

**Placa utilizada:** NodeMCU v3 CH340 (ESP8266)

---

## 🎯 Objetivo

Criar um sistema de alarme IoT com:
- ✅ Detecção de presença via sensor ultrassônico
- ✅ **Interface web moderna e responsiva**
- ✅ Controle remoto via Wi-Fi (ativar/desativar)
- ✅ **Configuração dinâmica** da distância de disparo
- ✅ **Monitoramento em tempo real** (atualização a cada 2s)
- ✅ **Estatísticas** (total de disparos)
- ✅ Controle físico com botão (3 cliques para desativar)
- ✅ Feedback visual (LED) e sonoro (buzzer)

---

## 📚 Referências do Projeto Base

Este projeto é uma **evolução** do Alarme v1, que por sua vez é baseado no projeto do **Manual do Mundo**:

- 📺 **Vídeo:** [Como fazer ALARME com Arduino](https://youtu.be/l8RX2wU6XKo)
- 🌐 **Documentação:** [Página do projeto](https://manualmaker.com/projeto/alarme-com-arduino/)
- 🔧 **Simulação:** [Tinkercad](https://www.tinkercad.com/things/aRIHPv8T5iw-alarm-with-ultrasonic-sensor-hc-sr04)

**Novidades da versão v2:**
- 🌐 **Web Server integrado** com interface HTML/CSS/JavaScript
- 📱 **Controle via navegador** (computador, celular, tablet)
- ⚙️ **Configuração em tempo real** da distância de disparo
- 📊 **Dashboard** com estatísticas e monitoramento
- 🎨 **Interface moderna** com design responsivo
- 🔄 **Atualização automática** do status (2 segundos)

---

## Ajuste importante
 - Na seleção de placa, recomendo alterar para **NodeMCU 0.9 (ESP-12 Module)**

## 🔌 Componentes Necessários

| Componente | Quantidade | Observação |
|------------|------------|------------|
| NodeMCU v3 CH340 | 1 | ESP8266 com Wi-Fi |
| Sensor HC-SR04 | 1 | Ultrassônico |
| Buzzer Ativo 5V | 1 | Piezoelétrico |
| LED (qualquer cor) | 1 | Indicador visual |
| Resistor 220Ω | 1 | Para o LED |
| Botão Push Button | 1 | Controle físico |
| Protoboard | 1 | Montagem |
| Jumpers | Vários | Conexões |
| **Rede Wi-Fi** | - | **Essencial!** |

---

## ⚡ Pinagem

### Esquema de Conexões

```
┌─────────────────────────────────────────────────────┐
│            NodeMCU ESP8266 (com Wi-Fi)               │
├─────────────────────────────────────────────────────┤
│                                                      │
│  D1 (GPIO5)  ──────────────► Buzzer (+)             │
│                               Buzzer (-) ──► GND    │
│                                                      │
│  D2 (GPIO4)  ──────┬─────────► LED (ânodo +)        │
│                    └─ 220Ω ──► LED (cátodo -) → GND│
│                                                      │
│  D5 (GPIO14) ──────────────► HC-SR04 (TRIG)         │
│  D6 (GPIO12) ──────────────► HC-SR04 (ECHO)         │
│                               HC-SR04 (VCC)  ──► 5V │
│                               HC-SR04 (GND)  ──► GND│
│                                                      │
│  D7 (GPIO13) ──────────────► Botão (terminal 1)     │
│  GND         ──────────────  Botão (terminal 2)     │
│                              (usa INPUT_PULLUP)      │
│                                                      │
│  5V          ──────────────► Alimentação positiva   │
│  GND         ──────────────► Terra comum            │
│                                                      │
│  [Wi-Fi Integrado] 📶                               │
│                                                      │
└─────────────────────────────────────────────────────┘
```

### Tabela de Conexões

| Componente | Pino Componente | Pino NodeMCU | GPIO | Observação |
|------------|----------------|--------------|------|------------|
| **HC-SR04** | VCC | 5V | - | Alimentação |
| | GND | GND | - | Terra |
| | TRIG | D5 | GPIO14 | Trigger |
| | ECHO | D6 | GPIO12 | Echo |
| **Buzzer** | Positivo (+) | D1 | GPIO5 | Sinal PWM |
| | Negativo (-) | GND | - | Terra |
| **LED** | Ânodo (+) | D2 | GPIO4 | Via resistor 220Ω |
| | Cátodo (-) | GND | - | Terra |
| **Botão** | Terminal 1 | D7 | GPIO13 | INPUT_PULLUP |
| | Terminal 2 | GND | - | Terra |

**A pinagem física é idêntica ao Alarme v1!**

---

## 🛠️ Montagem Física

A montagem é **exatamente igual ao Alarme v1**. Consulte o [README do Alarme v1](../alarme-v1/README.md) para detalhes da montagem física.

---

## ⚙️ Configuração Inicial

### 1. Configurar Credenciais Wi-Fi

**ANTES DE FAZER O UPLOAD**, edite o código nas linhas 9-10:

```cpp
const char* ssid = "sua-rede-wifi";      // ← Nome da sua rede Wi-Fi
const char* password = "sua-senha-wifi";  // ← Senha da sua rede
```

### 2. Configurar IP Fixo (Opcional mas Recomendado)

Edite as linhas 13-15 conforme sua rede:

```cpp
IPAddress ip(192, 168, 100, 147);      // ← IP desejado (ajuste!)
IPAddress gateway(192, 168, 100, 1);   // ← Gateway do seu roteador
IPAddress subnet(255, 255, 255, 0);    // ← Máscara de sub-rede
```

**Como descobrir sua faixa de rede:**
- Windows: `ipconfig` no CMD
- Linux/Mac: `ifconfig` ou `ip addr`
- Procure por "Gateway" ou "Roteador"

**Dica:** Use um IP fora da faixa DHCP do roteador (ex: acima de .100) para evitar conflitos.

---

## 🚀 Como Usar

### Upload e Inicialização:

1. **Configure** as credenciais Wi-Fi no código
2. **Faça o upload** para a NodeMCU
3. **Abra o Monitor Serial** (115200 baud)
4. Aguarde a conexão Wi-Fi

Você verá:
```
=========================
ALARME HC-SR04 + WEB
=========================

Conectando ao WiFi.....

✅ WiFi conectado!
📡 IP: 192.168.100.147
🌐 Acesse: http://192.168.100.147

🚀 Servidor Web iniciado!
```

5. **Acesse** o IP mostrado em qualquer navegador da mesma rede

---

## 🖥️ Interface Web

### Dashboard Principal:

A interface web possui:

#### 🎛️ **Painel de Status**
- **Status visual** com cores:
  - ⚪ **Desativado** - cinza
  - ✅ **Ativo** - verde (monitorando)
  - ⏳ **Ativando** - verde (aguardando 3 bips)
  - 🚨 **Disparado** - vermelho piscante
- **Distância atual** em tempo real

#### 🎮 **Controles**
- Botão **"Ativar Alarme"** - inicia o sistema
- Botão **"Desativar Alarme"** - desliga o sistema
- Desativação instantânea (sem necessidade dos 3 cliques)

#### 📊 **Informações**
- **Distância Atual** - leitura do sensor em cm
- **Total de Disparos** - contador de eventos

#### ⚙️ **Configurações**
- **Slider de distância** - ajusta visualmente (5-200 cm)
- **Campo numérico** - digitação direta da distância
- Botão **"Salvar Configuração"** - aplica mudanças
- Mudanças são **aplicadas imediatamente**

#### 🔄 **Atualização Automática**
- Interface atualiza **automaticamente a cada 2 segundos**
- Sem necessidade de recarregar a página
- Tecnologia AJAX (JavaScript)

---

## 🎮 Controle Físico vs. Web

### Via Botão Físico:

**Ativar:**
- Pressione o botão **1 vez**
- Sistema emite 3 bips e ativa

**Desativar:**
- Pressione **3 cliques rápidos** (< 1 segundo de intervalo)
- Sistema emite 2 bips curtos e desativa

### Via Interface Web:

**Ativar:**
- Clique em **"🔓 Ativar Alarme"**
- Aguarde 3 bips no dispositivo físico
- Status muda para **"ATIVO"**

**Desativar:**
- Clique em **"🔒 Desativar Alarme"**
- Sistema desativa **instantaneamente**
- Emite 2 bips de confirmação

**Vantagem:** Desativação web não requer 3 cliques!

---

## 📱 Acesso Remoto

### Na Mesma Rede Wi-Fi:

Acesse de qualquer dispositivo conectado à mesma rede:

- 💻 **Computador:** http://192.168.100.147 (seu IP)
- 📱 **Celular:** http://192.168.100.147
- 📲 **Tablet:** http://192.168.100.147

### Interface Responsiva:

- ✅ Adapta-se automaticamente ao tamanho da tela
- ✅ Funciona em smartphones, tablets e desktops
- ✅ Touch-friendly (botões grandes)
- ✅ Design moderno com gradientes e animações

---

## ⚙️ Configurações Avançadas

### Mudar Porta do Servidor:

Linha 17:
```cpp
ESP8266WebServer server(80);  // Porta 80 (padrão HTTP)
```

Para usar porta 8080: `ESP8266WebServer server(8080);`  
Acesso: `http://192.168.100.147:8080`

### Intervalo de Atualização Web:

Na interface HTML, linha 436:
```javascript
setInterval(atualizarStatus, 2000);  // 2000ms = 2 segundos
```

Para 5 segundos: `setInterval(atualizarStatus, 5000);`

### Distância Padrão:

Linha 20:
```cpp
int disparo = 20; // Distância inicial em cm
```

---

## 📊 API HTTP (Para Desenvolvedores)

O servidor expõe endpoints REST:

| Endpoint | Método | Função |
|----------|--------|--------|
| `/` | GET | Retorna página HTML principal |
| `/ativar` | GET | Ativa o alarme |
| `/desativar` | GET | Desativa o alarme |
| `/status` | GET | Retorna JSON com status |
| `/config?distancia=X` | GET | Configura distância (X em cm) |

### Exemplo de Status JSON:

```json
{
  "ligado": 2,
  "disparado": 0,
  "distancia": 45,
  "config_distancia": 20,
  "total_disparos": 3
}
```

**Estados do `ligado`:**
- `0` = Desativado
- `1` = Ativando (3 bips)
- `2` = Ativo (monitorando)

---

## 🔧 Troubleshooting

### Problemas de Wi-Fi:

**Não conecta ao Wi-Fi:**
- Verifique SSID e senha no código
- Confirme que a rede é 2.4GHz (ESP8266 não suporta 5GHz)
- Aproxime a NodeMCU do roteador
- Verifique se a rede permite novos dispositivos

**IP não aparece no Serial:**
- Aguarde mais tempo (até 30 segundos)
- Reset na placa e tente novamente
- Verifique LED azul piscando (indica tentativa de conexão)

### Problemas de Acesso Web:

**Não abre a página:**
- Confirme que está na **mesma rede Wi-Fi**
- Tente `http://` (não https://)
- Digite o IP exatamente como mostrado no Serial
- Teste ping: `ping 192.168.100.147` no terminal
- Desative VPN se estiver usando

**Página não atualiza:**
- Limpe cache do navegador (Ctrl+F5)
- Tente outro navegador
- Verifique console JavaScript (F12)

### Problemas de Hardware:

Consulte a seção de troubleshooting do [Alarme v1](../alarme-v1/README.md) - a parte física é idêntica.

---

## 📚 Conceitos Abordados

### Hardware:
- ✅ Sensor ultrassônico HC-SR04
- ✅ Controle de buzzer e LED
- ✅ Interrupções de hardware
- ✅ Debounce de botão

### Software ESP8266:
- ✅ **Conexão Wi-Fi** (`ESP8266WiFi.h`)
- ✅ **Web Server HTTP** (`ESP8266WebServer.h`)
- ✅ **IP estático** (`WiFi.config()`)
- ✅ **Rotas HTTP** (GET requests)
- ✅ **Respostas JSON**
- ✅ **Página HTML embarcada** (PROGMEM)

### Web Development:
- ✅ **HTML5** responsivo
- ✅ **CSS3** moderno (gradientes, animações, grid)
- ✅ **JavaScript** (AJAX, fetch API)
- ✅ **Design responsivo** (mobile-first)
- ✅ **Atualização assíncrona** (sem reload)

---

## 🎓 Próximos Passos

Possíveis melhorias:

- 📧 **Notificações via email** quando alarme dispara
- 📱 **Integração com Telegram** para alertas mobile
- 🔐 **Autenticação** (login/senha)
- 💾 **Histórico de eventos** (salvar em SPIFFS/LittleFS)
- 📈 **Gráficos** de distância ao longo do tempo
- 🌍 **Acesso externo** via DDNS ou Telegram
- 🔊 **Múltiplos tons** de alarme configuráveis
- 📷 **Integração com câmera** ESP32-CAM

---

## 🔄 Diferenças entre v1 e v2

| Recurso | Alarme v1 | Alarme v2 |
|---------|-----------|-----------|
| **Controle Físico** | ✅ Botão | ✅ Botão |
| **Controle Remoto** | ❌ | ✅ Interface Web |
| **Wi-Fi** | ❌ | ✅ Integrado |
| **Configuração Distância** | 📝 Código | 🌐 Interface Web |
| **Monitoramento** | 📟 Serial | 📟 Serial + 🌐 Web |
| **Estatísticas** | ❌ | ✅ Total de disparos |
| **Interface Gráfica** | ❌ | ✅ HTML/CSS/JS |
| **Acesso Multi-dispositivo** | ❌ | ✅ Qualquer navegador |

---

**Desenvolvido com base no projeto do Manual do Mundo**  
**Evolução com recursos IoT do ESP8266** 🌐🚀

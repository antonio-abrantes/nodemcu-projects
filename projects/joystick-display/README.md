# 🕹️ Joystick Display - Monitor de Joystick com Interface Web

## 📝 Descrição

Sistema de teste e monitoramento para **joystick analógico** com visualização em **display OLED SSD1306** e **interface web em tempo real**. Este projeto foi desenvolvido para facilitar o teste e calibração de módulos joystick, permitindo visualizar as leituras tanto localmente no display quanto remotamente via navegador.

O sistema exibe as coordenadas X e Y do joystick, a direção detectada (incluindo diagonais), o estado do botão integrado, além de fornecer uma interface web interativa com representação gráfica do movimento.

**Placa utilizada:** NodeMCU v3 CH340 (ESP8266)

---

## 🎯 Objetivo

Criar uma ferramenta de teste para módulos joystick que:
- ✅ Monitora eixos X e Y do joystick
- ✅ Detecta direções (8 posições + centro)
- ✅ Lê estado do botão integrado
- ✅ Exibe informações em display OLED local
- ✅ Fornece interface web para monitoramento remoto
- ✅ Atualiza dados em tempo real via Wi-Fi
- ✅ Interface gráfica com canvas HTML5

---

## 🔌 Componentes Necessários

| Componente | Quantidade | Observação |
|------------|------------|------------|
| NodeMCU v3 CH340 | 1 | ESP8266 com Wi-Fi |
| Display OLED SSD1306 | 1 | 128x64 pixels, I2C |
| Joystick Analógico | 1 | Módulo de 2 eixos + botão |
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
│  D1 (GPIO5)  ──────────────► OLED SCL (I2C Clock)   │
│  D2 (GPIO4)  ──────────────► OLED SDA (I2C Data)    │
│                               OLED VCC ──────► 3.3V │
│                               OLED GND ──────► GND  │
│                                                      │
│  A0 (ADC)    ──────────────► Joystick VRx (Eixo X)  │
│  D5 (GPIO14) ──────────────► Joystick VRy (Eixo Y)* │
│  D6 (GPIO12) ──────────────► Joystick SW (Botão)    │
│                               Joystick +5V ───► 3.3V│
│                               Joystick GND ───► GND │
│                                                      │
│  3.3V        ──────────────► Alimentação positiva   │
│  GND         ──────────────► Terra comum            │
│                                                      │
│  [Wi-Fi Integrado] 📶                               │
│                                                      │
└─────────────────────────────────────────────────────┘

* Nota: Eixo Y usa leitura digital devido à limitação de 
  apenas 1 ADC no ESP8266. Para melhor precisão, considere
  usar um multiplexador analógico.
```

### Tabela de Conexões

| Componente | Pino Componente | Pino NodeMCU | GPIO | Observação |
|------------|----------------|--------------|------|------------|
| **OLED SSD1306** | VCC | 3.3V | - | Alimentação |
| | GND | GND | - | Terra |
| | SCL | D1 | GPIO5 | I2C Clock |
| | SDA | D2 | GPIO4 | I2C Data |
| **Joystick** | VCC | 3.3V | - | Alimentação ⚠️ |
| | GND | GND | - | Terra |
| | VRx | A0 | ADC0 | Eixo X (analógico) |
| | VRy | D5 | GPIO14 | Eixo Y (digital)* |
| | SW | D6 | GPIO12 | Botão (pull-up) |

⚠️ **Importante**: 
- O joystick é alimentado com **3.3V** para compatibilidade com o ESP8266
- O eixo Y usa leitura **digital** (limitação de 1 ADC no ESP8266)
- Para aplicações que requerem leitura analógica de ambos os eixos, use um multiplexador ADC

---

## ⚙️ Configuração

### 1. Bibliotecas Necessárias

Instale as seguintes bibliotecas no Arduino IDE:

- **Adafruit_GFX** (para gráficos OLED)
- **Adafruit_SSD1306** (driver do display)
- **ESP8266WiFi** (já incluída com a placa)
- **ESP8266WebServer** (já incluída com a placa)

**Como instalar:**
1. Arduino IDE → `Sketch` → `Incluir Biblioteca` → `Gerenciar Bibliotecas...`
2. Pesquise e instale cada biblioteca

### 2. Configuração Wi-Fi

No código, configure suas credenciais de rede:

```cpp
const char* ssid = "sua_rede_wifi";
const char* password = "sua_senha_wifi";
```

Opcionalmente, ajuste o IP fixo:

```cpp
IPAddress ip(192, 168, 100, 147);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
```

### 3. Upload do Código

1. Conecte o NodeMCU via USB
2. Selecione a placa: `Generic ESP8266 Module` ou `NodeMCU 1.0`
3. Selecione a porta COM correta
4. Faça o upload do código

---

## 🚀 Como Usar

### 1. Inicialização

Após o upload, o sistema:
- Conecta-se à rede Wi-Fi configurada
- Exibe o IP no display OLED
- Inicia o servidor web na porta 80

### 2. Monitor Local (Display OLED)

O display mostra em tempo real:
- **IP**: Endereço para acesso web
- **X / Y**: Valores dos eixos (0-1023)
- **Dir**: Direção detectada
- **BTN**: Estado do botão (ON/OFF)

### 3. Interface Web

Acesse o IP mostrado no display através de um navegador:

```
http://192.168.100.147
```

A interface web apresenta:
- 🎮 Canvas interativo com representação visual do joystick
- 📊 Valores numéricos dos eixos X e Y
- 🧭 Direção atual (CENTER, UP, DOWN, LEFT, RIGHT, UL, UR, DL, DR)
- 🔘 Estado do botão
- 🔄 Atualização automática a cada 100ms

### 4. Detecção de Direções

O sistema detecta 9 posições:
- **CENTER**: Joystick em repouso
- **UP, DOWN, LEFT, RIGHT**: Direções cardeais
- **UL, UR, DL, DR**: Diagonais (Up-Left, Up-Right, etc.)

---

## 🛠️ Características Técnicas

### Leituras do Joystick
- **Eixo X**: Leitura analógica via ADC (0-1023)
- **Eixo Y**: Leitura digital (0 ou 1023)*
- **Botão**: Leitura digital com pull-up interno
- **Taxa de atualização**: ~20Hz (50ms)

### Interface Web
- **Protocolo**: HTTP
- **Porta**: 80
- **Atualização**: Polling a cada 100ms
- **Formato de dados**: JSON
- **Canvas**: 400x400 pixels

### Display OLED
- **Resolução**: 128x64 pixels
- **Interface**: I2C (0x3C)
- **Atualização**: ~20Hz sincronizada com leitura

---

## 🔍 Solução de Problemas

### Display não inicializa
- Verifique as conexões SDA/SCL
- Confirme o endereço I2C (0x3C é padrão)
- Teste com um I2C scanner

### Leituras instáveis do joystick
- Adicione capacitores de 100nF próximo ao joystick
- Verifique se está alimentado com 3.3V
- Calibre os valores de threshold no código

### Wi-Fi não conecta
- Verifique SSID e senha
- Confirme que a rede é 2.4GHz (ESP8266 não suporta 5GHz)
- Veja mensagens no Monitor Serial (115200 baud)

---

## 📖 Aprendizados do Projeto

Este projeto demonstra:
- Integração de múltiplos periféricos (I2C, ADC, GPIO)
- Limitação do ESP8266 (apenas 1 ADC)
- Criação de interfaces web responsivas
- Comunicação cliente-servidor em tempo real
- Workaround para leitura de múltiplos sensores analógicos

---

## 🔧 Possíveis Melhorias

- Adicionar multiplexador ADC para ler Y como analógico
- Implementar calibração automática do centro
- Adicionar WebSocket para atualização mais eficiente
- Salvar configurações de calibração na EEPROM
- Implementar modo de gravação de sequências

---

## 📄 Licença

Projeto desenvolvido para fins educacionais e de teste de componentes.

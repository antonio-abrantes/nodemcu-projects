# 🚨 Alarme v1 - Detecção de Presença com Sensor Ultrassônico

## 📝 Descrição

Sistema de alarme com detecção de presença utilizando sensor ultrassônico **HC-SR04**. Este projeto é uma **conversão do projeto original desenvolvido pelo Manual do Mundo**, adaptado para funcionar na placa **NodeMCU ESP8266**.

O alarme detecta objetos que se aproximam a uma distância configurada e dispara um alerta sonoro e visual. O sistema pode ser ativado/desativado através de um botão físico.

**Placa utilizada:** NodeMCU v3 CH340 (ESP8266)

---

## 🎯 Objetivo

Criar um sistema de alarme de presença funcional que:
- Detecta objetos através de sensor ultrassônico
- Emite alerta sonoro com buzzer
- Sinaliza visualmente com LED
- Permite ativação/desativação por botão físico
- Requer 3 cliques rápidos no botão para desativar (segurança)

---

## 📚 Referências do Projeto Original

Este projeto é baseado no trabalho do **Manual do Mundo**:

- 📺 **Vídeo:** [Como fazer ALARME com Arduino](https://youtu.be/l8RX2wU6XKo)
- 🌐 **Documentação:** [Página do projeto](https://manualmaker.com/projeto/alarme-com-arduino/)
- 🔧 **Simulação Tinkercad:** [Alarme com HC-SR04](https://www.tinkercad.com/things/aRIHPv8T5iw-alarm-with-ultrasonic-sensor-hc-sr04)

**Diferenças desta versão:**
- ✅ Adaptado para NodeMCU ESP8266
- ✅ Usa mapeamento de pinos GPIO do ESP8266
- ✅ Sem sensor magnético (apenas ultrassônico)
- ✅ Monitor Serial com informações detalhadas

---

## 🔌 Componentes Necessários

| Componente | Quantidade | Observação |
|------------|------------|------------|
| NodeMCU v3 CH340 | 1 | ESP8266 |
| Sensor HC-SR04 | 1 | Ultrassônico |
| Buzzer Ativo 5V | 1 | Piezoelétrico |
| LED (qualquer cor) | 1 | Indicador visual |
| Resistor 220Ω | 1 | Para o LED |
| Botão Push Button | 1 | Ativação/desativação |
| Resistor 10kΩ | 1 | Pull-down do botão |
| Protoboard | 1 | Montagem |
| Jumpers | Vários | Conexões |

---

## ⚡ Pinagem

### Esquema de Conexões

```
┌─────────────────────────────────────────────────────┐
│                   NodeMCU ESP8266                    │
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
│  GND         ──────┬─────── Botão (terminal 2)      │
│                    └─ 10kΩ ─► (pull-down)           │
│                                                      │
│  5V          ──────────────► Alimentação positiva   │
│  GND         ──────────────► Terra comum            │
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

---

## 🛠️ Montagem Física

### Passo a Passo:

1. **HC-SR04 (Sensor Ultrassônico)**
   - VCC → 5V da NodeMCU
   - GND → GND da NodeMCU
   - TRIG → D5 (GPIO14)
   - ECHO → D6 (GPIO12)

2. **LED Indicador**
   - Ânodo (perna longa) → Resistor 220Ω → D2 (GPIO4)
   - Cátodo (perna curta) → GND

3. **Buzzer**
   - Terminal positivo (+) → D1 (GPIO5)
   - Terminal negativo (-) → GND

4. **Botão de Controle**
   - Um terminal → D7 (GPIO13)
   - Outro terminal → GND
   - O código usa INPUT_PULLUP (resistor interno)

---

## 🎮 Como Usar

### Ativando o Alarme:

1. **Faça o upload** do código para a NodeMCU
2. **Abra o Monitor Serial** (115200 baud)
3. **Pressione o botão** uma vez
4. O sistema emitirá **3 bips** indicando ativação
5. Mensagem no Serial: `"ALARME ATIVO! Monitorando..."`
6. O alarme agora está **detectando objetos**

### Funcionamento:

- Quando um objeto se aproxima a **menos de 20 cm**, o alarme dispara
- **LED pisca** e **buzzer toca** continuamente
- **Monitor Serial** mostra a distância detectada

### Desativando o Alarme:

⚠️ **Importante:** Para desativar, é necessário fazer **3 cliques rápidos** no botão (intervalo máximo de 1 segundo entre cliques)

1. Clique no botão → "Clique 1/3"
2. Clique novamente (em até 1 segundo) → "Clique 2/3"
3. Clique novamente (em até 1 segundo) → "Clique 3/3"
4. Sistema emite **2 bips curtos** confirmando desativação
5. Mensagem: `"ALARME DESATIVADO!"`

---

## ⚙️ Configurações

### Ajustar Distância de Disparo:

No código, linha 15:
```cpp
#define disparo 20  // Distância em centímetros
```

Altere o valor `20` para a distância desejada (5 a 200 cm).

### Alterar Quantidade de Cliques para Desativar:

No código, linha 183:
```cpp
if (estado > 2){  // 2 = 3 cliques (inicia em 0)
```

Para 5 cliques, altere para: `if (estado > 4){`

---

## 📊 Informações do Monitor Serial

O sistema exibe informações em tempo real:

```
=========================
ALARME HC-SR04 INICIADO
=========================

>>> Ativando alarme...
>>> Alarme ativando em 3 toques...
>>> ALARME ATIVO! Monitorando...

Monitorando... Distância: 45 cm
Monitorando... Distância: 32 cm
Monitorando... Distância: 18 cm
!!! ALARME DISPARADO !!! Objeto detectado a 18 cm

Clique 1/3
Clique 2/3
Clique 3/3

>>> ALARME DESATIVADO!
```

---

## 🔧 Troubleshooting

**Sensor não detecta distância:**
- Verifique alimentação 5V do HC-SR04
- Confirme conexões TRIG (D5) e ECHO (D6)
- Mantenha sensor estável, sem vibração
- Teste com objeto grande e plano (parede, livro)

**Buzzer não toca:**
- Verifique polaridade (+ no D1, - no GND)
- Teste se é buzzer ativo (tem oscilador interno)
- Tente outro pino GPIO

**Botão não responde:**
- Confirme conexão D7 e GND
- Teste sem resistor (código usa INPUT_PULLUP)
- Cliques devem ser rápidos (< 1 segundo de intervalo)

**LED não acende:**
- Verifique polaridade (perna longa no resistor)
- Confirme resistor 220Ω instalado
- Teste LED com multímetro

---

## 📚 Conceitos Abordados

- ✅ Leitura de sensor ultrassônico HC-SR04
- ✅ Cálculo de distância por tempo de pulso
- ✅ Controle de buzzer com PWM (`tone()`)
- ✅ Uso de interrupções (`attachInterrupt()`)
- ✅ Debounce de botão
- ✅ Máquina de estados simples
- ✅ Comunicação Serial

---

## 🎓 Próximos Passos

Após dominar este projeto, experimente:
- 🔄 **[Alarme v2](../alarme-v2/)** - Versão com controle via Web Server Wi-Fi
- 📱 Adicionar notificação via Telegram
- 💾 Salvar log de detecções em cartão SD
- 🔋 Modo de economia de energia (deep sleep)

---

**Desenvolvido com base no projeto do Manual do Mundo**  
**Adaptado para ESP8266 para fins educacionais** 🎓

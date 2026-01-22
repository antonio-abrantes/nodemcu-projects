# 🔧 Servo Test - Testador de Servo Motores com Interface Web

## 📝 Descrição

Sistema de teste e controle para **servo motores** com **interface web interativa**. Este projeto foi desenvolvido para facilitar o teste de servos SG90 e similares no ESP8266, permitindo controle preciso do ângulo, detecção do tipo de servo (180° ou 360°), e testes de varredura através de uma interface web moderna.

O projeto utiliza **PWM direto** (sem biblioteca Servo) com parâmetros otimizados para compensar a tensão de 3.3V do ESP8266, garantindo melhor compatibilidade com servos que normalmente operam em 5V.

**Placa utilizada:** NodeMCU v3 CH340 (ESP8266)

---

## 🎯 Objetivo

Criar uma ferramenta de teste para servo motores que:
- ✅ Controla o ângulo do servo (0-180°)
- ✅ Detecta automaticamente o tipo (180° ou 360°)
- ✅ Interface web completa para controle remoto
- ✅ Teste de varredura automático
- ✅ Controle via slider e botões rápidos
- ✅ PWM otimizado para 3.3V do ESP8266
- ✅ Suporte para diferentes pinos GPIO

---

## 🔌 Componentes Necessários

| Componente | Quantidade | Observação |
|------------|------------|------------|
| NodeMCU v3 CH340 | 1 | ESP8266 com Wi-Fi |
| Servo Motor | 1 | SG90 ou similar |
| Protoboard | 1 | Montagem (opcional) |
| Jumpers | 3 | Conexões |
| **Rede Wi-Fi** | - | **Essencial!** |

---

## ⚡ Pinagem

### Esquema de Conexões

```
┌─────────────────────────────────────────────────────┐
│            NodeMCU ESP8266 (com Wi-Fi)               │
├─────────────────────────────────────────────────────┤
│                                                      │
│  D1 (GPIO5)  ──────────────► Servo Sinal (amarelo)  │
│                                                      │
│  VUSB (5V)   ──────────────► Servo VCC (vermelho)   │
│                              [TESTE 1 - PREFERENCIAL]│
│                                                      │
│  3.3V        ──────────────► Servo VCC (vermelho)   │
│                              [TESTE 2 - SE NÃO      │
│                               FUNCIONAR COM 5V]      │
│                                                      │
│  GND         ──────────────► Servo GND (marrom)     │
│                                                      │
│  [Wi-Fi Integrado] 📶                               │
│                                                      │
└─────────────────────────────────────────────────────┘
```

### Tabela de Conexões

| Componente | Fio Servo | Pino NodeMCU | GPIO | Observação |
|------------|-----------|--------------|------|------------|
| **Servo Motor** | Sinal (amarelo/laranja) | D1 | GPIO5 | PWM |
| | VCC (vermelho) | VUSB | - | 5V (teste 1) |
| | VCC (vermelho) | 3.3V | - | 3.3V (teste 2) |
| | GND (marrom/preto) | GND | - | Terra |

⚠️ **Importante**: 
- **TESTE 1**: Conecte o VCC do servo no pino **VUSB (5V)** - esta é a configuração preferencial
- **TESTE 2**: Se não funcionar, conecte o VCC no pino **3.3V**
- O código usa PWM direto com pulsos estendidos (400-2600µs) para melhor compatibilidade com 3.3V
- O pino D1 (GPIO5) é recomendado, mas você pode testar outros pinos alterando a constante `SERVO_PIN` no código

---

## ⚙️ Configuração

### 1. Bibliotecas Necessárias

O projeto usa apenas bibliotecas nativas do ESP8266:

- **ESP8266WiFi** (já incluída com a placa)
- **ESP8266WebServer** (já incluída com a placa)

**Nenhuma biblioteca adicional é necessária!** 🎉

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

### 3. Seleção do Pino (Opcional)

No código, você pode testar diferentes pinos GPIO descomentando a linha desejada:

```cpp
const int SERVO_PIN = 5;   // D1 (GPIO5) - RECOMENDADO
// const int SERVO_PIN = 4;   // D2 (GPIO4)
// const int SERVO_PIN = 0;   // D3 (GPIO0)
// const int SERVO_PIN = 2;   // D4 (GPIO2)
```

### 4. Upload do Código

1. Conecte o NodeMCU via USB
2. Selecione a placa: `Generic ESP8266 Module` ou `NodeMCU 1.0`
3. Selecione a porta COM correta
4. Faça o upload do código

---

## 🚀 Como Usar

### 1. Inicialização

Após o upload:
- O ESP8266 conecta-se à rede Wi-Fi
- O servo é posicionado em 90° (centro)
- O Monitor Serial exibe o IP de acesso

### 2. Interface Web

Acesse o IP através de um navegador:

```
http://192.168.100.147
```

A interface apresenta:

#### 🔍 Botão de Detecção
- **DETECTAR TIPO DO SERVO**: Executa teste automático de 10 segundos
- Identifica se é servo de 180° ou 360° (rotação contínua)
- Atualiza o status automaticamente

#### 🎚️ Controle Manual
- **Slider**: Controle preciso de 0 a 180 graus
- **Display grande**: Mostra o ângulo atual em tempo real
- Atualização imediata do servo ao mover o slider

#### ⚡ Botões Rápidos
- **0°, 45°, 90°, 135°, 180°**: Posições pré-definidas
- **Varredura**: Movimento automático de 0° a 180° e volta (5 segundos)

#### 📊 Status
- **Tipo**: Mostra o tipo detectado (180°, 360° ou Testando...)
- **Ângulo Atual**: Posição atual do servo
- Cores diferentes para cada tipo detectado

---

## 🛠️ Características Técnicas

### PWM Direto
- **Frequência**: 50Hz (período de 20ms)
- **Largura de pulso**: 400-2600µs (estendido para compensar 3.3V)
- **Resolução**: 180 posições
- **Número de pulsos**: 50 por comando (aproximadamente 1 segundo)

### Detecção de Tipo
O sistema testa automaticamente:
1. Move para 0°, aguarda, verifica comportamento
2. Move para 90°, aguarda, verifica comportamento
3. Move para 180°, aguarda, verifica comportamento
4. Analisa o padrão de movimento
5. Classifica como servo de 180° ou 360°

### Interface Web
- **Design responsivo**: Funciona em desktop e mobile
- **Atualização em tempo real**: Status atualizado após cada comando
- **Feedback visual**: Cores indicando tipo de servo
- **Controles intuitivos**: Slider, botões e varredura

---

## 🔍 Solução de Problemas

### Servo não se move ou movimento limitado
1. **TESTE 1**: Conecte VCC no pino **VUSB (5V)** - esta deve ser a primeira tentativa
2. **TESTE 2**: Se não funcionar, tente VCC no pino **3.3V**
3. Verifique se o fio de sinal está firmemente conectado no D1
4. Confirme que o GND está conectado corretamente
5. Teste outro pino GPIO alterando `SERVO_PIN` no código

### Servo treme ou vibra
- Normal durante a geração de pulsos PWM
- Indique que está recebendo o sinal corretamente
- Se tremer excessivamente, verifique a alimentação

### Wi-Fi não conecta
- Verifique SSID e senha no código
- Confirme que é rede 2.4GHz (ESP8266 não suporta 5GHz)
- Veja mensagens no Monitor Serial (115200 baud)

### Detecção de tipo incorreta
- Servos de 360° (rotação contínua) não param em posições fixas
- Servos de 180° devem parar claramente em cada ângulo
- Se o servo não responder bem, pode ser problema de alimentação

---

## 📖 Aprendizados do Projeto

Este projeto demonstra:
- Geração de PWM manual no ESP8266
- Adaptação de sinais de controle para diferentes tensões
- Interface web com controle em tempo real
- Detecção automática de características de hardware
- Otimização de parâmetros para compatibilidade

---

## 🔧 Possíveis Melhorias

- Adicionar suporte para múltiplos servos simultâneos
- Implementar calibração personalizada de pulsos
- Salvar configurações de calibração na EEPROM
- Adicionar WebSocket para atualização mais eficiente
- Criar sequências de movimento programáveis
- Implementar feedback de corrente para detectar problemas

---

## 💡 Dicas Técnicas

### Por que PWM direto?
A biblioteca Servo padrão pode ter problemas no ESP8266 devido a conflitos com o Wi-Fi. O PWM direto oferece:
- Melhor controle sobre os parâmetros
- Sem conflitos com Wi-Fi
- Possibilidade de ajuste fino para 3.3V

### Por que pulsos estendidos (400-2600µs)?
Servos normalmente esperam 500-2500µs em 5V. Com 3.3V, os pulsos mais curtos podem não ser detectados corretamente. Os valores estendidos compensam essa diferença.

### Alimentação do Servo
- **VUSB (5V)**: Melhor para a maioria dos servos SG90
- **3.3V**: Alternativa se 5V não funcionar (servo pode ter menor torque)
- **Sinal sempre 3.3V**: O pino GPIO do ESP8266 gera sinal de 3.3V (suficiente para a maioria dos servos)

---

## 📄 Licença

Projeto desenvolvido para fins educacionais e de teste de componentes.

//===================================================
// SISTEMA DE ALARME COM HC-SR04 - NodeMCU ESP8266
//===================================================

//Define variáveis que serão utilizadas
int counter;
int alarme_disparado;
volatile int ligado = 0;
volatile int estado;
float distancia;

//Pinos utilizados para HC-SR04, led, buzzer e botão
#define pino_trig D5      // GPIO14 - Trigger do HC-SR04
#define pino_echo D6      // GPIO12 - Echo do HC-SR04
#define pino_led D2       // GPIO4  - LED
#define pino_buzzer D1    // GPIO5  - Buzzer
#define pino_botao D7     // GPIO13 - Botão (uso com interrupt)

//Define distância de disparo do alarme (em centímetros)
#define disparo 20

void setup()
{
  //inicia monitor serial
  Serial.begin(115200);
  Serial.println("\n=========================");
  Serial.println("ALARME HC-SR04 INICIADO");
  Serial.println("=========================\n");
  
  //Define os pinos de entrada e saída
  pinMode(pino_trig, OUTPUT);
  pinMode(pino_echo, INPUT);
  pinMode(pino_led, OUTPUT);
  pinMode(pino_buzzer, OUTPUT);
  pinMode(pino_botao, INPUT_PULLUP);
  
  //Inicia variável do alarme como não disparado
  alarme_disparado = 0;
  
  //Garante que LED e buzzer começam desligados
  digitalWrite(pino_led, LOW);
  digitalWrite(pino_buzzer, LOW);
}

void loop()
{
  //Verifica se botão foi pressionado para ligar o alarme ou se programa voltou a estado inicial
  if (ligado == 0){
    detachInterrupt(digitalPinToInterrupt(pino_botao));
    alarme_disparado = 0;
    digitalWrite(pino_led, LOW);
    digitalWrite(pino_buzzer, LOW);
    delay(200);
    
    if (digitalRead(pino_botao) == LOW){
      ligado = 1;
      Serial.println(">>> Ativando alarme...");
    }
  }
  
  else if(ligado == 1){
    //Caso ligado toca aviso de início do alarme. 3x
    Serial.println(">>> Alarme ativando em 3 toques...");
    for (counter = 0; counter < 3; ++counter){
      //Os números definem respectivamente tempo de duração do som e intervalo entre sons.
      alarmetoca(1000);
      delay(1000);
    }
    ligado = 2;
    Serial.println(">>> ALARME ATIVO! Monitorando...\n");
    
    //Inicia função de interrupção no botão para que possa cancelar o alarme
    attachInterrupt(digitalPinToInterrupt(pino_botao), ativa_desativa, RISING);
    estado = 0;
  }
  
  //Rotinas após alarme estar ativo
  else if (ligado == 2){
    //Mede a distância usando o HC-SR04
    distancia = medirDistancia();
    
    //Verifica se objeto está dentro da distância de disparo
    if (distancia > 0 && distancia <= disparo) {
      if (alarme_disparado == 0) {
        Serial.print("!!! ALARME DISPARADO !!! Objeto detectado a ");
        Serial.print(distancia);
        Serial.println(" cm");
      }
      alarme_disparado = 1;
    }
    
    //Toca o alarme caso ativado
    if (alarme_disparado == 1){
      alarmetoca(100);
    }
    else {
      //Mostra distância no monitor serial quando em modo ativo
      if (distancia > 0) {
        Serial.print("Monitorando... Distância: ");
        Serial.print(distancia);
        Serial.println(" cm");
      }
      delay(200); // Delay entre medições quando não disparado
    }
  }
}

//===================================================
// FUNÇÃO PARA MEDIR DISTÂNCIA COM HC-SR04
//===================================================
float medirDistancia() {
  // Limpa o trigger
  digitalWrite(pino_trig, LOW);
  delayMicroseconds(2);
  
  // Envia pulso de 10 microsegundos
  digitalWrite(pino_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pino_trig, LOW);
  
  // Lê o tempo de retorno (timeout de 30ms = ~5 metros máximo)
  long duracao = pulseIn(pino_echo, HIGH, 30000);
  
  // Calcula e retorna distância em centímetros
  if (duracao > 0) {
    float dist = duracao * 0.034 / 2;
    return dist;
  }
  else {
    return -1; // Retorna -1 se não detectou nada
  }
}

//===================================================
// FUNÇÃO DO TOQUE DO ALARME
//===================================================
void alarmetoca(int tempo){
  //liga o LED
  digitalWrite(pino_led, HIGH);
  //Toca o buzzer (pino, frequência, tempo ligado)
  tone(pino_buzzer, 523, tempo);
  //Aguarda tempo para desligar o LED
  delay(tempo);
  //Desliga o LED
  digitalWrite(pino_led, LOW);
  delay(tempo);
}

//===================================================
// FUNÇÃO DE INTERRUPÇÃO DO BOTÃO
// Cancela o alarme - retorna para estado desativado
//===================================================
ICACHE_RAM_ATTR void ativa_desativa(){
  static unsigned long millisatual = millis();
  static unsigned long millisanterior = 0;
  static unsigned long delayEstado;
  
  //Define tempo mínimo entre cliques para evitar problemas de contato
  if ((millis() - delayEstado) > 500){
    millisatual = millis();
    if (estado == 0){
      estado = estado + 1;
      millisanterior = millisatual;
      Serial.println("Clique 1/3");
    }
    else if (estado > 0){
      millisatual = millis();
      //Define tempo máximo entre cliques (1 segundo)
      if (millisatual - millisanterior <= 1000){
        estado = estado + 1;
        millisanterior = millisatual;
        Serial.print("Clique ");
        Serial.print(estado);
        Serial.println("/3");
      }
      else{
        estado = 1;
        millisanterior = millisatual;
        Serial.println("Tempo excedido! Reiniciando contagem...");
        Serial.println("Clique 1/3");
      }
    }
    //Define quantidade de cliques necessários (3 cliques)
    if (estado > 2){
      estado = 0;
      millisanterior = 0;
      ligado = 0;
      Serial.println("\n>>> ALARME DESATIVADO!\n");
      //aviso sonoro da desativação
      alarmetoca(100);
      delay(100);
      alarmetoca(100);
    }
    delayEstado = millis();
  }
}
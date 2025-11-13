/*
 * CÓDIGO FINAL - Alarme de Proximidade (Versão 5V)
 *
 * Este código assume que o divisor de tensão (1kΩ/2kΩ)
 * está instalado corretamente no pino Echo.
 *
 * PINOUT:
 * - Sensor VCC:  VIN (5V)
 * - Sensor Trig: D13
 * - Sensor Echo: D12 (COM DIVISOR DE TENSÃO)
 * - Sensor GND:  GND
 * * - Buzzer (+):  D4  (Buzzer ATIVO)
 * - Buzzer (-):  GND
 */

// Pinos dos componentes
const int trigPin = 2;
const int echoPin = 4;
const int buzzerPin = 26;

// Distância em CM para o alarme disparar
const int LIMITE_DISTANCIA_CM = 20;

// Variáveis para o cálculo
long duration;
int distance;

void setup() {
  // Opcional: Inicia o Monitor Serial para ver os números
  Serial.begin(115200);
  
  // Define os modos dos pinos
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Garante que o buzzer (ativo) comece desligado
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // 1. Faz a leitura da distância
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  // 2. Imprime no Monitor Serial (ótimo para debug)
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // 3. Lógica do Alarme (para buzzer ATIVO)
  if (distance < LIMITE_DISTANCIA_CM && distance > 0) {
    // ALARME ATIVADO
    digitalWrite(buzzerPin, HIGH); // Liga o buzzer
  } else {
    // ALARME DESATIVADO
    digitalWrite(buzzerPin, LOW); // Desliga o buzzer
  }
  
  // Pausa antes da próxima leitura
  delay(100); 
}
/*
 * CÓDIGO FINAL - DESAFIO EXTRA (Lógica Gradativa de 4 Zonas)
 * Hardware: ESP32, Sensor HC-SR04, Buzzer
 * Ação: Loga dados no Firebase com resposta gradativa do buzzer.
 *
 * PINAGEM (conforme seu último código):
 * - Sensor Trig: D2
 * - Sensor Echo: D4
 * - Buzzer (+):  D26
 */

// Bibliotecas necessárias
#include <WiFi.h>
#include <FirebaseESP32.h>

// --- 1. CONFIGURAÇÃO (PREENCHA SÓ O WI-FI) ---
#define WIFI_SSID "Luis-Gustavo's-Galaxy-S21-FE" // <-- PREENCHA AQUI
#define WIFI_PASSWORD "wpvn6161"  // <-- PREENCHA AQUI

// Credenciais do Firebase (JÁ PREENCHIDAS)
#define FIREBASE_HOST "detector-distanciaesp32-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "dBfZaKDrBPmDqttQBPhrh3ZQhLjlEENVMn5HJrNP" 

// Pinos dos componentes (ATUALIZADOS)
const int trigPin = 2;
const int echoPin = 4;
const int buzzerPin = 26;

// Variáveis para o cálculo
long duration;
float distance; // Usar float para mais precisão

// Objetos do Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variável para armazenar a ação disparada (conforme PDF)
String acaoDisparada = "INICIALIZANDO";
unsigned long idEpisodio = 0; // Contador para o ID

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW); // Garante que comece desligado

  // --- 2. CONECTAR AO WI-FI ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConectado!");

  // --- 3. CONECTAR AO FIREBASE ---
  config.host = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // 1. Faz a leitura da distância
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2; // Percepção D(t)

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- 4. LÓGICA R.C.A. GRADATIVA (O que você pediu) ---
  // Baseado na tabela "Regras Condição-Ação" do PDF 
  
  if (distance > 50) {
    // Zona Segura 
    acaoDisparada = "BUZZER_OFF_SEGURO";
    digitalWrite(buzzerPin, LOW); // Buzzer Desligado 
    
  } else if (distance > 20 && distance <= 50) {
    // Zona de Atenção 
    acaoDisparada = "BUZZER_OFF_ATENCAO";
    digitalWrite(buzzerPin, LOW); // Buzzer Desligado 
    
  } else if (distance > 10 && distance <= 20) {
    // Zona de Alerta 
    acaoDisparada = "BUZZER_PULSANTE_LENTO";
    digitalWrite(buzzerPin, HIGH); // Buzzer Pulsando 
    delay(80); // Duração do pulso
    digitalWrite(buzzerPin, LOW);
    
  } else if (distance <= 10 && distance > 0) {
    // Zona Crítica 
    acaoDisparada = "BUZZER_MAXIMO_CONTINUO";
    digitalWrite(buzzerPin, HIGH); // Buzzer Contínuo [cite: 9]
    
  } else {
    // Fora de alcance ou erro
    acaoDisparada = "ERRO_LEITURA";
    digitalWrite(buzzerPin, LOW);
  }

  // --- 5. ARMAZENAR DADOS NA NUVEM (Implementa o Desafio Extra) ---
  
  idEpisodio++; // Incrementa o ID do episódio [cite: 35]
  
  // Cria o objeto JSON para enviar (conforme seus "Dados a serem Armazenados")
  FirebaseJson json;
  json.set("timestamp", ".sv", "timestamp"); // [cite: 24, 28]
  json.set("distancia_cm", distance);        // [cite: 29, 31]
  json.set("acao_disparada", acaoDisparada);  // [cite: 32, 34]
  json.set("id_episodio", (int)idEpisodio);  // [cite: 35, 37]

  // Envia (set) o JSON para um "nó" usando o id_episodio como chave
  String nodePath = "logs_sensor/" + String(idEpisodio);
  
  Serial.println("Enviando dados para o Firebase...");
  if (Firebase.setJSON(fbdo, nodePath, json)) {
    Serial.println("DADOS ENVIADOS COM SUCESSO!");
  } else {
    Serial.println("ERRO AO ENVIAR DADOS:");
    Serial.println(fbdo.errorReason());
  }
  
  // Pausa antes da próxima leitura.
  // Este delay controla o "espaço" entre os pulsos.
  delay(420); 
}

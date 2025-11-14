/*
 * CÓDIGO FINAL - DESAFIO EXTRA (Lógica Gradativa de 4 Zonas)
 * Hardware: ESP32, Sensor HC-SR04, Buzzer
 * Status: CORRIGIDO (Setup fechado e Firebase Configurado)
 */

// Bibliotecas necessárias
#include <WiFi.h>
#include <FirebaseESP32.h>

// --- 1. CONFIGURAÇÃO ---
#define WIFI_SSID "Luis-Gustavo's-Galaxy-S21-FE" 
#define WIFI_PASSWORD "wpvn6161" 

// Credenciais do Firebase
#define FIREBASE_HOST "detector-distanciaesp32-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "dBfZaKDrBPmDqttQBPhrh3ZQhLjlEENVMn5HJrNP" 

// Pinos dos componentes
const int trigPin = 2;
const int echoPin = 4;
const int buzzerPin = 26;

// Variáveis para o cálculo
long duration;
float distance; 

// Objetos do Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variável para armazenar a ação disparada
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

  // --- 3. CONECTAR AO FIREBASE (Versão Corrigida) ---
  config.database_url = FIREBASE_HOST;           // URL do banco
  config.signer.tokens.legacy_token = FIREBASE_AUTH; // Token/Segredo do banco
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

} // <--- AQUI ESTÁ A CHAVE MÁGICA QUE FALTAVA!

void loop() {
  // 1. Faz a leitura da distância
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2; 

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- 4. LÓGICA R.C.A. GRADATIVA ---
  
  if (distance > 50) {
    // Zona Segura 
    acaoDisparada = "BUZZER_OFF_SEGURO";
    digitalWrite(buzzerPin, LOW); 
    
  } else if (distance > 20 && distance <= 50) {
    // Zona de Atenção 
    acaoDisparada = "BUZZER_OFF_ATENCAO";
    digitalWrite(buzzerPin, LOW); 
    
  } else if (distance > 10 && distance <= 20) {
    // Zona de Alerta 
    acaoDisparada = "BUZZER_PULSANTE_LENTO";
    digitalWrite(buzzerPin, HIGH); 
    delay(80); // Duração do pulso
    digitalWrite(buzzerPin, LOW);
    
  } else if (distance <= 10 && distance > 0) {
    // Zona Crítica 
    acaoDisparada = "BUZZER_MAXIMO_CONTINUO";
    digitalWrite(buzzerPin, HIGH); 
    
  } else {
    // Fora de alcance ou erro
    acaoDisparada = "ERRO_LEITURA";
    digitalWrite(buzzerPin, LOW);
  }

  // --- 5. ARMAZENAR DADOS NA NUVEM ---
  
  idEpisodio++; 
  
  FirebaseJson json;
  
  // Correção para o Timestamp do Firebase
  FirebaseJson timestampVar;        
  timestampVar.set(".sv", "timestamp"); 
  json.set("timestamp", timestampVar);  
  
  json.set("distancia_cm", distance);        
  json.set("acao_disparada", acaoDisparada);  
  json.set("id_episodio", (int)idEpisodio);  

  // Envia o JSON
  String nodePath = "logs_sensor/" + String(idEpisodio);
  
  Serial.println("Enviando dados para o Firebase...");
  if (Firebase.setJSON(fbdo, nodePath, json)) {
    Serial.println("DADOS ENVIADOS COM SUCESSO!");
  } else {
    Serial.println("ERRO AO ENVIAR DADOS:");
    Serial.println(fbdo.errorReason());
  }
  
  // Pausa antes da próxima leitura
  delay(420); 
}
#include <WiFi.h>
#include <ThingSpeak.h>

// Wi-Fi
const char* ssid = "Rede_LAB_SE9";
const char* password = "ricardofranco_se9@ime";

// const char* ssid = "GGP Oi Fibra";
// const char* password = "apartamentoime25";


// ThingSpeak
const char* writeAPIKey = "1ZUEHUF2T0C9Q228";
const long channelID = 2974048;
WiFiClient client;

// Pinos
const int touchPin = 4;
const int buzzerPin = 13;

// Notas
#define NOTE_DO  262
#define NOTE_RE  294
#define NOTE_MI  330
#define NOTE_FA  349
#define NOTE_SOL  392
#define NOTE_LA  440
#define NOTE_SI  494

int hino[] = {
  NOTE_MI, NOTE_MI, NOTE_FA, NOTE_MI, NOTE_SOL, NOTE_RE,
  NOTE_MI, NOTE_SOL, NOTE_MI, NOTE_RE, NOTE_DO, NOTE_DO
};

int duracoes[] = {
  2, 2, 4, 2, 4, 8,
  2, 3, 1, 2, 8, 2
};

void tocarHinoFlamengo() {
  for (int i = 0; i < sizeof(hino) / sizeof(int); i++) {
    int duracao = duracoes[i]*100;
    tone(buzzerPin, hino[i], duracao);
    delay(duracao * 1.3);
    noTone(buzzerPin);
  }
}

void tocarCampainha() {
  tone(buzzerPin, 1000, 500);  // 1000 Hz por 500ms
  delay(600);                  // Pequena pausa
  tone(buzzerPin, 1200, 300);  // 1200 Hz por 300ms
  delay(400);
  noTone(buzzerPin);           // Garante que o som pare
}


// Variáveis
int contadorToques = 0;
bool toqueAnterior = HIGH;
unsigned long ultimaAtualizacao = 0;

void setup() {
  Serial.begin(115200);
  pinMode(touchPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado.");

  ThingSpeak.begin(client);
}

void loop() {
  bool leituraAtual = digitalRead(touchPin);

  if (toqueAnterior == HIGH && leituraAtual == LOW) {
    contadorToques++;
    Serial.print("👉 Toque detectado! Total: ");
    Serial.println(contadorToques);
    tocarHinoFlamengo(); 
  }
  toqueAnterior = leituraAtual;

  if (millis() - ultimaAtualizacao >= 15000) {
    ThingSpeak.setField(1, contadorToques);
    int code = ThingSpeak.writeFields(channelID, writeAPIKey);

    if (code == 200) {
      Serial.print("📡 Enviado: ");
      Serial.println(contadorToques);
    } else {
      Serial.print("Erro: ");
      Serial.println(code);
    }

    contadorToques = 0;
    ultimaAtualizacao = millis();
  }

  delay(10);
}

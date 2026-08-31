#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// =====================================================
// CONFIGURAÇÃO DO WIFI
// =====================================================

const char* WIFI_SSID = "CirkitWifi";
const char* WIFI_PASSWORD = "";


// =====================================================
// CONFIGURAÇÃO DO FIREBASE
// =====================================================

// Banco Firebase
const char* FIREBASE_URL =
  "https://leonardo-89179-default-rtdb.firebaseio.com/led.json";


// =====================================================
// CONFIGURAÇÃO DO LED
// =====================================================

// Pela montagem mostrada, confirme se o fio amarelo está
// realmente conectado ao GPIO 1.
const int LED_PIN = 1;


// Intervalo entre consultas ao Firebase
const unsigned long INTERVALO = 2000;

unsigned long ultimaConsulta = 0;


// =====================================================
// CONECTAR AO WIFI
// =====================================================

bool conectarWiFi()
{
  Serial.println();
  Serial.println("================================");
  Serial.println("       CONECTANDO AO WIFI       ");
  Serial.println("================================");

  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED && tentativas < 20)
  {
    delay(500);

    Serial.print(".");

    tentativas++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WIFI CONECTADO!");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.println("================================");

    return true;
  }

  Serial.println("ERRO: WIFI NAO CONECTOU!");

  return false;
}


// =====================================================
// LER VALOR DO FIREBASE
// =====================================================

void lerFirebase()
{
  // Verifica Wi-Fi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi desconectado.");

    conectarWiFi();

    return;
  }


  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("CONSULTANDO FIREBASE...");


  // Cliente HTTPS
  WiFiClientSecure client;

  // Necessário para o teste no simulador
  client.setInsecure();


  HTTPClient http;


  // Abre conexão
  if (!http.begin(client, FIREBASE_URL))
  {
    Serial.println("ERRO AO ABRIR FIREBASE!");

    return;
  }


  // Faz GET
  int codigoHTTP = http.GET();


  Serial.print("HTTP: ");
  Serial.println(codigoHTTP);


  // ===================================================
  // FIREBASE RESPONDEU CORRETAMENTE
  // ===================================================

  if (codigoHTTP == HTTP_CODE_OK)
  {
    String resposta = http.getString();

    resposta.trim();


    Serial.print("Firebase: ");
    Serial.println(resposta);


    // ================================================
    // TRUE = LIGA LED
    // ================================================

    if (resposta == "true")
    {
      digitalWrite(LED_PIN, HIGH);

      Serial.println("COMANDO RECEBIDO: TRUE");
      Serial.println("LED VERDE: LIGADO");
    }


    // ================================================
    // FALSE = DESLIGA LED
    // ================================================

    else if (resposta == "false")
    {
      digitalWrite(LED_PIN, LOW);

      Serial.println("COMANDO RECEBIDO: FALSE");
      Serial.println("LED VERDE: DESLIGADO");
    }


    // ================================================
    // VALOR INCORRETO
    // ================================================

    else
    {
      Serial.println("ERRO: valor diferente de true/false.");
    }
  }


  // ===================================================
  // ERRO HTTP
  // ===================================================

  else
  {
    Serial.println("ERRO AO CONSULTAR FIREBASE!");

    Serial.print("Mensagem: ");
    Serial.println(http.errorToString(codigoHTTP));
  }


  http.end();

  Serial.println("--------------------------------");
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(2000);


  Serial.println();
  Serial.println();
  Serial.println("################################");
  Serial.println("#       ESP32 + FIREBASE       #");
  Serial.println("#          LED VERDE            #");
  Serial.println("################################");


  // Configura LED
  pinMode(LED_PIN, OUTPUT);


  // Começa desligado
  digitalWrite(LED_PIN, LOW);


  Serial.print("LED configurado no GPIO: ");
  Serial.println(LED_PIN);


  // Conecta Wi-Fi
  conectarWiFi();
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // Consulta Firebase a cada 2 segundos
  if (millis() - ultimaConsulta >= INTERVALO)
  {
    ultimaConsulta = millis();

    lerFirebase();
  }
}

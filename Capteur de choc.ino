#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "Identifiants.h"

#define Capteur_reed 0
#define LED 2

unsigned long Moment_Present = 0;
unsigned long Timer_Moment_Memorise = 0;
unsigned long Choc_Moment_Memorise = 0;

bool Choc = false;
bool Ouverture = false;

int App_Timer_Choc = Timer_Choc_Secondes;

int i = 0;

void setup() {

  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  pinMode(Capteur_reed, INPUT);
  pinMode(LED, OUTPUT);

  Led(false);  

  WiFi.begin(SSID, Mot_de_Passe);
  Serial.println("Connexion");
  while (WiFi.status() != WL_CONNECTED) {
    cLed();
    Serial.print(".");
    i++;
    if (i== 15) ESP.restart();
  }
  Serial.println("");
  Serial.print("Connecté, Adresse IP: ");
  Serial.println(WiFi.localIP());
  
  Post(ID_Choc_Jeedom, "1");
 
  if (Timer_Veille_Secondes - App_Timer_Choc < 1) App_Timer_Choc = Timer_Veille_Secondes - 1;
  Timer_Moment_Memorise = millis();
  Choc_Moment_Memorise = millis();
  
}

void loop() {

  if (digitalRead(Capteur_reed) == HIGH) {
    if (!Ouverture) {
      Ouverture = true;
      Post(ID_Ouverture_Jeedom, "1");
      Timer_Moment_Memorise = millis();
    }


  } else {
    if (Ouverture) {
      Ouverture = false;
      Post(ID_Ouverture_Jeedom, "0");
      Timer_Moment_Memorise = millis();
    }
  }

  
  Moment_Present = millis();
  if ((Moment_Present - Choc_Moment_Memorise) > App_Timer_Choc * 1000) {
    if (!Choc) {
      Choc = true;
      Post(ID_Choc_Jeedom, "0");
    }
  }

  if ((Moment_Present - Timer_Moment_Memorise) > Timer_Veille_Secondes * 1000) {
    cLed();
    ESP.deepSleep(0);
  }
}

void Led(bool Etat) {
  if (Etat) {
    digitalWrite(LED, 0);
  } else {
    digitalWrite(LED, 1);
  }
}

void cLed() {
  digitalWrite(LED, 0);
  delay(50);
  digitalWrite(LED, 1);
  delay(450);
}

void Post(String ID, String Etat) {
  String Temporaire = URL;
  Temporaire.replace("#IP#", IP_Jeedom);
  Temporaire.replace("#APIKEY#", API_Key_Jeedom);
  Temporaire.replace("#ID#", ID);
  Temporaire.replace("#VALUE#", Etat);
  Serial.println(Temporaire);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, Temporaire.c_str()); 

  int httpResponseCode = http.POST(Temporaire);


  
  http.end();
  delay(500);
}

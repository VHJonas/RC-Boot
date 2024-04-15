#define BLYNK_TEMPLATE_ID    "user3"  // Definieert de Blynk-template ID
#define BLYNK_TEMPLATE_NAME  "user3@server.wyns.it"  // Definieert de Blynk-template naam

#include <ESP8266WiFi.h>  // Inclusief de ESP8266WiFi-bibliotheek
#include <BlynkSimpleEsp8266.h>  // Inclusief de BlynkSimpleEsp8266-bibliotheek
#include <Servo.h>  // Inclusief de Servo-bibliotheek
#include <Wire.h> // Toegevoegd voor I2C-communicatie
#include <QMC5883LCompass.h> // Correcte bibliotheekinclusie voor de kompasmodule

char auth[] = "IiLevYfwU0GhCWnCEfNdq55uwuvdeyN";  // Blynk-authenticatietoken
char ssid[] = "Jonas van hool";  // SSID van het WiFi-netwerk
char pass[] = "jonasvanhool";  // Wachtwoord van het WiFi-netwerk

int ENA = 14; // D5 voor motor  // Definieert de pin voor ENA (snelheidsregeling) van de motor
int IN1 = 12; // D6 voor motor  // Definieert de pin voor IN1 (richting) van de motor
int IN2 = 13; // D7 voor motor  // Definieert de pin voor IN2 (richting) van de motor

#define SERVO_PIN 15 // Pin voor de servo  // Definieert de pin voor de servo

Servo servo;  // Maakt een Servo-object aan
int servoPosition = 90;  // Startpositie voor de servo  // Initialiseert de startpositie voor de servo

QMC5883LCompass compass; // Kompas object  // Instantieert het kompasobject
double heading;   // Variabele om de richting op te slaan  // Variabele om de richting op te slaan

BLYNK_WRITE(V1) {  // Schuifregelaar-widget in de Blynk-app voor de servo  // Functie voor het verwerken van de schuifregelaarwidget voor de servo
  servoPosition = param.asInt();  // Haalt de waarde van de schuifregelaar op
  servo.write(servoPosition);  // Stelt de positie van de servo in
}

BLYNK_WRITE(V2) {  // Schuifregelaar-widget in de Blynk-app voor de motor (snelheid)  // Functie voor het verwerken van de schuifregelaarwidget voor de motorsnelheid
  int speed = param.asInt();  // Haalt de waarde van de schuifregelaar op
  int motorSpeed = map(speed, 0, 100, 0, 255); // Mapping van de snelheid naar het bereik [0, 255]
  analogWrite(ENA, motorSpeed);  // Stelt de motorsnelheid in
}

BLYNK_WRITE(V3) {  // Knoppen-widget in de Blynk-app voor de motor (vooruit/achteruit)  // Functie voor het verwerken van de knoppenwidget voor de motorrichting
  int forwardButton = param.asInt();  // Haalt de waarde van de knoppenwidget op
  
  if (forwardButton == HIGH) {  // Controleert of de vooruitknop is ingedrukt
    digitalWrite(IN1, HIGH);  // Zet IN1 hoog
    digitalWrite(IN2, LOW);  // Zet IN2 laag
  } else {  // Anders achteruit
    digitalWrite(IN1, LOW);  // Zet IN1 laag
    digitalWrite(IN2, HIGH);  // Zet IN2 hoog
  }
}

void setup() {  // Setup-functie
  pinMode(ENA, OUTPUT);  // Initialiseert ENA als uitgang
  pinMode(IN1, OUTPUT);  // Initialiseert IN1 als uitgang
  pinMode(IN2, OUTPUT);  // Initialiseert IN2 als uitgang
  
  servo.attach(SERVO_PIN);  // Koppelt de servo aan de juiste pin

  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);  // Initialiseert Blynk

  // Initialiseer de kompasmodule
  Wire.begin(); // Start I2C-bus
  compass.init(); // Initialiseer kompasmodule

  Serial.begin(9600); // Initialiseer seriële communicatie voor debuguitvoer
}

void loop() {  // Loop-functie
  Blynk.run();  // Voert Blynk-taken uit

  // Lees kompasrichting
  compass.read();
  byte a = compass.getAzimuth(); // Leest azimuthhoek (het hoekverschil van een gefixeerd punt)
  char myArray[3];
  compass.getDirection(myArray, a); // Leest richting uit
  Serial.print(myArray[0]); // Druk richting af naar de seriële monitor
  Serial.print(myArray[1]);
  Serial.print(myArray[2]);
  Serial.println();
  Blynk.virtualWrite(V4, a); // Stuurt azimuthhoek naar Blynk-app
  delay(100); // Wacht een korte tijd voordat de volgende meting wordt uitgevoerd
}

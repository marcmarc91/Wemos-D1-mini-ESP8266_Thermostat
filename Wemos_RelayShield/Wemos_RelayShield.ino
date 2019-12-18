#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//IPAddress SendIP(192, 168, 0, 100);
WiFiUDP udp;

const char *ssid = "Net";
const char *pass = "secret123";

const int relayPin = D1;

unsigned int localPort = 2000;
char packetBuffer[1];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);   //Connect to access point
  WiFi.hostname("WemosRelay");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(400);
  }

  Serial.begin(9600);
  udp.begin(localPort);
}

void loop()
{
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int n = udp.read(packetBuffer, 1);
    packetBuffer[n] = 0;
    IPAddress remoteIp = udp.remoteIP();
    int remotePrt = udp.remotePort();

    Serial.println(packetBuffer);

    if (packetBuffer[0] == 1) {
      digitalWrite(relayPin, HIGH);
      Serial.println('1');
      sendResponse(1, remoteIp, remotePrt);
    } else if (packetBuffer[0] == 0) {
      digitalWrite(relayPin, LOW);
      Serial.println('0');
      sendResponse(0, remoteIp, remotePrt);
    }
  }

}

void sendResponse(int response, IPAddress remoteIp, int remotePort) {
  udp.beginPacket(remoteIp, remotePort);
  char cmd[1];
  cmd[0] = char(response);
  udp.write(cmd, 1); //Send one byte to Wemos Relay
  udp.endPacket();
}

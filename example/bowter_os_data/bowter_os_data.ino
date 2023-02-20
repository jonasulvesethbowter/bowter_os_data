#include <VeDirect.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>

VeDirect ShuntData;
long volt, pwr, i, h1, soc, ttg;
char text_alarm[5];
const char ssid[] = "ulveseth_Guest";
const char pass[] = "pass";
WiFiClientSecure net;
MQTTClient client; 
unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  // do not verify tls certificate
  // check the following example for methods to verify the server:
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecure/WiFiClientSecure.ino
  net.setInsecure();
  while (!client.connect("arduino", "kfbvYA.b4cxoA", "gJ5v9EaxHRSyFJ5ZhOMx0DYkFY6rew84UXvzTNxD1zE")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("1");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(payload == "started") {
    Serial.println("jag startar båten");
  }
  if(payload == "stopped") {
    Serial.println("jag stoppar båten");
  }

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void ny_indata()
{
    /* Parametrar: keyword, utdata, "bitpattern" som sätts om data skrivs */
    /* variabler */
    if (ShuntData.IfMatchGetVariable("V", &volt, 1)) {}
    else if (ShuntData.IfMatchGetVariable("P", &pwr, 2)) {}
    else if (ShuntData.IfMatchGetVariable("I", &i, 4)) {}
    else if (ShuntData.IfMatchGetVariable("SOC", &soc, 8)) {}
    /* text */
    else if (ShuntData.IfMatchGetVariable("TTG", &ttg, 16)) {}}

/* Callback vid checksum, kommer sist efter alla värden lästs in */
void ny_checksum(bool ok)
{
    if (!ok) /* Om Checksum var inte var ok */
      printf("Chk-errror: ");

    /* Om checksum var ok */
    else if (ShuntData.rx_bits == 31) /* om alla bitar "bitpattern" satts = all data lästs in */
    {
      Serial.print(volt);
      Serial.print("mV, ");
      Serial.print(pwr);
      Serial.print("W, ");
      Serial.print(i);
      Serial.print("mA, ");
      Serial.print("SOC=");
      Serial.print(soc);
      Serial.print(", TTG=");
      Serial.println(ttg);
      ShuntData.rx_bits = 0; /* Börja om */
    }
}



void setup() 
{
  Serial.begin(115200); // Initialize serial communications with the PC for debugging.
  ShuntData.rx_bits = 0;
  //WiFi.begin(ssid);
  //client.begin("mqtt.ably.io", 8883, net);
  //client.onMessage(messageReceived);
  //connect();
}

// the loop routine runs over and over again forever:
void loop() 
{
  ShuntData.GetItems(ny_indata, ny_checksum); /* Läs in ny data */
  //client.loop();
  //delay(2000);
  // delay
}

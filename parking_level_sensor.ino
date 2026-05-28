#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT Setup
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "yourunique/parking/level"; // Change this to be unique!

// HC-SR04 Pins
const int TRIG_PIN = D1;
const int ECHO_PIN = D2;

// LED Bar Indicator Pins
const int LED_GREEN = D5;
const int LED_YELLOW = D6;
const int LED_RED = D7;

// Calibration Constants (Adjust these based on your setup in cm)
const int MAX_DISTANCE = 50;  // 0% Full (Object is far away / Tank empty)
const int MIN_DISTANCE = 5;   // 100% Full (Object is very close / Tank full)

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-LevelSensor-";
    clientId += String(random(0, 0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  // Pin modes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 1. Trigger the Ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 2. Read the echo bounce time (in microseconds)
  long duration = pulseIn(ECHO_PIN, HIGH);

  // 3. Calculate distance in cm (Speed of sound is ~340m/s or 0.034 cm/us)
  int distance = duration * 0.034 / 2;

  // Constrain distance within our calibrated limits
  distance = constrain(distance, MIN_DISTANCE, MAX_DISTANCE);

  // 4. Map distance to a Fill Level Percentage
  // As distance gets smaller (closer), fill percentage goes UP.
  int fillPercentage = map(distance, MAX_DISTANCE, MIN_DISTANCE, 0, 100);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Fill Level: ");
  Serial.print(fillPercentage);
  Serial.println("%");

  // 5. LED Bar Indicator Logic
  if (fillPercentage < 33) {
    // Low level / Object far away -> Only Green
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } 
  else if (fillPercentage >= 33 && fillPercentage < 75) {
    // Mid level / Object approaching -> Green + Yellow
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  } 
  else {
    // Critical level / Object very close -> All LEDs ON (Green + Yellow + Red)
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, HIGH);
  }

  // 6. Publish data to MQTT Broker
  String payload = String(fillPercentage);
  client.publish(mqtt_topic, payload.c_str());
  Serial.println("Data published to MQTT.");

  delay(1500); // Wait 1.5 seconds before next reading
}

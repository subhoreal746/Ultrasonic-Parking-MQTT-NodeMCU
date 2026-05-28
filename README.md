# Ultrasonic Parking / Level Sensor with MQTT

An intermediate IoT project using an ESP8266 NodeMCU and an HC-SR04 ultrasonic sensor to calculate distances, map them to real-time fill-level percentages, drive a 3-LED hardware bar indicator, and stream data over MQTT.

## Concepts Covered
- **Ultrasonic Pulse Timing:** Measuring duration to compute distance.
- **Data Mapping:** Using the `map()` function to convert raw centimeters to a 0-100% capacity range.
- **Voltage Divider:** Stepping down the 5V Echo signal safely to 3.3V logic levels.
- **MQTT Telemetry:** Publishing data packets to a public HiveMQ broker.

---

## Hardware Connections & Voltage Divider Blueprint

Because the HC-SR04 runs on 5V, its `Echo` pin outputs 5V. A voltage divider consisting of a **1kΩ resistor** and a **2kΩ resistor** is wired to safely drop the signal to 3.3V before entering the NodeMCU `D2` pin.

| Component | NodeMCU Pin | Connection Note |
| :--- | :--- | :--- |
| **HC-SR04 VCC** | `VV` / `VIN` | Connects to 5V USB Rail |
| **HC-SR04 GND** | `GND` | Ground Share |
| **HC-SR04 Trig**| `D1` | Digital Output |
| **HC-SR04 Echo**| `D2` | Wire inline through the 1kΩ/2kΩ divider network |
| **Green LED** | `D5` | Safe Zone (via 220Ω Resistor) |
| **Yellow LED** | `D6` | Warning Zone (via 220Ω Resistor) |
| **Red LED** | `D7` | Critical Alert Zone (via 220Ω Resistor) |

---

## Setup Instructions
1. Install the `PubSubClient` library in your Arduino IDE.
2. Open `parking_level_sensor.ino` and replace `YOUR_WIFI_SSID` and `YOUR_WIFI_PASSWORD` with your credentials.
3. Update `mqtt_topic` to avoid listening to or overriding someone else's stream on the public broker.
4. Upload the code and open the Serial Monitor set to `115200` baud.

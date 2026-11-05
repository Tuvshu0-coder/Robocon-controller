#include <PS4Controller.h>

// ESP32 core 3.x frees Bluetooth memory at boot unless a sketch says it uses it.
extern "C" bool btInUse() { return true; }

void setup() {
  Serial.begin(115200);

  // Replace the "78:E3:6D:09:23:D6" with the MAC address
  // the controller wants to pair to
  // Note: MAC address must be unicast
  PS4.begin("78:E3:6D:09:23:D6");
  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
    Serial.println("Connected!");
  }

  delay(3000);
}

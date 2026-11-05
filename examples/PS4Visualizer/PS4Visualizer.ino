// Streams the PS4 controller state over USB serial for ps4-visualizer.html.
// Close the Arduino Serial Monitor before connecting from the web page.
#include <PS4Controller.h>


// Button bit order must match BUTTONS in ps4-visualizer.html
uint32_t readButtons() {
  bool b[] = {
    PS4.Up(), PS4.Down(), PS4.Left(), PS4.Right(),
    PS4.Square(), PS4.Cross(), PS4.Circle(), PS4.Triangle(),
    PS4.L1(), PS4.R1(), PS4.L2(), PS4.R2(),
    PS4.Share(), PS4.Options(), PS4.L3(), PS4.R3(),
    PS4.PSButton(), PS4.Touchpad()
  };
  uint32_t bits = 0;
  for (int i = 0; i < (int)(sizeof(b) / sizeof(b[0])); i++) {
    if (b[i]) bits |= (1UL << i);
  }
  return bits;
}

void setup() {
  Serial.begin(115200);
  PS4.begin("78:E3:6D:09:23:D6");
  Serial.println("Ready.");
}

void loop() {
  // One line per frame: P,connected,buttons,l2,r2,lx,ly,rx,ry,battery,charging
  if (PS4.isConnected()) {
    Serial.printf("P,1,%lu,%u,%u,%d,%d,%d,%d,%u,%d\n",
                  (unsigned long)readButtons(), PS4.L2Value(), PS4.R2Value(),
                  PS4.LStickX(), PS4.LStickY(), PS4.RStickX(), PS4.RStickY(),
                  PS4.Battery(), PS4.Charging() ? 1 : 0);
  } else {
    Serial.println("P,0,0,0,0,0,0,0,0,0,0");
  }
  delay(20);  // about 50 updates per second
}

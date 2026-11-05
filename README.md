# PS4 Controller for ESP32

Use a Sony DualShock 4 (PS4) controller as a wireless Bluetooth controller for an ESP32 with the Arduino framework.

The ESP32 pretends to be the PS4 console the controller is paired with. A DualShock 4 only connects to one Bluetooth MAC address, the "master" it remembers. You give the ESP32 that same MAC address, and the controller connects to it.

This library is based on [aed3/PS4-esp32](https://github.com/aed3/PS4-esp32).

---

## What you need

- An ESP32 board with **Classic Bluetooth**: ESP32-WROOM or ESP32-WROVER, such as the common "ESP32 DevKit". The ESP32-S2, S3, C3 and C6 **will not work** because they only support Bluetooth Low Energy.
- A DualShock 4 controller (PS4 controller).
- A USB cable for the controller (only for pairing with SixaxisPairTool).
- A Windows PC with the [Arduino IDE](https://www.arduino.cc/en/software) and the ESP32 board package installed.

---

## Step 1: Install the library

1. Copy this whole folder (`PS4-esp32-master`) into your Arduino libraries folder:
   `Documents\Arduino\libraries\`
2. Restart the Arduino IDE.
3. The examples now appear under **File → Examples → PS4Controller**.

---

## Step 2: Choose the MAC address

The controller and the ESP32 must agree on one MAC address. You can do this in two ways.

### Option A: Use the controller's current master address (no extra tool)

If the controller is already paired with a PS4, you can make the ESP32 copy the console's address.

1. On the PS4 console go to **Settings → System → System Information** and write down the **Bluetooth address**.
2. Use that address in `PS4.begin("xx:xx:xx:xx:xx:xx")` (see Step 3).

> If the controller is later used with the console again, it will still work, because both use the same address.

### Option B: Write a new address into the controller with SixaxisPairTool (recommended)

Use this if you don't have a PS4 console, or want to pick your own address.

1. Download and install **SixaxisPairTool** (search for "SixaxisPairTool download").
2. Plug the PS4 controller into the PC with a USB cable. Windows will install a driver the first time.
3. Open SixaxisPairTool. It shows the **Current Master** address stored in the controller.
4. Type the address you want into the **Change Master** box and click **Update**. For example:
   ```
   78:E3:6D:09:23:D6
   ```
   The address must be **unicast**: the first byte must be an even number (for example `78`, `1a`, `00` are OK; `01`, `ff` are not).
5. Check that **Current Master** now shows your new address.
6. Unplug the controller.

Use that same address in your sketch in Step 3.

---

## Step 3: Upload the connection test sketch

1. Open **File → Examples → PS4Controller → Ps4Connect**.
2. Put your MAC address from Step 2 into `PS4.begin(...)`:
   ```cpp
   PS4.begin("78:E3:6D:09:23:D6");
   ```
3. Select your board: **Tools → Board → ESP32 Arduino → ESP32 Dev Module** (or your board), and select the right **Port**.
4. Click **Upload**.
5. Open **Tools → Serial Monitor** and set the baud rate to **115200**. You should see `Ready.`

> **ESP32 Arduino core 3.x:** every sketch that uses this library must contain this line, otherwise the core frees the Bluetooth memory at boot and the controller never connects:
> ```cpp
> extern "C" bool btInUse() { return true; }
> ```
> All examples already include it. Add it to your own sketches too.

---

## Step 4: Connect the controller

1. Make sure the controller is **not** plugged into the PC, and turn off any PS4 console nearby (otherwise the controller may connect to the console instead).
2. Press the **PS button** on the controller once.
3. The light bar blinks for a few seconds and then stays on. The Serial Monitor prints `Connected!` every 3 seconds.

That's it. Next time just power on the ESP32 and press the PS button.

---

## Step 5: Read buttons and sticks

Open **File → Examples → PS4Controller → PS4ReceiveData** for a full example. The basic pattern:

```cpp
#include <PS4Controller.h>

extern "C" bool btInUse() { return true; }

void setup() {
  Serial.begin(115200);
  PS4.begin("78:E3:6D:09:23:D6");  // your MAC address
}

void loop() {
  if (PS4.isConnected()) {
    if (PS4.Cross()) Serial.println("Cross pressed");
    Serial.printf("Left stick: %d, %d\n", PS4.LStickX(), PS4.LStickY());
  }
  delay(50);
}
```

### Available functions

| Type | Functions | Values |
|---|---|---|
| D-pad | `Up()` `Down()` `Left()` `Right()` `UpLeft()` `UpRight()` `DownLeft()` `DownRight()` | `true` / `false` |
| Face buttons | `Cross()` `Circle()` `Square()` `Triangle()` | `true` / `false` |
| Shoulders | `L1()` `R1()` `L2()` `R2()` | `true` / `false` |
| Triggers (analog) | `L2Value()` `R2Value()` | `0` to `255` |
| Sticks | `LStickX()` `LStickY()` `RStickX()` `RStickY()` | `-128` to `127` |
| Stick clicks | `L3()` `R3()` | `true` / `false` |
| Other buttons | `Share()` `Options()` `PSButton()` `Touchpad()` | `true` / `false` |
| Status | `Battery()` `Charging()` `Audio()` `Mic()` | battery level / `true` / `false` |
| Connection | `isConnected()` | `true` / `false` |

Sticks rarely rest at exactly `0`, so use a dead zone in robot code, for example `if (abs(PS4.LStickX()) > 10)`.

### Connect / disconnect callbacks

```cpp
void onConnect()    { Serial.println("Controller connected!"); }
void onDisconnect() { Serial.println("Controller disconnected."); }

void setup() {
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisconnect);
  PS4.begin("78:E3:6D:09:23:D6");
}
```

For a robot, use `onDisconnect` to **stop all motors** so the robot doesn't keep driving if the controller drops out.

### Send data to the controller (LED and rumble)

See the **PS4SendData** example.

```cpp
PS4.setLed(255, 0, 0);        // light bar color (red, green, blue)
PS4.setRumble(0, 200);        // weak motor, strong motor (0-255)
PS4.setFlashRate(100, 100);   // light on ms, light off ms (0, 0 = always on)
PS4.sendToController();       // actually sends the settings
```

Don't call `sendToController()` faster than about every 10 ms.

---

## Examples

| Example | What it does |
|---|---|
| `Ps4Connect` | Minimal test: prints `Connected!` when the controller connects. |
| `PS4ReceiveData` | Prints every button, stick, trigger and battery value. Also prints the ESP32's Bluetooth address. |
| `PS4SendData` | Rainbow light bar, flash rate from the sticks, rumble from the triggers. |
| `Ps4ViewIncomingBits` | Prints the raw data packet bit by bit (for debugging). |
| `PS4Visualizer` | Streams the controller state over USB to the web visualizer (see below). |

---

## PS4 Visualizer (see the controller on your PC)

1. Upload `examples/PS4Visualizer` to the ESP32 (with your MAC address).
2. **Close the Arduino Serial Monitor** (only one program can use the port).
3. Open `tools/ps4-visualizer.html` in **Chrome** or **Edge** (Firefox doesn't support Web Serial).
4. Click connect, choose the ESP32's COM port, then press the PS button on the controller.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Controller blinks and then turns off, never connects | The MAC address in `PS4.begin()` doesn't match the controller's master address. Check it again in SixaxisPairTool. Make sure the first byte is even (unicast). |
| Never connects on ESP32 core 3.x | Add `extern "C" bool btInUse() { return true; }` to the sketch. |
| Controller connects to the PS4 console instead | Turn off the console or unplug it while using the ESP32. |
| `PS4.begin failed` or Bluetooth errors | Make sure your board is a classic ESP32 (not S2/S3/C3/C6), and that no other Bluetooth library is started in the same sketch. |
| Compile error: sketch too big | **Tools → Partition Scheme → Huge APP (3MB No OTA)**. |
| Visualizer can't open the port | Close the Arduino Serial Monitor and any other program using the COM port. Use Chrome or Edge. |
| SixaxisPairTool doesn't see the controller | Use a USB **data** cable (some cables only charge), try another USB port, and wait for Windows to install the driver. |
| Connection drops sometimes | Keep the controller charged and within a few meters. Add a stop-motors action in `onDisconnect`. |

To check which address the ESP32 is actually using, upload `PS4ReceiveData`: it prints `ESP32 Bluetooth address: ...` at startup. That must match **Current Master** in SixaxisPairTool.

---

## ESP-IDF users

The library also builds as an ESP-IDF component (`component.mk`, `Kconfig`). If you get compile errors on an older ESP-IDF, change **PS4 → Framework compatibility** in `idf.py menuconfig`.

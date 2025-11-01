## AVR64DD32 Data Logger

![IMG_20251030_154057_137](https://github.com/user-attachments/assets/ccda0be3-5725-4cc0-8649-d56d21ef147d)


The **AVR64DD32 Data Logger** is designed to collect environmental and system data and send it to **Google Sheets** via a **custom script** and an **intermediate relay server**.

Since this program is written as a **universal version**, the data being sent are **static values** stored in the `RS485networkdata` structure:

```c
RS485data = { // dummy data initial values  
    .url = "http://yoururl.com/index.php?data=[",
    .dataBuffer = {0},
    .azimuth = 23750,       // 237.50°
    .elevation = 1230,      // 12.30°
    .dayTopElevation = 5625,// 56.25°
    .windSpeed = 13,        // 13 m/s
    .winDirection = 6,      // West
    .firWindSpeed = 10,     // filtered wind speed
    .firWindDirection = 10, // filtered wind direction
    .sunLevel = 495,        // average light level in mV
    .sht21T = 2100,         // temperature +21.00°C
    .sht21RH = 632,         // humidity 63.2%
    .bmp280P = 1001,        // pressure in hPa
    .bmp280T = 2133         // +21.33°C
};
```

---

### Hardware Components

The data logger consists of the following modules:

- **AVR64DD32 microcontroller** – operates using an internal 24 MHz oscillator.  
  Responsible for managing all system processes.
- **SIM800L GSM module** – legacy 2G module used to connect to the internet via **GPRS** and transmit data using **HTTP**.  
- **BK280 GNSS module** – used for local time acquisition and display.  
- **HS180S10B LCD display** – displays system messages and process information.  

---

### Operating Principle

When the data logger is powered on, the following steps occur:

1. **SIM800L Initialization** –  
   The system checks if the GSM module is ready. If successful, corresponding status messages are displayed on the screen.  

2. **GNSS Module Check** –  
   The system waits until the GNSS module receives enough data to determine accurate time.  
   This process can take from a few seconds to several minutes, depending on the **number of satellites** and **signal strength**.  

   Since this step is not critical to the logger’s operation, the **waiting animation** can be disabled by commenting out this line in `main.c`:
   ```c
   generic_animation(gnss_check, "Searching GNSS...", "GNSS Locked!");
   ```
   The same applies to the **SIM800L check** procedure.

3. **HTTP Transmission Initialization** –  
   Once the time is fixed, the SIM800L module begins initializing HTTP communication for data transmission.

4. **Data Send/Receive Status Check** –  
   The logger checks the server response to confirm successful data transfer.

---

### SIM800L Command Processing

All sent AT commands and received responses are processed and translated into more human-readable text in the function:

```c
SIM800LUARTReceiver2(...)
```

This is achieved using:
```c
ChangeAnswers(answer, MESSAGE_LENGTH_SIM800L); 
ChangeCommand(command, MESSAGE_LENGTH_SIM800L);
```

To view **raw AT commands**, simply **comment out** these two lines to disable the text translation layer.

---

### Data Transmission Path

The data flow follows this route:

```
Data Logger → Relay Server → Google Sheets
```

This intermediate server is required because the **SIM800L module does not support HTTPS**, **SSL**, or **TLS 1.2 / 1.3** protocols.  
Therefore, it cannot send data **directly** to Google Sheets.

---

### Image Descriptions

Below are the main operational stages of the **AVR64DD32 Data Logger**, illustrated with example screenshots.

---

#### 1️⃣ SIM800L Status Check
When the data logger is powered on, the **SIM800L module status check** begins.  
This verifies whether the GSM module is properly initialized and ready for communication.

![IMG_20251030_154217_975](https://github.com/user-attachments/assets/aee9db09-82b4-4f63-bd67-3cdcc9ca0c33)


---

#### 2️⃣ Successful SIM800L Initialization
If everything is functioning correctly, the display shows confirmation **messages** indicating successful initialization.

![IMG_20251030_154229_297](https://github.com/user-attachments/assets/d8ecda92-6a33-4242-ac1d-d41baa6d0e7f)

![IMG_20251030_154231_650](https://github.com/user-attachments/assets/72825f20-6304-4f09-9f1a-6026afcbb8ab)


---

#### 3️⃣ BK280 GNSS Module Check
Next, the **BK280 GNSS module** is tested. The system waits until the module receives enough satellite data to determine accurate **local time**.

![IMG_20251030_154321_701](https://github.com/user-attachments/assets/c0a16fa7-2f5c-46ef-a6be-47eed303174c)

![IMG_20251030_155432_890](https://github.com/user-attachments/assets/d2ac0ede-65a6-4a40-950b-dc713afc5842)


---

#### 4️⃣ HTTP Initialization
Once the **local time** is fixed, the **SIM800L module** starts its **HTTP initialization** process to prepare for data transmission.

![IMG_20251030_155437_003](https://github.com/user-attachments/assets/643f1282-cf8b-4ffd-a112-47a61c5b7941)

![IMG_20251030_155439_905](https://github.com/user-attachments/assets/3cd0335c-41ff-4820-9691-e1b6d03f7ca2)

![IMG_20251030_155443_033](https://github.com/user-attachments/assets/82e0b672-9d8e-4752-8017-a21d711da8ce)

![IMG_20251030_155446_296](https://github.com/user-attachments/assets/0ac0221a-812f-4e8b-9f34-aee5d6d71172)

![IMG_20251030_155447_467](https://github.com/user-attachments/assets/621785cc-8d3a-474d-9ef5-a9f9d674e377)

![IMG_20251030_155449_548](https://github.com/user-attachments/assets/dd706c56-3757-4e67-8414-f87c0ed6e95c)


---

#### 5️⃣ Data Transmission and Status Check
Finally, the system begins **sending data** and verifies the **server response** to confirm successful transmission.

![IMG_20251030_155451_163](https://github.com/user-attachments/assets/8ea33482-da9d-441c-aa36-51b2d3c26e5b)

![IMG_20251030_155506_958](https://github.com/user-attachments/assets/17b56aaf-4190-403b-9745-162b3150a354)

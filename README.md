# BP-Telegram-ESP32CAM
ESP32-CAM based IoT system that captures blood pressure monitor readings and sends the image to a Telegram bot for remote monitoring.
# IoT Blood Pressure Monitor Logger using ESP32-CAM and Telegram Bot

## Project Overview

This project implements an **IoT-based Blood Pressure Monitoring Logger** using an **ESP32-CAM microcontroller**. The system captures the display of a digital blood pressure monitor and sends the captured image to a **Telegram bot over WiFi**.

After a blood pressure measurement is completed, the operator presses a **push button**. The ESP32-CAM automatically captures a photo of the BP monitor screen showing **SYS/DIA values** and sends the image to Telegram for **remote monitoring and record keeping**.

This system allows doctors or operators to **store and view BP readings remotely**, making it useful for **medical camps, telemedicine systems, and remote patient monitoring**.

---

## Features

- ESP32-CAM based image capture system  
- Capture BP monitor display values  
- Push button triggered image capture  
- Flash LED support for clear image capture  
- Automatic image transmission using Telegram Bot  
- WiFi based IoT communication  
- Dynamic Telegram Chat ID detection  
- Serial commands for testing and debugging  

---

## Hardware Components

- ESP32-CAM (AI Thinker module)  
- Digital Blood Pressure Monitor  
- Push Button (capture trigger)  
- Flash LED  
- FTDI Programmer (USB-TTL)  
- Jumper wires  
- Power supply  
- Custom enclosure  

---

## System Workflow

### 1. System Start
- ESP32-CAM powers on  
- Device connects to WiFi  
- Telegram bot connection initializes  
- System waits for button trigger  

### 2. BP Measurement
- Blood pressure is measured using the digital BP monitor  
- SYS and DIA values appear on the display  

### 3. Image Capture Trigger
- Operator presses the push button  
- ESP32-CAM activates the flash LED  

### 4. Photo Capture
- ESP32-CAM camera captures an image of the BP monitor display  

### 5. Telegram Transmission
- Captured image is sent to the Telegram bot via WiFi using HTTPS  

### 6. Data Logging
- BP reading image appears in the Telegram chat  
- Doctors or operators can view and store BP readings remotely  

### 7. System Reset
- System returns to idle mode  
- Waits for the next BP measurement  

---

## Telegram Bot Integration

The system uses the **Telegram Bot API** to send captured images.

### Telegram Bot Functions

- Receive BP reading images  
- Provide remote monitoring  
- Store BP reading records through chat history  

---

## Setup Steps

1. Open **Telegram**
2. Search for **BotFather**
3. Create a new bot using:

```
/newbot
```

4. Copy the **Bot Token**
5. Replace the token inside the project code
6. Upload the code to **ESP32-CAM**

The system will automatically detect the **Telegram Chat ID dynamically**.

---

## Software Details

### Microcontroller
ESP32-CAM

### Programming Language
Embedded C (Arduino Framework)

### Development Environment
Arduino IDE

### Libraries Used

- WiFi  
- WiFiClientSecure  
- esp_camera  
- UniversalTelegramBot  
- ArduinoJson  
- HTTPClient  

---

## Applications

- Remote patient monitoring systems  
- Telemedicine platforms  
- Medical camp data collection  
- IoT healthcare monitoring systems  
- BP reading documentation systems  

---

## Project Images

Add images inside the `images` folder and reference them like this:

```
![System Setup](images/system.jpg)

![Captured BP Image](images/bp_capture.jpg)
```

---

## Author

**Jeyaprakash V**  
Embedded Systems & IoT Developer

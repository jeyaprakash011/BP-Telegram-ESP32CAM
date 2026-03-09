/*
 * Using ESP32 and ESP CAM (Capture Photo) 
 * Create a Telegram Bot 
 * This is a 
 */

 
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

char msg[500];

const char* ssid = "admin";
const char* password = "1234567890";

String BOTtoken = "xcvbnmzxcvbnmzxcvbnxcvbn"; // replace with your bottoken ID
//String CHAT_ID="863765688"; // static chat id
String CHAT_ID="";
bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define FLASH_LED_PIN 12
#define BUTTON_PIN 15
#define POWER_PIN 13
int a,b,c=1;

bool flashState = LOW;
bool buttonPressed = false;

unsigned long lastTimeBotRan;
const int botRequestDelay = 1000; 

unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 300;

uint32_t ts=0;
uint32_t ts1=0;



// CAMERA_MODEL_AI_THINKER config
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void configInitCamera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // Adjust frame size and quality based on available memory
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA; // Lower resolution than UXGA
    config.jpeg_quality = 12;           // Lower quality (0-63, lower means better)
    config.fb_count = 1;                // Only one frame buffer
  } else {
    config.frame_size = FRAMESIZE_CIF;  // Even lower resolution
    config.jpeg_quality = 15;           // Lower quality
    config.fb_count = 1;
  }
  
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    Serial.println("Trying again with lower resolution...");
    
  
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 20;
    err = esp_camera_init(&config);
    
    if (err != ESP_OK) {
      Serial.printf("Camera init failed again with error 0x%x", err);
      delay(1000);
      ESP.restart();
    }
  }
}

String sendPhotoTelegram() {
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";
  digitalWrite(FLASH_LED_PIN, HIGH);
  delay(200); 

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb); // dispose the buffered image
  
  // Take a new photo
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    digitalWrite(FLASH_LED_PIN, LOW);
    return "Camera capture failed";
  }  

  Serial.println("Connect to " + String(myDomain));

  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    size_t imageLen = fb->len;
    size_t extraLen = head.length() + tail.length();
    size_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000; 
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  digitalWrite(FLASH_LED_PIN, LOW);
  return getBody;
}

void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  Serial.begin(115200);
  delay(100); 


  pinMode(FLASH_LED_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
   digitalWrite(POWER_PIN, HIGH);
  digitalWrite(FLASH_LED_PIN, flashState);
  pinMode(BUTTON_PIN, INPUT_PULLUP);


  
  Serial.println("Initializing camera...");
  configInitCamera();
  Serial.println("Camera initialized successfully");


  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)  
  {  
   Serial.println("Waiting for connection");delay(250);
  }
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT);
 
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("System ready");
  bot.sendMessage(CHAT_ID, String(" System...Initialized"), "");
}

 
void handleNewMessages(int numNewMessages) {   // This function for get a chat ID 
  for (int i = 0; i < numNewMessages; i++) {
    String newChatID = String(bot.messages[i].chat_id);
    
    if (CHAT_ID != newChatID) {
      CHAT_ID = newChatID;
      Serial.print("New CHAT ID detected: ");
      Serial.println(CHAT_ID);

      bot.sendMessage(CHAT_ID, "Chat linked dynamically", "");
    }
  }
}

void loop() {

    if (millis() > lastTimeBotRan + botRequestDelay) {
    int newMessages = bot.getUpdates(bot.last_message_received + 1);

  while (newMessages) {
    handleNewMessages(newMessages);
    newMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  lastTimeBotRan = millis();
}

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonTime > debounceDelay) {

      Serial.println("Button Pressed  Sending Photo...");

      bot.sendMessage(CHAT_ID, "Capturing Photo...", "");

      String result = sendPhotoTelegram();
      
      Serial.println(result);

      lastButtonTime = millis();
    }
  }

  if (Serial.available() > 0) {    // For testing 
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "photo") {
      Serial.println("Photo request received via Serial");
      String response = sendPhotoTelegram();
      bot.sendMessage(CHAT_ID, msg, "");
    } else if (command == "flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.print("Flash toggled via Serial - Now ");
      Serial.println(flashState ? "ON" : "OFF");
    } 

  }



}

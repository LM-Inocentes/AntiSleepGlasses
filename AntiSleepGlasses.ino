#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


/* 1. Define the WiFi credentials */
#define WIFI_SSID "ZTE_2.4G_kEu6M7"
#define WIFI_PASSWORD "VK37M2vR"
#define API_KEY "AIzaSyDEd7DgmMBGQCf5uuBXFiclLRcAw9jmhyU"
#define DATABASE_URL "https://anti-sleep-glasses-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define inputPin 34
#define buzzerPin 32

FirebaseData fbdo, fbdo_s1;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long inputHighStartTime = 0; // To track when the input goes HIGH
bool signupOK = false;
bool isBuzzerOn = false;
bool flag = false;
int IRSensor = 0;

void setup(){
    pinMode(inputPin, INPUT);
    pinMode(buzzerPin, OUTPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
    }

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        signupOK = true;
    }
    config.token_status_callback = tokenStatusCallback; 
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    //stream
    // Firebase.RTDB.beginStream(&fbdo_s1, "isBuzzerOn");
}

void loop(){
      IRSensor = digitalRead(inputPin);
        //Buzzer Function
        if (IRSensor == LOW) {
          if (inputHighStartTime == 0) { // If the input has just gone HIGH
            inputHighStartTime = millis(); // Record the time
          }else if ((millis() - inputHighStartTime) >= 2000) { // Check if it's been high for 2 seconds
            digitalWrite(buzzerPin, HIGH); // Turn the buzzer on
            isBuzzerOn = true;
          }
        }else {
          inputHighStartTime = 0; 
          digitalWrite(buzzerPin, LOW); 
          isBuzzerOn = false;
        }

    //Send Data
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0) ){
        sendDataPrevMillis = millis();
        //Store Data
        flag = !flag;
        Firebase.RTDB.setInt(&fbdo, "/Users/E9KHvYFHCTRlsLcKkZgDfl36ksD3/ir_data", IRSensor);
        Firebase.RTDB.setBool(&fbdo, "/Users/E9KHvYFHCTRlsLcKkZgDfl36ksD3/isBuzzerOn", isBuzzerOn);
        Firebase.RTDB.setBool(&fbdo, "/Users/E9KHvYFHCTRlsLcKkZgDfl36ksD3/isFlag", flag);
        Firebase.RTDB.setBool(&fbdo, "/Users/E9KHvYFHCTRlsLcKkZgDfl36ksD3/isOnline", true);
    }
    
    //Read Data
    // if (Firebase.ready() && signupOK){
    //   Firebase.RTDB.readStream(&fbdo_s1);
    //   if(fbdo_s1.streamAvailable()){
    //       if(fbdo_s1.boolData()){
    //         Serial.println("The User is Drowsy");
    //       }else{
    //         Serial.println("The User is Awake");
    //       }
    //   }
    // }      
}
#include <WiFi.h>
#include <WiFiManager.h>

#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= FIREBASE =================

#define API_KEY "AIzaSyAibfhocPq34T88lX9B1xMumLGlLpYxORE"

#define DATABASE_URL "https://farm-project-d8451-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;

FirebaseAuth auth;

FirebaseConfig config;

bool signupOK = false;

WiFiManager wm;

// ================= PINS =================

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define MQ2_PIN 34

#define RELAY_PIN 25

#define RESET_WIFI_PIN 27

#define SOIL_PIN 35

// ================= OBJECTS =================

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= VARIABLES =================

int temperature = 0;

int humidity = 0;

int soilMoisture = 0;

int mq2Raw = 0;

bool fanOn = false;

String mode = "AUTO";

// ================= DEGREE SYMBOL =================

byte degChar[8] = {

  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {

  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);

  pinMode(RESET_WIFI_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, LOW);

  dht.begin();

  analogReadResolution(12);

  analogSetAttenuation(ADC_11db);

  Wire.begin(21, 22);

  lcd.init();

  lcd.backlight();

  lcd.createChar(0, degChar);

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("Connecting WiFi");

  // ================= WIFI MANAGER =================



  bool res = wm.autoConnect("ESP32-AirMonitor");

  if (!res) {

    lcd.clear();

    lcd.print("WiFi Failed");

    delay(3000);

    ESP.restart();
  }

  lcd.clear();

  lcd.print("WiFi Connected");

  delay(2000);

  // ================= FIREBASE =================

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {

    signupOK = true;

  } else {

    Serial.println(config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  lcd.clear();

  lcd.print("System Ready");

  delay(2000);
}

void loop() {

  if (digitalRead(RESET_WIFI_PIN) == LOW) {

    delay(3000);

    if (digitalRead(RESET_WIFI_PIN) == LOW) {

      Serial.println("Resetting WiFi");

      lcd.clear();

      lcd.setCursor(0, 0);

      lcd.print("Reset WiFi");

      lcd.setCursor(0, 1);

      lcd.print("Restarting...");

      delay(2000);

      wm.resetSettings();

      ESP.restart();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi Lost... Reconnecting");

    WiFi.reconnect();

    delay(5000);
  }
  if (!Firebase.ready()) {

    Serial.println("Firebase not ready");
  }

  // ================= DHT =================

  float t = dht.readTemperature();

  float h = dht.readHumidity();

  if (!isnan(t))
    temperature = (int)t;

  if (!isnan(h))
    humidity = (int)h;

  // ================= MQ2 =================

  long sum = 0;

  for (int i = 0; i < 10; i++) {

    sum += analogRead(MQ2_PIN);

    delay(2);
  }

  mq2Raw = sum / 10;


  // ================= SOIL MOISTURE =================

  soilMoisture = analogRead(SOIL_PIN);

  // ================= MODE =================

  if (Firebase.ready() && signupOK) {

    if (Firebase.RTDB.getString(&fbdo,
                                "/device1/mode")) {

      mode = fbdo.stringData();
    }

    // ================= AUTO MODE =================

    if (mode == "AUTO") {

      if (mq2Raw > 1200) {

        digitalWrite(RELAY_PIN, HIGH);

        fanOn = true;
      }

      else {

        digitalWrite(RELAY_PIN, LOW);

        fanOn = false;
      }
    }

    // ================= MANUAL MODE =================

    else {

      if (Firebase.RTDB.getInt(&fbdo,
                               "/device1/fan")) {

        int fanValue = fbdo.intData();

        if (fanValue == 1) {

          digitalWrite(RELAY_PIN, HIGH);

          fanOn = true;
        }

        else {

          digitalWrite(RELAY_PIN, LOW);

          fanOn = false;
        }
      }
    }

    // ================= UPLOAD DATA =================

    Firebase.RTDB.setInt(&fbdo,
                         "/device1/temperature",
                         temperature);

    Firebase.RTDB.setInt(&fbdo,
                         "/device1/humidity",
                         humidity);

    Firebase.RTDB.setInt(&fbdo,
                         "/device1/gas",
                         mq2Raw);

    Firebase.RTDB.setInt(&fbdo,
                         "/device1/fanState",
                         fanOn);

    Firebase.RTDB.setString(&fbdo,
                            "/device1/mode",
                            mode);

    // ================= REAL TIMESTAMP =================

    FirebaseJson json;

    json.set(".sv", "timestamp");

    Firebase.RTDB.setJSON(&fbdo,
                          "/device1/lastSeen",
                          &json);


    Firebase.RTDB.setInt(&fbdo,
                         "/device1/soil",
                         soilMoisture);
  }



  // ================= SERIAL =================

  Serial.print("Temp: ");

  Serial.print(temperature);

  Serial.print("C ");

  Serial.print("Humidity: ");

  Serial.print(humidity);

  Serial.print("% ");

  Serial.print("Gas: ");

  Serial.print(mq2Raw);

  Serial.print(" Soil: ");

  Serial.print(soilMoisture);

  Serial.print(" Fan: ");

  Serial.print(fanOn ? "ON" : "OFF");

  Serial.print(" Mode: ");

  Serial.println(mode);

  // ================= LCD =================

  static bool screenToggle = false;

  lcd.clear();

  if (screenToggle) {

    lcd.setCursor(0, 0);

    lcd.print("T:");

    lcd.print(temperature);

    lcd.write(0);

    lcd.print("C ");

    lcd.print("H:");

    lcd.print(humidity);

    lcd.print("%");

    lcd.setCursor(0, 1);

    lcd.print("Gas:");

    lcd.print(mq2Raw);

    if (fanOn)
      lcd.print(" FAN");
  }

  else {

    lcd.setCursor(0, 0);

    lcd.print("Soil:");

    lcd.print(soilMoisture);

    lcd.setCursor(0, 1);

    lcd.print("Mode:");

    lcd.print(mode);
  }

  screenToggle = !screenToggle;

  delay(2000);
}
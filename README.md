# Smart Farm & Air Monitor Project

This project is an IoT-based smart farming and air monitoring system built with an ESP32 microcontroller, Firebase Realtime Database, and a web dashboard.

## Features
- **Sensor Data Collection:** Reads temperature and humidity (DHT11), gas levels (MQ2), and soil moisture.
- **Automated/Manual Fan Control:** 
  - **Auto Mode:** Automatically turns on the fan/ventilation via a relay when gas levels exceed a threshold (e.g., > 1200).
  - **Manual Mode:** Allows remote control of the fan from the web dashboard.
- **Local Display:** Shows sensor readings and system status on an I2C 16x2 LCD.
- **WiFi Manager:** Easy WiFi configuration via a captive portal (access point "ESP32-AirMonitor"). Includes a physical reset button.
- **Web Dashboard:** A responsive web interface to monitor real-time sensor data and control the device modes remotely.
- **Firebase Integration:** Uses Firebase Realtime Database to store sensor readings and sync control states.

## Hardware Components
- ESP32 Development Board
- DHT11 Temperature and Humidity Sensor
- MQ2 Gas Sensor
- Soil Moisture Sensor
- I2C 16x2 LCD Display
- 1-Channel Relay Module (for Fan/Ventilation)
- Push Button (for WiFi Reset)
- Connecting Wires & Breadboard

## Project Structure
- `Farm_project_esp_code/`: Contains the Arduino (`.ino`) sketch for the ESP32.
- `air monitor dashboard/`: Contains the source code (HTML, CSS, JS) for the web dashboard.
- `Farm Project.fzz`: Fritzing file containing the circuit wiring diagram.

## Setup Instructions

### Hardware
Open the `Farm Project.fzz` file in Fritzing to view the complete wiring diagram and connect your components accordingly.

### Software (ESP32)
1. Open `Farm_project_esp_code.ino` in the Arduino IDE.
2. Install required libraries:
   - `WiFiManager`
   - `Firebase ESP Client`
   - `DHT sensor library`
   - `LiquidCrystal I2C`
3. Update the Firebase credentials (`API_KEY` and `DATABASE_URL`) in the code if setting up your own Firebase project.
4. Upload the code to your ESP32.

### Web Dashboard
1. Set up a Firebase project and configure the Realtime Database.
2. Ensure your Firebase configuration is correctly set in the dashboard's `script.js` or Firebase initialization scripts.
3. Host the dashboard using Firebase Hosting, GitHub Pages, or run it locally.

## License
MIT License

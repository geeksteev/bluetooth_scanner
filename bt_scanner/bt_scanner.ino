#include <NimBLEDevice.h>     // NimBLE library for ESP32
#include <SPI.h>              // SPI for the TFT and SD card
#include <Adafruit_GFX.h>     // Adafruit GFX library for graphics
#include <Adafruit_ILI9341.h> // TFT library for ILI9341 controller
#include <SD.h>               // SD card library

// Pin configurations
#define TFT_MOSI 23        // Data in (SDA)
#define TFT_SCLK 18        // Clock (SCL)
#define TFT_CS   15        // Chip select
#define TFT_DC   2         // Data/Command select
#define TFT_RST  4         // Reset
#define TFT_BL   14        // Backlight

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
std::vector<std::string> uniqueDevices;  // Vector to store unique device addresses

// Array to store found devices
String foundDevices[50];
int deviceCount = 0;

// Function to check if a device is already listed
bool isDeviceListed(String macAddress) {
  for (int i = 0; i < deviceCount; i++) {
    if (foundDevices[i] == macAddress) {
      return true;
    }
  }
  return false;
}

// Callback class to handle found devices
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    String deviceManufacturer = advertisedDevice->getManufacturerData().c_str();
    String deviceName = advertisedDevice->getName().c_str();
    String deviceAddress = advertisedDevice->getAddress().toString().c_str();
    
    // Only add devices with a valid name and check for uniqueness
    if (deviceName.length() > 0 && !isDeviceListed(deviceAddress)) {
      if (deviceCount < 50) {
        foundDevices[deviceCount++] = deviceAddress;

        // Print device name and MAC address on TFT with smaller font
        tft.setCursor(10, 10 + (deviceCount * 15));  // Adjust cursor position for each device
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(1);  // Set smaller font size
        tft.println(deviceName + " (" + deviceAddress + ")");

        // Debugging on Serial Monitor
        Serial.println("Found Device: " + deviceName + " (" + deviceAddress + ")");
      }
    }
  }
};

// Called when scan is complete
void scanComplete(NimBLEScanResults results) {
  Serial.println("Scan complete!");
  
  // Clear the TFT screen and reset device count for the next scan
  tft.fillScreen(ILI9341_BLACK);
  deviceCount = 0;

  // Restart the scan immediately for continuous scanning
  NimBLEDevice::getScan()->start(10, scanComplete);  // Scan again for 10 seconds
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(2);  // Set to portrait mode
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);  // Set smaller font size for initial text
  tft.setCursor(10, 10);
  tft.println("Scanning for BT devices...");
  
  // Initialize NimBLE device
  NimBLEDevice::init("");
  NimBLEScan* pScan = NimBLEDevice::getScan();  // Create scan object
  pScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pScan->setActiveScan(true);                   // Active scan uses more power but gets results faster
  pScan->start(10, scanComplete);               // Start scan for 10 seconds
}

void loop() {
  // Nothing needed in the loop since the scan is handled in the background
}

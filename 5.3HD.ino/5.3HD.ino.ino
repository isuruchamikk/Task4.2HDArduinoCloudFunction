#include <WiFiNINA.h>
#include <Firebase_Arduino_WiFiNINA.h>

// WiFi credentials
#define WIFI_SSID "WiFi-1D3B01"
#define WIFI_PASSWORD "14880964"

// Firebase configuration
#define FIREBASE_HOST "arduinotrafficlight-54a3a-default-rtdb.firebaseio.com" //URL of your Firebase Realtime Database
#define FIREBASE_AUTH "bzikJab0UHXMkMP9EiOj6qyiK9KMJfarlDwdhZTk" // Database secret key

// Assigns digital pins into the LEDs
#define RED_PIN 2
#define GREEN_PIN 3
#define BLUE_PIN 4

// Creates an object to handle data
FirebaseData fbdo;

// Defines paths in Firebase(where LED states are stored) 
String RED = "/leds/red";
String GREEN = "/leds/green";
String BLUE = "/leds/blue";

void setup() {
  Serial.begin(9600);

  // Initialize LEDs (Configures LED pins as outputs and prints a confirmation)
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.println("LED pins initialized");

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase (with simpler error checking)
  Serial.print("Connecting to Firebase...");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  // Test Firebase connection by writing a value (Firebase Test)
  if (Firebase.setString(fbdo, "/connection_test", "hello")) {
    Serial.println(" SUCCESS");
  } else {
    Serial.println(" FAILED");
    Serial.println("Reason: " + fbdo.errorReason());
    while (1)
      ;  // Stop if Firebase fails
  }

  // Start listening to LED commands (in Firebase)
  if (!Firebase.beginStream(fbdo, "/leds")) {
    Serial.println("Stream error: " + fbdo.errorReason());
  } else {
    Serial.println("Stream started at path: /leds");
  }
}

void loop() {    // Checks for new Firebase data or errors
  if (!Firebase.readStream(fbdo)) {
    Serial.println("Stream error: " + fbdo.errorReason());
  }

  if (fbdo.streamAvailable()) {
    String path = fbdo.streamPath();   // e.g., "/leds/red"

    // Extract LED color from path (e.g., "red" from "/leds/red")
    String ledColor = path.substring(path.lastIndexOf('/') + 1);

    // Debug print to Serial Monitor
    Serial.print("LED: ");
    Serial.print(ledColor);

    // // Update LED states 
    if (Firebase.getString(fbdo, "/leds/red")) {
      toggleLED(RED_PIN, String(fbdo.stringData()));
    }
    if (Firebase.getString(fbdo, "/leds/green")) {
      toggleLED(GREEN_PIN, String(fbdo.stringData()));
    }
    if (Firebase.getString(fbdo, "/leds/blue")) {
      toggleLED(BLUE_PIN, String(fbdo.stringData()));
    }
  }
  delay(100);
}

// Turns the LED based on Firebase commands
void toggleLED(int pin, String command) {
  if (command == "on") {
    digitalWrite(pin, HIGH);
    Serial.println("LED " + String(pin) + " turned ON");
  } else if (command == "off") {
    digitalWrite(pin, LOW);
    Serial.println("LED " + String(pin) + " turned OFF");
  }
}
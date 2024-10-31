#include <WiFi.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "your_SSID";        // Replace with your network SSID (name)
const char* password = "your_PASSWORD"; // Replace with your network password

WiFiServer server(80); // Create a server that listens on port 80

// Define GPIO pins for specific actions (Replace with your actual pin numbers)
const int upActionPin = 13;
const int downActionPin = 12;   //********må endres ut ifra servomotoren!!
const int leftActionPin = 14;   // *******left og right kan muligens fjernes, kobles opp til samme servomotor
const int rightActionPin = 27; 


// Definer pinnen og variabler for PWM
const int servoPin = 22;         // ******PWM-utgangspinn for servo. ENDRES
int current_turn_state = 90;

Servo servo;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  servo.attach(servoPin);
  servo.write(current_turn_state); //setter servoen i nøytral posisjon
  // Initialize GPIO pins
  pinMode(upActionPin, OUTPUT);
  pinMode(downActionPin, OUTPUT);
  pinMode(leftActionPin, OUTPUT);
  pinMode(rightActionPin, OUTPUT);

  // Set initial states to LOW
  digitalWrite(upActionPin, LOW);
  digitalWrite(downActionPin, LOW);
  digitalWrite(leftActionPin, LOW);
  digitalWrite(rightActionPin, LOW);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to Wi-Fi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
}



void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");
    
    String currentLine = ""; // To store incoming data
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); // Read incoming data from client
        if (c == '\n') {
          // Trim the incoming command to remove extra spaces
          currentLine.trim(); 
          // Process the data
          Serial.println("Received: " + currentLine);

          // Perform actions based on received command
          if (currentLine == "UP") {
            Serial.println("Performing UP action");
            digitalWrite(upActionPin, HIGH);  // Here comes your code for UP action
            delay(100);                       // Simulate an action duration
            digitalWrite(upActionPin, LOW);
          }
          else if (currentLine == "DOWN") {
            Serial.println("Performing DOWN action");
            digitalWrite(downActionPin, HIGH); // Here comes your code for DOWN action
            delay(100);
            digitalWrite(downActionPin, LOW);
          }
          else if (currentLine == "LEFT") {
            current_turn_state += 10;
            if (current_turn_state > 180) current_turn_state = 180;  // Limiting max angle
            servo.write(current_turn_state);
            Serial.println("Turning LEFT: " + String(current_turn_state));
            delay(100);
          }
          else if (currentLine == "RIGHT") {
            current_turn_state -= 10;
            if (current_turn_state < 0) current_turn_state = 0;      // Limiting min angle
            servo.write(current_turn_state);
            Serial.println("Turning RIGHT: " + String(current_turn_state));
            delay(100);
          }
          else if (currentLine == "ESC") {
            Serial.println("ESC key pressed - stopping all actions.");
            // Perform any ESC action if needed, like resetting the states.
          }
          else if (currentLine == "HJEM") {
            Serial.println("Performing HJEM - Turning around");        // Snu 180 grader. Enten vha prøv-feil eller sensor 
            servo.write(70);
            delay(1000);
            digitalWrite(upActionPin, HIGH);
            delay(2000);                                               // Hvor lenge den må kjøre for å snu helt rundt
            digitalWrite(upActionPin, LOW);  
            servo.write(90);                   
          }

          // Clear the currentLine after processing
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c; // Add character to the line
        }
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}

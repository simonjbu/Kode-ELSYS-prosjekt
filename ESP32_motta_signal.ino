#include <WiFi.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <MPU6050.h>

// Wi-Fi credentials
const char* ssid = "SIMONPC";        // Byttes ut med nettverkets SSID
const char* password = "12345678"; // Byttes ut med ordentlig passord

serial.print
WiFiServer server(80); 

const int upActionPin = 12;
const int downActionPin = 14; 
const int turnActionPin = 27;  

const int turnAngle = 180;

// Definer pinnen og variabler for PWM
const int servoPin = 22;         // PWM-utgangspinn for servo.
int current_turn_state = 90;     // Setter roret i nøytral posisjon. Dette må endres med testing

Servo servo;
MPU6050 gyrosensor;

void setup() {
  // Initialser kommunikasjon
  Serial.begin(115200);

  servo.attach(servoPin);
  servo.write(current_turn_state); //setter servoen i nøytral posisjon

  pinMode(upActionPin, OUTPUT);
  pinMode(downActionPin, OUTPUT);
  pinMode(turnActionPin, OUTPUT);

  digitalWrite(upActionPin, LOW);
  digitalWrite(downActionPin, LOW);
  digitalWrite(turnActionPin, LOW);

  // Initialiser gyro-sensor MPU6050
  Wire.begin();
  gyrosensor.initialize();
  if (!gyrosensor.testConnection()) {
    Serial.println("MPU6050 tilkobling feilet");
    while (1);  // Stopp hvis gyrosensoren ikke fungerer
  }


  // Koble til Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to Wi-Fi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start serveren
  server.begin();
}


// Funksjon for å utføre 180-graders rotasjon
void snuBat180Grader() {
    float aktuellVinkel = 0.0;  // Startvinkel
    unsigned long forrigeTid = millis();  // Starttid for integrasjon

    // Les første rotasjonshastighet (for trapesintegrasjon)
    int16_t gyroX, gyroY, gyroZ;
    gyrosensor.getRotation(&gyroX, &gyroY, &gyroZ);
    float forrigeVinkelHastighetZ = gyroZ / 131.0;  // Skaleringsfaktor for MPU6050

    // Start rotasjonen
    servo.write(20);            // Sett roret til høyre (juster verdi etter behov)
    digitalWrite(upActionPin, HIGH);

    // Kontinuerlig oppdatering av gyrosensor for å måle rotasjonen
    while (abs(aktuellVinkel) < turnAngle) {
        unsigned long nåværendeTid = millis();
        float tidsDifferanse = (nåværendeTid - forrigeTid) / 1000.0;  // Delta-t i sekunder
        forrigeTid = nåværendeTid;
  
        // Konverter gyroskopdata til vinkelhastighet i grader/sekund og integrer
        gyrosensor.getRotation(&gyroX, &gyroY, &gyroZ);
        float aktuellVinkelHastighetZ = gyroZ / 131.0;  // Skaleringsfaktor for MPU6050
        
        // Bruk trapesintegrasjon for å oppdatere total rotasjon
        float snittVinkelHastighetZ = (forrigeVinkelHastighetZ + aktuellVinkelHastighetZ) / 2.0;
        aktuellVinkel += snittVinkelHastighetZ * tidsDifferanse;
        forrigeVinkelHastighetZ = aktuellVinkelHastighetZ;

        Serial.print("Vinkel: ");
        Serial.println(aktuellVinkel);

        delay(10);  // Kort pause for å unngå for rask oppdatering
    }

    // Stopp DC-motoren og sett roret tilbake til nøytral posisjon
    digitalWrite(upActionPin, LOW);
    servo.write(90);             // Sett roret til midtstilling
}


void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");
    
    String currentLine = ""; // For å lagre innkommende data
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); //
        if (c == '\n') {
          // Fjern mellomrom
          currentLine.trim(); 
          Serial.println("Received: " + currentLine);


          // Handle basert på hvilke signal som mottas fra PCen
          if (currentLine == "UP") {
            Serial.println("Performing UP action");
            digitalWrite(upActionPin, HIGH); 
            delay(100);                   
            digitalWrite(upActionPin, LOW);
          }
          else if (currentLine == "DOWN") {
            Serial.println("Performing DOWN action");
            digitalWrite(downActionPin, HIGH); 
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
            if (current_turn_state < 0) current_turn_state = 0;    
            servo.write(current_turn_state);
            Serial.println("Turning RIGHT: " + String(current_turn_state));
            delay(100);
          }
          else if (currentLine == "ESC") {
            Serial.println("ESC key pressed - stopping all actions.");
            // Perform any ESC action if needed, like resetting the states.
          }
          
          else if (currentLine == "HJEM") {
            Serial.println("Performing HJEM - Turning around");       
            // snuBat180Grader();                                        
            // Snu 180 grader. Enten vha gyro-sensor eller en gjetning på hvor lenge den skal snu. 
            // Hvis vi ikke får gyrosensoren til å fungere, utføres den kommenterte koden.
            servo.write(20);                                       
            delay(1000);
            digitalWrite(upActionPin, HIGH);
            delay(2000);                                      
            digitalWrite(upActionPin, LOW);  
            servo.write(90);                   
          }

          
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
  
    client.stop();
    Serial.println("Client disconnected.");
  }
}

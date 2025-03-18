#include <WiFi.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

const int ledPin = 2;  // Built-in LED on most ESP32 boards

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Set up Access Point
  Serial.println("Setting up Access Point...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>body { font-family: Arial; text-align: center; }");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; }</style></head>");
            
            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p><a href=\"/LED=ON\"><button class=\"button\">ON</button></a></p>");
            client.println("<p><a href=\"/LED=OFF\"><button class=\"button\">OFF</button></a></p>");
            client.println("</body></html>");
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        
        if (currentLine.endsWith("GET /LED=ON")) {
          digitalWrite(ledPin, HIGH);
        } else if (currentLine.endsWith("GET /LED=OFF")) {
          digitalWrite(ledPin, LOW);
        }
      }
    }
    client.stop();
  }
}
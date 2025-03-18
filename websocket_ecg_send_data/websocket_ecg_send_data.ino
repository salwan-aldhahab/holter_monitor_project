#include <WiFi.h>
#include <vector>
#include <ArduinoWebsockets.h>

using namespace websockets;

const char* ssid = "Sal-S23";        // Replace with your WiFi SSID
const char* password = "S6478987757A"; // Replace with your WiFi password

WebsocketsServer server;
std::vector<WebsocketsClient> clients;

#define ECG_PIN 34  // GPIO34 - AD8232 OUT

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("Connected! IP Address: ");
    Serial.println(WiFi.localIP());

    // Start WebSocket Server
    server.listen(81);
    Serial.println("WebSocket Server Started on port 81");
}

void loop() {
    server.poll();  // Handle new WebSocket connections

    // Check for new clients
    if (server.available()) {
        WebsocketsClient newClient = server.accept();
        clients.push_back(newClient);
        Serial.println("New WebSocket Client Connected");
    }

    int ecg_value = analogRead(ECG_PIN);  // Read ECG data
    String message = String(ecg_value);

    // Send ECG data to all connected clients
    for (auto& client : clients) {
        if (client.available()) {
            client.send(message);
        }
    }

    Serial.println("ECG Sent: " + message);
    delay(50);  // Adjust this delay for real-time streaming
}
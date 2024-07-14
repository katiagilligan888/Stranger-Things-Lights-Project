#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

// WiFi credentials
const char* ssid = "";
const char* password = "";

// LED strip settings
#define LED_PIN     18 
#define NUM_LEDS    26
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// Web server on port 80
WebServer server(80);

void setup() {
    // Initialize serial and wait for port to open
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting setup...");

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("Connected to WiFi. IP address: ");
    Serial.println(WiFi.localIP());

    // Setup LED strip
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    Serial.println("LED setup complete.");

    // Define web server routes
    server.on("/", HTTP_GET, []() {
        String html = "<html><head><title>LED Message Board</title></head><body>"
                      "<h2>LED Message Display</h2>"
                      "<form action=\"/show\" method=\"GET\">"
                      "Message: <input type=\"text\" name=\"msg\">"
                      "<input type=\"submit\" value=\"Send\">"
                      "</form></body></html>";
        server.send(200, "text/html", html);
    });

    server.on("/show", HTTP_GET, []() {
        String message = server.arg("msg");
        if (message.length() > 0) {
            displayMessage(message.c_str());
        }
        server.send(200, "text/plain", "Message received: " + message);
    });

    // Start the server
    server.begin();
    Serial.println("Server started.");
}

void loop() {
    // Handle client requests
    server.handleClient();
}

void displayMessage(const char* message) {
    FastLED.clear();
    for (int i = 0; message[i] != '\0'; i++) {
        displayLetter(message[i]);
        FastLED.show();
        delay(1000); 
    }
   
    delay(5000); 
   
    FastLED.clear();
    FastLED.show();
}

void displayLetter(char letter) {
    int ledIndex = getLEDIndexForLetter(letter);
    if (ledIndex != -1) {
        leds[ledIndex] = CRGB::White;
    }
}

int getLEDIndexForLetter(char letter) {
    letter = toupper(letter);
    if (letter < 'A' || letter > 'Z') {
        return -1;
    }
    return letter - 'A';
}

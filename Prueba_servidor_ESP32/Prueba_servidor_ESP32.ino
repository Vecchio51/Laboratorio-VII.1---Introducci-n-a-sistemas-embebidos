#include <WiFi.h>
#include <WebServer.h>

// WiFi Network Credentials (Corregido)
const char* ssid = "Fran WiFi 2.4Ghz";
const char* password = "Fran247226782345";

WebServer server(80);

// Simple baseline HTML response stored in RAM
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'></head>";
  html += "<body style='background:#030704;color:#33ff33;text-align:center;font-family:monospace;padding-top:100px;'>";
  html += "<h1>NOOBIX BASIC NODE ONLINE</h1>";
  html += "<p style='color:#888;'>Connected via raw IP Address mapping.</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n========================================");
  Serial.println("         BASIC IP TEST BOOT              ");
  Serial.println("========================================");
  
  Serial.print("[WIFI] Seeking connection to: ");
  Serial.println(ssid);
  
  // Initialize the WiFi radio profile in station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Non-blocking wait loop prints dots until connection is acknowledged
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  
  // CRITICAL VIDEO SHOT: Print the raw IP node allocation out to the terminal [INDEX]
  Serial.println("\n\n[WIFI] Infrastructure bridge complete!");
  Serial.println("----------------------------------------");
  Serial.print("TARGET IP ADDRESS: ");
  Serial.println(WiFi.localIP()); 
  Serial.println("----------------------------------------");
  
  // Bind the root directory route and start the internal background server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("[OK] HTTP background daemon active on port 80.\n");
}

void loop() {
  server.handleClient(); // Keep processing local web query packets from the LAN
}


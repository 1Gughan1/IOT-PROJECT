#include <WiFi.h>
#include <WebServer.h>

#define ssid "Gughan's Room"
#define password "12345678"
#define relayPin1 4  // Switch 1
#define relayPin2 5  // Switch 2
#define buttonPin1 2  // Push button for relay1
#define buttonPin2 17  // Push button for relay2

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

bool relayStatus1 = HIGH;
bool relayStatus2 = LOW;
bool buttonState1 = HIGH;  // Assume button1 is not pressed initially
bool lastButtonState1 = HIGH;
bool buttonState2 = HIGH;  // Assume button2 is not pressed initially
bool lastButtonState2 = HIGH;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay = 50;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 1000; // Update every 1 second

void setup() {
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(1000);

  server.on("/", HTTP_GET, handleOnConnect);
  server.on("/turnon1", HTTP_GET, handleTurnOn1);
  server.on("/turnoff1", HTTP_GET, handleTurnOff1);
  server.on("/turnon2", HTTP_GET, handleTurnOn2);
  server.on("/turnoff2", HTTP_GET, handleTurnOff2);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.begin(115200);
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
  checkButton();

  unsigned long now = millis();
  if (now - lastUpdateTime >= updateInterval) {
    server.send(200, "text/plain", getStatusJSON());
    lastUpdateTime = now;
  }

  digitalWrite(relayPin1, relayStatus1);
  digitalWrite(relayPin2, relayStatus2);
}

void checkButton() {
  int reading1 = digitalRead(buttonPin1);

  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == LOW) {
        // Button1 is pressed
        if (relayStatus1 == HIGH) {
          relayStatus1 = LOW;
          Serial.println("Relay 1 Status: OFF");
          server.send(200, "text/html", getHTML());
        } else {
          relayStatus1 = HIGH;
          Serial.println("Relay 1 Status: ON");
          server.send(200, "text/html", getHTML());
        }
      }
    }
  }

  lastButtonState1 = reading1;

  int reading2 = digitalRead(buttonPin2);

  if (reading2 != lastButtonState2) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading2 != buttonState2) {
      buttonState2 = reading2;

      if (buttonState2 == LOW) {
        // Button2 is pressed
        if (relayStatus2 == HIGH) {
          relayStatus2 = LOW;
          Serial.println("Relay 2 Status: OFF");
          server.send(200, "text/html", getHTML());
        } else {
          relayStatus2 = HIGH;
          Serial.println("Relay 2 Status: ON");
          server.send(200, "text/html", getHTML());
        }
      }
    }
  }

  lastButtonState2 = reading2;
}

void handleOnConnect() {
  server.send(200, "text/html", getHTML());
}

void handleTurnOn1() {
  relayStatus1 = HIGH;
  Serial.println("Relay 1 Status: ON");
  server.send(200, "text/html", getHTML());
}

void handleTurnOff1() {
  relayStatus1 = LOW;
  Serial.println("Relay 1 Status: OFF");
  server.send(200, "text/html", getHTML());
}

void handleTurnOn2() {
  relayStatus2 = HIGH;
  Serial.println("Relay 2 Status: ON");
  server.send(200, "text/html", getHTML());
}

void handleTurnOff2() {
  relayStatus2 = LOW;
  Serial.println("Relay 2 Status: OFF");
  server.send(200, "text/html", getHTML());
}

void handleStatus() {
  server.send(200, "text/plain", getStatusJSON());
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

String getHTML() {
  String htmlCode = "<!DOCTYPE html><html lang='en'>";
  htmlCode += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  htmlCode += "<title>Gughan's room</title>"; // Updated title
  htmlCode += "<style>body{font-family:'Arial',sans-serif;margin:20px;text-align:center;}h1{color:#333;}p{font-size:18px;margin:10px;} .status{font-size:24px;color:#555;} .switch-status{display:inline-block;padding:10px 20px;margin:10px;text-decoration:none;font-size:18px;border-radius:5px;transition:background-color 0.3s;} .on{background-color:#4CAF50;color:white;} .off{background-color:#f44336;color:white;}</style>";
  htmlCode += "</head>";
  htmlCode += "<body>";
  htmlCode += "<h1>Gughan's room</h1>"; // Updated heading
  htmlCode += "<div class='status'>Light Status: <span id='relayStatus1' class='switch-status relay1 " + String(relayStatus1 ? "on" : "off") + "'>" + String(relayStatus1 ? "ON" : "OFF") + "</span></div>"; // Updated Light status
  htmlCode += "<div class='status'>Fan Status: <span id='relayStatus2' class='switch-status relay2 " + String(relayStatus2 ? "on" : "off") + "'>" + String(relayStatus2 ? "ON" : "OFF") + "</span></div>"; // Updated Fan status
  htmlCode += "<p><a class='switch-status on' href='#' onclick='sendCommand(\"turnon1\");'>Turn On Light</a></p>"; // Updated Turn On Light
  htmlCode += "<p><a class='switch-status off' href='#' onclick='sendCommand(\"turnoff1\");'>Turn Off Light</a></p>"; // Updated Turn Off Light
  htmlCode += "<p><a class='switch-status on' href='#' onclick='sendCommand(\"turnon2\");'>Turn On Fan</a></p>"; // Updated Turn On Fan
  htmlCode += "<p><a class='switch-status off' href='#' onclick='sendCommand(\"turnoff2\");'>Turn Off Fan</a></p>"; // Updated Turn Off Fan

  htmlCode += "<script>";
  htmlCode += "function sendCommand(command) {";
  htmlCode += "  var xhr = new XMLHttpRequest();";
  htmlCode += "  xhr.onreadystatechange = function() {";
  htmlCode += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  htmlCode += "      updateStatus(JSON.parse(xhr.responseText));";
  htmlCode += "    }";
  htmlCode += "  };";
  htmlCode += "  xhr.open('GET', '/' + command, true);";
  htmlCode += "  xhr.send();";
  htmlCode += "}";

  htmlCode += "function updateStatus(status) {";
  htmlCode += "  document.getElementById('relayStatus1').innerHTML = status.relayStatus1;";
  htmlCode += "  document.getElementById('relayStatus2').innerHTML = status.relayStatus2;";
  htmlCode += "}";

  htmlCode += "function pollStatus() {";
  htmlCode += "  var xhr = new XMLHttpRequest();";
  htmlCode += "  xhr.onreadystatechange = function() {";
  htmlCode += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  htmlCode += "      updateStatus(JSON.parse(xhr.responseText));";
  htmlCode += "    }";
  htmlCode += "  };";
  htmlCode += "  xhr.open('GET', '/status', true);";
  htmlCode += "  xhr.send();";
  htmlCode += "}";

  htmlCode += "setInterval(pollStatus, 1000);"; // Poll every 1 second
  htmlCode += "</script>";
  htmlCode += "</body></html>";

  return htmlCode;
}


String getStatusJSON() {
  String json = "{\"relayStatus1\":\"" + String(relayStatus1 ? "ON" : "OFF") + "\", \"relayStatus2\":\"" + String(relayStatus2 ? "ON" : "OFF") + "\"}";
  return json;
}

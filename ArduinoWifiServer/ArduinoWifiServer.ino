/*
 * Adapted from https://www.arduino.cc/en/Tutorial/WiFiNINASimpleWebServerWiFI
 * https://startingelectronics.org/tutorials/arduino/ethernet-shield-web-server-tutorial/web-server-read-switch-using-AJAX/
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>


#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

String HTTP_req;

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(4, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        HTTP_req += c;  // save the HTTP request 1 char at a time
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: keep-alive");
            client.println();
            
              if (HTTP_req.indexOf("flip_switch") > -1) {
                  CycleRelay(4);
              }
              else {  // HTTP request for web page
                  // send web page - contains JavaScript with AJAX calls
                  client.println("<!DOCTYPE html>");
                  client.println("<html>");
                  client.println("<head>");
                  client.println("<title>Arduino Web Page</title>");
                  client.println("<script>");
                  client.println("function GetSwitchState() {");
                  client.println("nocache = \"&nocache=\"\
                                                   + Math.random() * 1000000;");
                  client.println("var request = new XMLHttpRequest();");
                  client.println("request.onreadystatechange = function() {");
                  client.println("if (this.readyState == 4) {");
                  client.println("if (this.status == 200) {");
                  client.println("if (this.responseText != null) {");
                  client.println("document.getElementById(\"switch_txt\").innerHTML = this.responseText;");
                  client.println("}}}}");
                  client.println("request.open(\"GET\", \"flip_switch\" + nocache, true);");
                  client.println("request.send(null);");
                  client.println("}");
                  client.println("</script>");
                  client.println("</head>");
                  client.println("<body>");
                  client.println("<h1>Garage Door Opener</h1>");
                  client.println("<button type=\"button\"\
                      onclick=\"GetSwitchState()\">Push Button</button>");
                  client.println("</body>");
                  client.println("</html>");
              }
              HTTP_req = "";            // finished with request, empty string
              
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(4, HIGH);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);
          digitalWrite(4, LOW);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void CycleRelay(int pin)
{
          digitalWrite(LED_BUILTIN, HIGH);
          digitalWrite(pin, HIGH);
          delay(500);
          digitalWrite(LED_BUILTIN,LOW);
          digitalWrite(pin, LOW);
          delay(500);
}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

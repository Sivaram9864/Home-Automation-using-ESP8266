#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Sivaram";
const char* password = "chinnu123";

bool led1status = false;
bool led2status = false;
bool Buzzerstatus = false; 
bool Allstatus = false;
bool mobileConnected =false;

ESP8266WebServer server(80);

#define LED1_PIN 5
#define LED2_PIN 4
#define Buzzer 14

// ================================
// Main Web Page
// ================================

void handleRoot()
{
    String webpage = "";

    webpage += "<html>";
    webpage += "<head>";
    webpage += "<title>Sivaram Home Automation </title>";
    webpage += "</head>";

    webpage += "<body>";
    webpage += "<h1 style='text-align:center;'>ESP8266 Home Automation</h1>";
    webpage += "<h2 style='text-align:center;'>LED's CONTROL</h2>";

    addLedControl(webpage, 1);
    addLedControl(webpage, 2);

    webpage += "<h4 style='text-align:center;'>Buzzer</h4>";

    webpage += "<div style='display:flex; justify-content:center; gap:20px;'>";
    
    webpage += "<button onclick=\"fetch('/buzzer";
    webpage += "On')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "Buzzer ON";
    webpage += "</button>";

    webpage += "<button onclick=\"fetch('/buzzer";
    webpage += "Off')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "Buzzer OFF";
    webpage += "</button>";
    webpage += "</div>";

    webpage += "<h5 style='text-align:center;'> All </h4>";

    webpage += "<div style='display:flex; justify-content:center; gap:20px;'>";
    
    webpage += "<button onclick=\"fetch('/all";
    webpage += "on')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "All ON";
    webpage += "</button>";

    webpage += "<button onclick=\"fetch('/all";
    webpage += "off')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "All OFF";
    webpage += "</button>";

    webpage += "</body>";
    webpage += "</html>";

    server.send(200, "text/html", webpage);
}

void addLedControl(String &webpage, int ledNumber)
{
    webpage += "<h3 style='text-align:center;'>LED ";
    webpage += ledNumber;
    webpage += "</h3>";

    webpage += "<div style='display:flex; justify-content:center; gap:20px;'>";
    
    // LED ON0
    webpage += "<button onclick=\"fetch('/led";
    webpage += ledNumber;
    webpage += "on')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "LED ON";
    webpage += "</button>";
    
    // LED OFF
    webpage += "<button onclick=\"fetch('/led";
    webpage += ledNumber;
    webpage += "off')\" ";
    webpage += "style='width:200px; height:60px; font-size:20px;'>";
    webpage += "LED OFF";
    webpage += "</button>";
    
    webpage += "</div>";
}


// ================================
// LED ON
// ================================
void handleLED1On()
{
    if (led1status == false)
    {
        digitalWrite(LED1_PIN, HIGH);
        led1status = true;
        Serial.println("LED1 turned ON");
        printStatus();
    }
    else
    {
        Serial.println("LED1 is already ON");
    }

    server.send(200, "text/plain", "LED1 ON");
}

void handleLED1Off()
{
    if (led1status == true)
    {
        digitalWrite(LED1_PIN, LOW);
        led1status = false;
        Serial.println("LED1 turned OFF");
        printStatus();
    }
    else
    {
        Serial.println("LED1 is already OFF");
    }

    server.send(200, "text/plain", "LED1 OFF");
}

void handleLED2On()
{
    if (led2status == false)
    {
        digitalWrite(LED2_PIN, HIGH);
        led2status = true;
        Serial.println("LED2 turned ON");
        printStatus();
    }
    else
    {
        Serial.println("LED2 is already ON");
    }

    server.send(200, "text/plain", "LED2 ON");
}

void handleLED2Off()
{
    if (led2status == true)
    {
        digitalWrite(LED2_PIN, LOW);
        led2status = false;
        Serial.println("LED2 turned OFF");
        printStatus();
    }
    else
    {
        Serial.println("LED2 is already OFF");
    }

    server.send(200, "text/plain", "LED2 OFF");
}

void handleBuzzerOn()
{
    if(Buzzerstatus == false){
        digitalWrite(Buzzer, HIGH);
        Serial.println("Buzzer turned ON");
        Buzzerstatus=true;
        printStatus();
    }
    else{
        Serial.println("Buzzer is already ON");
    }

    server.send(200, "text/plain", "Buzzer ON");
}

void handleBuzzerOff()
{
    if(Buzzerstatus == true){
        digitalWrite(Buzzer, LOW);
        Serial.println("Buzzer turned OFF");
        Buzzerstatus=false;
        printStatus();
    }
    else{
        Serial.println("Buzzer is already OFF");
    }

    server.send(200, "text/plain", "Buzzer Off");
}

void handleAllOn()
{
    if(led1status == false || led2status == false || Buzzerstatus == false){
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(Buzzer, HIGH);
        led1status=true;
        led2status=true;
        Buzzerstatus=true;

        Serial.println("All turned ON");
        //Allstatus=true;
        printStatus();
    }
    else{
        Serial.println("All are already ON");
    }

    server.send(200, "text/plain", "All turned ON");
}

void handleAllOff()
{
    if(led1status == true || led2status == true || Buzzerstatus == true){
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(Buzzer, LOW);
        led1status=false;
        led2status=false;
        Buzzerstatus=false;

        Serial.println("All turned OFF");
        //Allstatus=false;
        printStatus();
    }
    else{
        Serial.println("All are already OFF");
    }

    server.send(200, "text/plain", "All turned OFF");
}

void printStatus()
{
    Serial.print("LED1: ");
    Serial.print(led1status);
    Serial.print("   ");  // 3 spaces
    Serial.print("LED2: ");
    Serial.print(led2status);
    Serial.print("   ");
    Serial.print("Buzzer: ");
    Serial.println(Buzzerstatus);
}

void checkWiFiConnection()
{
    int devices = WiFi.softAPgetStationNum();

    if (devices == 0 && mobileConnected == true)
    {
        mobileConnected = false;

        Serial.println("Mobile Wi-Fi disconnected!");
        Serial.println("Please connect your mobile to ESP8266 Wi-Fi...");
    }
    else if (devices > 0 && mobileConnected == false)
    {
        mobileConnected = true;

        Serial.println("Mobile connected successfully!");
    }
}

// ================================
// SETUP
// ================================

void setup()
{
    Serial.begin(115200);

    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(Buzzer, LOW);

    // Start ESP8266 Wi-Fi
    WiFi.softAP(ssid, password);

    Serial.println();
    Serial.println("ESP8266 Wi-Fi Started");

    Serial.print("Wi-Fi Name: ");
    Serial.println(ssid);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());


    // Tell user to connect
    Serial.println("Connect your mobile to the Wi-Fi...");


    // ==========================================
    // WAIT UNTIL MOBILE CONNECTS
    // ==========================================

    while (WiFi.softAPgetStationNum() == 0)
    {
        delay(100);
    }

    mobileConnected=true;

    // Mobile has connected
    Serial.println("Mobile connected!");


    // ==========================================
    // START WEB SERVER
    // ==========================================

    server.on("/", handleRoot);

    server.on("/led1on", handleLED1On);
    server.on("/led2on", handleLED2On);
    
    server.on("/led1off", handleLED1Off);
    server.on("/led2off", handleLED2Off);

    server.on("/buzzerOn", handleBuzzerOn);
    server.on("/buzzerOff", handleBuzzerOff);
    
    server.on("/allon", handleAllOn);
    server.on("/alloff", handleAllOff);
    
    server.begin();

    Serial.println("Web Server Started");
    Serial.println("Open http://192.168.4.1");
}


// ================================
// LOOP
// ================================

void loop()
{
    server.handleClient();
    checkWiFiConnection();
}
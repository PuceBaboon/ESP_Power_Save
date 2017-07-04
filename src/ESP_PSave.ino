/*
 *  $Id: ESP_PSave.ino,v 1.5 2017/07/04 01:21:48 gaijin Exp $
 *
 * Demonstrate switching the ESP8266 WiFi off and back on again,
 * while leaving all other functions operational.
 *
 * The actual stop/start sequence using the default "modem
 * sleep" mode came from the discussion at:-
 *
 *    https://github.com/esp8266/Arduino/issues/644
 *
 *       ...and I'd like to thank everyone who contributed
 * to that particular thread.
 *
 * I would suggest running this with the ESP8266 console in 
 * one window, while running a continuous ping to the ESP's
 * IP address in another window.
 *
 * This version is configured with a static IP address (and
 * netmask, gateway and DNS server), which need to be configured
 * in the user_config.h for -your- specific network.  You can
 * remove all of them and use DHCP instead, if you prefer it.
 * The static address version may be very marginally faster to
 * connect though, if that is an issue for your application.
 *
 * The embedded web server (used for current consumption
 * testing) is just a slightly cut-down copy of the "FSWebServer"
 * SPIFFS/WebServer example from the standard ESP8266WebServer
 * library examples, which is Copyright (c) 2015 Hristo Gochkov.
 *
 * Data for the web server is stored in SPIFFS on the ESP and
 * the SPIFFS is created from the contents of the "data"
 * directory.  Using PlatformIO this is achieved using:-
 *
 *           platformio run -t buildfs
 *           platformio run -t uploadfs
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "user_config.h"
/* *INDENT-OFF* */
extern "C" {
    #include "user_interface.h"
}
/* *INDENT-ON* */

WiFiClient wifiClient;
ESP8266WebServer server(80);

/* NOTE:- All of these values are configured in user_config.h.  */
const byte ip[] = WIFI_IPADDR;
const byte gateway[] = WIFI_GATEWY;
const byte netmask[] = WIFI_NETMSK;
const byte dnssrv[] = WIFI_DNSSRV;

/*  Globals.  */
int conn_tries = 0;             // WiFi re-connect counter.
int count_count = 0;            // Count of counts.


/*
 * Turn the ESP8266 WiFi off by calling forceSleepBegin() with
 * no parameters (defaults to "Modem Sleep").
 */
bool WiFi_Off() {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    yield();

    /* 
     * It can take a while for the ESP to disconnect, so we need
     * to give it a couple of seconds before returning a fail.
     */
    while ((WiFi.status() == WL_CONNECTED)
           && (conn_tries++ < WIFI_RETRIES)) {
        delay(100);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
    if (WiFi.status() != WL_CONNECTED)
        return (true);
    else
        return (false);
}


/*
 * Turn the WiFi back on again with forceSleepWake().
 * NOTE:-  =Your= SSID and password must be in the
 *         user_config.h file.
 */
bool WiFi_On() {
    conn_tries = 0;


    WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);
    wifi_station_connect();
    WiFi.begin(STA_SSID, STA_PASS, WIFI_CHANNEL, NULL);

    /* 
     * It may take a while for the ESP to re-connect, depending
     * upon external factors (e:- has the access-point moved out
     * of range since we were last connected?).  This loop will
     * give us a reasonable time to re-connect without blocking
     * permanently.
     */
    while ((WiFi.status() != WL_CONNECTED)
           && (conn_tries++ < WIFI_RETRIES)) {
        delay(100);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
    if (WiFi.status() == WL_CONNECTED)
        return (true);
    else
        return (false);
}


/*
 * Toggle the WiFi from on to off (or vice-versa).
 */
bool Toggle_WiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        return (WiFi_On());
    } else {
        return (WiFi_Off());
    }
}


/*
 * Display current WiFi status.
 */
void Stat_WiFi() {
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());
    Serial.println();
}


/* Just something to show that we're still alive. */
void Count_Loop() {
    int x = 0;
    bool odd;

    if (++count_count % 2) {
        odd = true;
    } else {
        odd = false;
    }

    for (x = 1; x <= 100; x++) {
        if (odd && !(count_count % 3)) {
            Serial.print(x, DEC);
        } else {
            if (odd) {
                Serial.print(x, BIN);
            } else {
                Serial.print(x, HEX);
            }
        }
        Serial.print(F(" "));
        delay(100);
    }
    Serial.println();
}


void prompt() {
    Serial.print(F("#> "));
}


void command_info() {
    Serial.println();
    Serial.println(F
                   (" c\t-- Count to 100 (show us you're doing something)."));
    Serial.println(F(" h or ?\t-- This help screen."));
    Serial.println(F(" s\t-- Display current WiFi status."));
    Serial.println(F(" w\t-- Toggle WiFi on/off."));
    Serial.println();
}


/*
 * SPIFFS support function to format bytes.
 */
String formatBytes(size_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

/*
 * HTTP server content types.
 */
String getContentType(String filename) {
    if (server.hasArg("download"))
        return "application/octet-stream";
    else if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/"))
        path += "AppleB.jpg";   // Default test file.
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        if (SPIFFS.exists(pathWithGz))
            path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void handleFileList() {
    if (!server.hasArg("dir")) {
        server.send(500, "text/plain", "BAD ARGS");
        return;
    }

    String path = server.arg("dir");
    Serial.println("handleFileList: " + path);
    Dir dir = SPIFFS.openDir(path);
    path = String();
    String output = "[";
    while (dir.next()) {
        File entry = dir.openFile("r");
        if (output != "[")
            output += ',';
        bool isDir = false;
        output += "{\"type\":\"";
        output += (isDir) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(entry.name()).substring(1);
        output += "\"}";
        entry.close();
    }
    output += "]";
    server.send(200, "text/json", output);
}


/*
 * Initialize the filesystem and the web server.
 */
void WebServerInit(void) {

    SPIFFS.begin(); {
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) {
            String fileName = dir.fileName();
            size_t fileSize = dir.fileSize();
            Serial.printf("FS File: %s, size: %s\r\n",
                          fileName.c_str(), formatBytes(fileSize).c_str());
        }
        Serial.printf("\n");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(F
                       ("ERROR:  WiFi not connected.  Cannot initialize web server."));
        return;
    }

    /*
     * HTTP Server Callbacks.
     */
    // Load from SPIFFS.
    server.onNotFound([]() {
                      if (!handleFileRead(server.uri()))
                      server.send(404, "text/plain", "File Not Found");}
    );

/* *INDENT-OFF* */
    //get heap status, analog input value and all GPIO statuses in one json call
    server.on("/all", HTTP_GET, [](){
      String json = "{";
      json += "\"heap\":"+String(ESP.getFreeHeap());
      json += ", \"analog\":"+String(analogRead(A0));
      json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
      json += "}";
      server.send(200, "text/json", json);
      json = String();
    });
/* *INDENT-ON* */

    /* ...and start the server. */
    server.begin();
    Serial.println("HTTP server started");
}


void setup() {
    Serial.begin(115200);

    delay(200);
    Serial.println(F("\n"));    // <CR> past boot messages.
#ifdef DEBUG
    Serial.print(F("Reset Info: "));
    Serial.println(ESP.getResetInfo());
    Serial.println(F("\n"));
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(STA_SSID);
#endif

    /*
     * An open-ended loop  here will flatten the battery on
     * the sensor if, for instance, the access point is 
     * down, so limit our connection attempts.
     */
    Serial.print("WiFi Stat: ");
    Serial.println(WiFi.status());      // Reputed to fix slow-start problem.
    WiFi.mode(WIFI_STA);
    WiFi.config(IPAddress(ip), IPAddress(gateway),
                IPAddress(netmask), IPAddress(dnssrv));
    WiFi.begin(STA_SSID, STA_PASS, WIFI_CHANNEL, NULL);
    yield();
    while ((WiFi.status() != WL_CONNECTED)
           && (conn_tries++ < WIFI_RETRIES)) {
        delay(100);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
    if (conn_tries == WIFI_RETRIES) {
#ifdef DEBUG
        Serial.println(F("No WiFi connection ...restarting."));
#endif
        ESP.reset();
    }
#ifdef DEBUG
    Stat_WiFi();
#endif

    WebServerInit();

    Serial.println(F("\n\r\n\rWiFi On/Off Test\r\n================"));
    Serial.println(F("Type \"h\" or \"?\" for command info."));
    command_info();
    prompt();
}


void loop() {
    char c;

    /*
     * Handle any HTTP requests.
     */
    server.handleClient();

    /*
     * Console menu.
     */
    while (Serial.available()) {
        c = Serial.read();
        switch (c) {

        case 'c':
        case 'C':
            Serial.println(F
                           ("Count to 100 (show us you're doing something):"));
            Count_Loop();
            prompt();
            break;
        case 'h':
        case 'H':
        case '?':
            command_info();
            prompt();
            break;
        case 's':
        case 'S':
            Serial.println(F("Status:"));
            Stat_WiFi();
            prompt();
            break;
        case 'w':
        case 'W':
            Serial.println(F("Toggle WiFi:"));
            if (!Toggle_WiFi()) {
                Serial.println(F("WiFi On/Off ==FAILED=="));
            } else {
                Stat_WiFi();
            }
            prompt();
            break;
        default:
            Serial.println(F("Help:"));
            command_info();
            prompt();
            break;
        }
    }
}

/*
 *  $Id: ESP_PSave.ino,v 1.3 2017/06/29 13:00:12 gaijin Exp $
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
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "user_config.h"
/* *INDENT-OFF* */
extern "C" {
    #include "user_interface.h"
}
/* *INDENT-ON* */

WiFiClient wifiClient;

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
    Serial.println(F(" w\t-- Toggle WiFi on/off."));
    Serial.println(F(" s\t-- Display current WiFi status."));
    Serial.println();
}


void setup() {

    Serial.begin(115200);

    delay(200);
    Serial.println(F("\n"));    // <CR> past boot messages.
#ifdef DEBUG
    Serial.print(F("Reset Info: "));
    Serial.println(ESP.getResetInfo());
    Serial.println(F("\n"));
#endif

#ifdef DEBUG
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
    Serial.println();
    WiFi.printDiag(Serial);
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());
#endif

    Serial.println(F("WiFi On/Off Test\r\n==============="));
    Serial.println(F("Type \"h\" or \"?\" for command info."));
    command_info();
    prompt();
}


void loop() {
    char c;

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

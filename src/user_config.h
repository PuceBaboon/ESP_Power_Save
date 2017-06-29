/*
 * $Id: user_config.h,v 1.4 2017/04/10 14:44:00 gaijin Exp $
 *
 * Configuration for local set-up (ie:- Access-point ID & PWD).
 */

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__


/*
 * GENERAL SETUP
 */
#define DEBUG		1		// Verbose output. "#undef" to disable.
// #undef DEBUG


/*
 * WIFI SETUP
 *
 * Number of times to test WiFi connection status before 
 * giving up.
 */
#define WIFI_RETRIES	30

/* Assign a static IP address to your ESP8266 */
#define WIFI_IPADDR	{ 192, 168, 1, 4 }
#define WIFI_NETMSK	{ 255, 255, 255, 0 }
#define WIFI_GATEWY	{ 192, 168, 1, 51 }
#define WIFI_DNSSRV	{ 192, 168, 1, 39 }
#define WIFI_CHANNEL	6		// !!You MUST change this!! -- Restart might be unreliable without this set.

#define STA_SSID	"Ziggy999"	// !!You MUST change this!!
#define STA_PASS	"*********"	// !!You MUST change this!!
#define STA_TYPE	AUTH_WPA2_PSK


#endif		// __USER_CONFIG_H__

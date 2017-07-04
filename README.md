

# ESP_Power_Save

   This test program demonstrates a method for saving power when using
   the ESP8266, by implementing on/off switching of the ESP8266 WiFi
   without affecting any of the module's other functionality.

   This power saving method was developed by members of the ESP8266 community
   and is outlined in the submissions to 
   [GitHub issue #644 of the ESP8266/Arduino project] (https://github.com/esp8266/Arduino/issues/644)
   by users "torntrousers", "hallard", "chaeplin", "Links2004" and others.
   To them go the accolades, to me only the blame if I've misinterpreted
   or misrepresented their work.


## Important

   This method does -not- put the microcontroller to sleep and requires
   neither an interrupt nor a reset to turn the WiFi back on again.
   You don't need to modify your hardware in any way to use it.

   The ESP8266 consumes only about one tenth of the power (very roughly)
   with the WiFi off, compared with having the WiFi switched on and
   transmitting, so the power savings are considerable.  However, this
   method may still not be suitable for applications where the battery
   capacity is limited, as even with the WiFi switched off, the ESP still
   draws in the region of 20mA (in the same ball park as a constantly
   illuminated, high brightness LED).

   This program shows how to use this method to reliably and asynchronously
   disable and re-enable WiFi from your own application.


## Functionality and Use

   This program was built using PlatformIO, but should be directly transferable
   to the Arduino IDE and (with a small amount of work) to the standard SDK.

   Before attempting to use this on your own network, you **need to update**
   the IP addresses, access-point SSID and password in the user\_config.h
   file.

   The ESP\_Power\_Save.ino contains two main subroutines, WiFi\_Off() and 
   WiFi\_On() which are called from the standard Arduino loop() to give the
   user control of the WiFi.

   There is a simple menu system using single character input to provide
   user control from the ESP console connection.  The menu commands are:-

   + "c" for "count".  This simply counts from 1 to 100 to demonstrate to
     the user that the ESP8266 core is still running.
   + "h" or "?" (or any other character which the menu doesn't understand) 
     will display help information (basically this table).
   + "s" will display the current WiFi status (including AP UID and password, 
     so you have been warned).
   + "w" will toggle the WiFi on and off.

   The WiFi toggle function has a delay loop which will terminate as soon as
   the requested operation completes successfully, or timeout after a preset
   time of 3 seconds and produce an error message if the toggle operation
   didn't complete for whatever reason.

   I would recommend opening two windows when using this; one connected to
   the ESP8266 console to use the command menu and a second to set up a
   repeated "ping" to your ESP's IP address, so that you can verify that
   the WiFi is actually off or on.

#### Update

   As of 4th July 2017, the program also includes a web server and a
   data directory (with a fairly substantial JPEG file) for testing.
   To have the ESP serve the JPEG image, you can connect to the ESP
   (with a web browser, or wget, or curl) using "http://\<IP ADDRESS\>/"
   (where "\<IP ADDRESS\>" is the IP address you've assigned to your
   module, obviously).  The JPEG is approximately 750Kb, so it will take
   a while for the ESP to grab it from SPIFFS, chunk it up and spit it
   out, giving enough time to measure the current consumption of the
   ESP while busy and transmitting.

   If the WiFi is turned off when you submit your query, almost all
   browsers (and wget and curl) will normally continue to retry for
   a while and the JPEG should be served as soon as the WiFi is
   toggled back on.


### Building

   The data file for the web server is stored in the "data" directory.
   This data needs to be loaded into SPIFFS (the SPI Flash File System)
   on the ESP8266.  This is accomplished under PlatformIO using these
   two commands:-

                    platformio run -t buildfs
                    platformio run -t uploadfs

   Note that the upload takes quite a while, as the whole filesystem
   is re-written.
 
### Caution

   If you try to connect to a non-existent access-point, or one which is
   out of range, or you use an incorrect password, then the WiFi will
   *not* turn off.  That seems pretty counter-intuitive to me and I
   haven't yet worked out why it happens.  At any rate, it's obviously
   worth doing some testing (I use a hand-held ESP8266 rig running on
   three AA batteries) to make sure that you have connectivity before
   deploying this power-save code to your remote units.


## Background

   The ultimate goal of this project is to reduce the power consumption
   of an ESP8266 to the point where it is practicable to use it for
   control of a solar battery charger, giving constant, real-time control
   while still being capable of sending charger data periodically to the
   (remote) main network.

   The battery is a standard, lead-acid car battery with a capacity
   of about 35Ah.  The solar panel is a 21v (max) output unit with
   a peak current capability of 300mA, so this is a trickle charger,
   designed specifically for very long term use where the main issue is
   the possibility of over-charging and "gassing", due to the long hours
   of strong sunlight we have during a typical Japanese summer.

   The battery provides power to a low volume, impeller-type pump in an
   outdoor sump well for orchard irrigation use.  The pump is used only
   intermittently during the summer and autumn and is removed from the
   well completely during the winter months.

   Obviously, with a wireless-enabled current of around 350ma, the ESP
   would have the capability (if it were transmitting continuously)
   of discharging the battery completely in about four and a half days 
   (or of soaking up the entire output of the solar panel, even on the 
   sunniest of days).

   Switching the WiFi off completely and using the ESP as a non-networked
   microcontroller can reduce the steady current requirement to less than
   20mA, giving us a theoretical battery life of slightly less than two
   and a half months (and consuming less than 1/10th of the solar panel
   output on a sunny day).

   *All of the figures above are back-of-the-envelope and extremely
   optimistic.  Real world conditions always tend to be (a lot!) less
   than ideal, but even so, this project should provide both a workable
   protection method for the battery and a steady stream of loggable
   status data (not to mention yet another attack vector for my home
   network).  Batteries not included.  Some assembly (but no assembler)
   required.  May contain nuts.*



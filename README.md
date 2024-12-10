# Garage Door Sensor

this projects checks whether the garage door is open or closed using a reed contact with a magnet mounted on the garage door.
2 LEDs are turned on if the garage door is open. The builtin LED indicates an active wifi connection.

Additionally, the current state of the door will be send to a webserver every 15 min or when the state changes.

## setup
you need to create a file called "wifi_credentials.h" following the example file located under src. It contains the wifi_credentials used by the microcontroller to connect to the wifi.
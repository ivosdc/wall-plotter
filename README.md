# wall-plotter
esp-8266 wall plotter

This project is inspired by https://www.hackster.io/fredrikstridsman/stringent-the-15-wall-plotter-d965ca - good work!

### Why?
Time goes by... I have no Arduino left. But there are 3 Node-MCU boards in my garage.

My needs were to use a Node-MCU board instead of a *poor* Arduino. In the ongoing project we will:

### Hardware:
- 1 x NodeMCU 12e (ESP8266)
- 2 x step motor 28BYJ-48
- 1 x case (https://github.com/snebragd/stringent/blob/master/Hardware/)
- 2 x spools (https://github.com/snebragd/stringent/blob/master/Hardware/spool2.stl)
- 1 x Servo SG90
- min. 3 meters of fishing line. Depends on your wanted canvas size.
- a pen
- rubber band

### Spawns own WiFi when no WiFi is reachable. 
 - you can enter your own WiFi parameter. Just POST a json ``` {"ssid":"MY-SSID","password":"PASSWORD"} ``` to the server.
 *You need a rest-api plugin for your browser or a tool like postman!*
 - Default IP from own **Accespoint 192.168.0.1**
 - Your uploaded WiFI credentials are stored in a persistant config.json

### Convert "gimp"svgs to plot data 
 - cd svg-converter
 - node svg2json.js svg/vws.svg
 This will create a `wall-plotter.data` file for upload.

### Upload plots with WiFi 
 - Upload `wall-plotter.data` to ESP8266 `/upload`. A webform for file upload is presented.

### Set configuration parameter
- Change the output size. POST ``` {"zoomFactor":"1"} ``` to `/zoom`. 1 (no zoom) is default. 
 *You need a rest-api plugin for your browser or a tool like postman!*
- Set canvas width and the position of the wall-plotter. POST ```{"canvasWidth":"1000","currentLeft":"330","currentRight":"999","zoomFactor":"1"}``` to `/config`.
 
### Used libraries:
- https://github.com/rydepier/Arduino-and-ULN2003-Stepper-Motor-Driver
- ArduinoJson6

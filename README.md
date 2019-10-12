# wall-plotter
esp-8266 wall plotter

This project is inspired by https://www.hackster.io/fredrikstridsman/stringent-the-15-wall-plotter-d965ca - good work!

### Why?
Time goes by... I have no Arduino left. But there are 3 Node-MCU boards in my garage.

My needs were to use a Node-MCU board instead of a *poor* Arduino. In the ongoing project we will:

- build converters for 2D g-code templates and / or SVGs to wall-plotter.json format. Started with SVGs-"M" path conditions to wall-plotter.json. Have a look at the cli tool svg2json
- create a basic frontend / SPA
- setup zoom-factor, start/zero point, canvas width.
- preprocess data (errors, preview)

### Hardware:
- 1 x NodeMCU 12e (ESP8266)
- 2 x step motor 28BYJ-48
- 1 x case (I used the packaging of the motors)
- 2 x spools (https://github.com/snebragd/stringent/blob/master/Hardware/spool2.stl)
- 1 x Servo SG90
- min. 3 meters of fishing line. Depends on your wanted canvas size.
- a pen
- rubber band, 2 screws, a small plywood plate as pen holder

### Upload plots with WiFi 
 - POST plot-json data to ESP8266 (http://your_server/plot/)

### Used libraries:
- https://github.com/rydepier/Arduino-and-ULN2003-Stepper-Motor-Driver
- ArduinoJson6
- Servo

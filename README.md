# wall-plotter
esp-8266 wall plotter

This project is inspired by https://www.hackster.io/fredrikstridsman/stringent-the-15-wall-plotter-d965ca - good work!

### Why?
Time goes by... I have no Arduino left. But there are 3 Node-MCU boards in my garage.

My needs were to use a Node-MCU board instead of a *poor* Arduino. In the ongoing project we will:

### Upload wall-plotter.json with WiFi 
 - Convert SVGs-"M" path conditions to wall-plotter.json with cli tool svg2json
 - POST plot-json data to ESP8266 (http://your_server/plot/)

### Used libraries:
- https://github.com/rydepier/Arduino-and-ULN2003-Stepper-Motor-Driver
- ArduinoJson6
- Servo

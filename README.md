# wall-plotter
esp-8266 wall plotter

This project is inspired by https://www.hackster.io/fredrikstridsman/stringent-the-15-wall-plotter-d965ca - good work!

### Why?
Time goes by... I have no Arduino left. But there are 3 Node-MCU boards in my garage.


My needs were to use a Node-MCU board instead of a *poor* Arduino. In the ongoing project we will:

 - activate it's W-LAN capabilities: Setup a HTTP-Server
 - create an API
 - POST the coordinates as JSON.
 - ... do further senseful things like scaling, starting, stopping, pausing, ... etc.
 - idea: distribute JSON data via MQTT?  


### Used libraries:
- https://github.com/rydepier/Arduino-and-ULN2003-Stepper-Motor-Driver
- ArduinoJson6
- Servo

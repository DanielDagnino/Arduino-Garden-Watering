# arduino_orchard_watering
This is a project to water an urban orchard using an arduino. For this purpose I have used an electro-valve to control the watering, a sensor to measure the temperature and humidity, a LCD display to write information for the user, two buttons to modify the watering time and to open the LCD light and a Real Time Clock.
The main characteristics of these components, the connections to the arduino and the code are the following:
* The electro-valve requires a 220V current to work so that, it is connected to a relay. The relay is controlled by the arduino.
* The watering of the plants is done at sunrise and 3h before sunset. The watering time is calculated based on equations from Astronomical Algorithms, by Jean Meeus, it requires the local latitude and longitude and the current day. For more information:
   - https://en.wikipedia.org/wiki/Position_of_the_Sun
   - https://www.esrl.noaa.gov/gmd/grad/solcalc/calcdetails.html
* A DHT 11 sensor to measure the temperature and humidity. The code also stopped the watering if the intraday temperature has arrived at 94%. However, I have to mentioned that in my case, the electric-box with all the instrumentation except the DHT sensor that is outside is in a small room opened to the exterior, and in that circumstances the humidity is always bellow the real one so that, it does not work properly.
* An LCD display to show the current time, the next watering time, the temperature, the humidity and the maximum humidity intraday. In case of using the button to change the watering duration it will show this time duration. And during the watering it displays the remaining time to finish the watering.
* A Real Time Clock DS1307 is used in order to avoid loosing the current time due to a power cut.
* Two buttons: One to change the watering period and the other to switch on/off the background LCD light.

The test circuit is show bellow (the valve is substituted by a light):
![alt text](https://raw.github.com/DanielDagnino/arduino_orchard_watering/master/img/test.jpg?raw=true "Circuit")

The display:
![alt text](https://raw.github.com/DanielDagnino/arduino_orchard_watering/master/img/lcd.jpg?raw=true "Display")

The final circuit with the vale:
![alt text](https://raw.github.com/DanielDagnino/arduino_orchard_watering/master/img/valve.jpg?raw=true "Valve")


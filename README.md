# arduino_orchard_watering
This is a project to water an urban orchard using an arduino. For this purpose I have used an electro-valve to control the watering, a sensor to measure the temperature and humidity, a LCD display to write information for the user and two buttons to modify the watering time and open the LCD light.
The main characteristics of these components, the connections to the arduino and the code are the following:
* The electro-valve requires a 220v current to works so that, it is connected to a relay. The relay is connected to the XXX pin of the arduino to control the current to the electro-valve.
* The watering of the plants is done at sunrise and 3h before sunset. The watering time is calculated based on equations from Astronomical Algorithms, by Jean Meeus, it requires the local latitude and longitude and the current day. For more information:
   - https://en.wikipedia.org/wiki/Position_of_the_Sun
   - https://www.esrl.noaa.gov/gmd/grad/solcalc/calcdetails.html
* A DHT sensor to measure the temperature and humidity. The code also stopped the watering if the intraday temperature has arrived at 94%. However, I have to mentioned that in my case the electric-box with all the instrumentation except the DHT sensor that is outside is inside a small room opened to the exterior and in that circumstances the humidity is always bellow the real one so that, it does not work properly.
* An LCD display to show the current time, the next watering time, the temperature, the humidity and the max humidity intraday. In case of using the button to change the watering duration it will show this time duration. And during the watering it displays the remaining time to finish the watering.

![alt text](https://raw.github.com/DanielDagnino/arduino_orchard_watering/master/IMG_20170430_175309.jpg?raw=true "Circuit")
![alt text](https://raw.github.com/DanielDagnino/arduino_orchard_watering/master/IMG_20170430_175423.jpg?raw=true "Display")

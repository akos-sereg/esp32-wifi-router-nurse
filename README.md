# esp32-wifi-router-nurse

Sometimes I have to restart my wifi router because for some reason my phone can not connect to the router. After router restart, everything works fine. I usually just unplug the power supply, then plug it back again.
This procedure can be automated by this little device.
Components:
- Esp32 device that connects to my local wifi network, monitoring wifi link status
- Relay switch that disconnects the power source of router for 3 seconds when wifi is down
- 5V DC/DC step-down converter as power supply of the esp32 microcontroller (router works with 12V).

![Nurse](https://raw.githubusercontent.com/akos-sereg/esp32-wifi-router-nurse/master/docs/wifi-nurse.png)

## Circuit

```
   ESP32
 __________          +3.3V
|          |           |
|          |      [ 3V relay ]
|          |           |
|          |           | (c)
| GPIO 16  |----(b)[ BC 182 ]
|__________|           | (e)
                      GND

```
    

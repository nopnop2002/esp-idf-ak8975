# esp-idf-ak8975
Display the orientation of AK8975 with ESP32.

First, find the offset value for each axis.   
As you can see, the X and Y axes are quite off-center.   
![ak8975-calib-1](https://user-images.githubusercontent.com/6020549/229248487-4ef39253-7bc9-4c86-a70f-0af2390a5973.jpg)

And display the orientation.   
![ak8975-heading-1](https://user-images.githubusercontent.com/6020549/232190029-ea4f85d6-8a32-46f7-9d71-04c42389296e.jpg)

# Software requiment
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   
ESP-IDF V5.1 is required when using ESP32-C6.   

# Hardware requirements
AK8975 3-axis Electronic Compass   

# Wireing
|AK8975||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6||
|:-:|:-:|:-:|:-:|:-:|:-:|
|VCC|--|3.3V|3.3V|3.3V||
|GND|--|GND|GND|GND||
|SCL|--|GPIO22|GPIO12|GPIO5|(*1)|
|SDA|--|GPIO21|GPIO11|GPIO4|(*1)|
|SO|--|N/C|N/C|N/C||
|CSB|--|3.3V|3.3V|3.3V|use i2c|
|DRDY|--|N/C|N/C|N/C||
|CAD1(CADY)|--|GND|GND|GND|(*2)|
|CAD0|--|GND|GND|GND|(*2)|

(*1)You can change it to any pin using menuconfig.   

(*2)Choosing an i2c address.
|CAD1|CAD0|Address|
|:-:|:-:|:-:|
|GND|GND|0x0C|
|GND|3.3V|0x0D|
|3.3V|GND|0x0E|
|3.3V|3.3V|0x0E|


# Caribration

```
git clone https://github.com/nopnop2002/esp-idf-ak8975
cd esp-idf-ak8975/calibrate
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```


### Configuration   
To find the offset value, set the compass offset to 0.   
![config-top](https://user-images.githubusercontent.com/6020549/229249348-21ca8f80-e976-4ddb-8bca-435c475a3290.jpg)
![config-app](https://user-images.githubusercontent.com/6020549/229249346-0da21399-9640-4708-bdb6-beed7549d55a.jpg)

### Execute calibration   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

As you move the compass it plots the X, Y and Z values.   
X, Y, Z offset are displayed.   

![ak8975-calib-1](https://user-images.githubusercontent.com/6020549/229249418-fb4431ed-ba92-42ba-9964-71711b448361.jpg)

### Execute calibration again   
If you set the offset you got from the calibration and run it again, the circle position will change.   

![ak8975-calib-2](https://user-images.githubusercontent.com/6020549/229249457-0ef43ec7-b7a6-42a4-bb99-2e4ffa76533c.jpg)


# Display the orientation   
```
git clone https://github.com/nopnop2002/esp-idf-ak8975
cd esp-idf-ak8975/heading
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```


### Configuration   
Sets the compass offset obtained by calibration.   
![config-top](https://user-images.githubusercontent.com/6020549/229249348-21ca8f80-e976-4ddb-8bca-435c475a3290.jpg)
![config-app](https://user-images.githubusercontent.com/6020549/229249346-0da21399-9640-4708-bdb6-beed7549d55a.jpg)


### View orientation   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

Click the mouse to change the display.   
![ak8975-heading-1](https://user-images.githubusercontent.com/6020549/232190029-ea4f85d6-8a32-46f7-9d71-04c42389296e.jpg)
![ak8975-heading-2](https://user-images.githubusercontent.com/6020549/232190031-57866343-e108-46f1-9d92-82666de43ae3.jpg)

WEB pages are stored in the html folder.   
I used [this](https://canvas-gauges.com/) for gauge display.   
All Configuration Options for the gauge display is [here](https://canvas-gauges.com/documentation/user-guide/configuration).   
You can change the design and color according to your preference.   

# Interface between EVLogger and sensors/vehicle

Logger samples ADC channels:  

* 6  - P6.6
* 7  - P6.7
* 12 - P7.0
* 13 - P7.1
* 14 - P7.2
* 15 - P7.3
* 5  - User potentiometer

6 and 7 are pins 2 and 14 on Pin Header J4  
12, 13, 14, 15 are pins 2, 4, 6, 8 on Pin Header J5  

For GPS, UCA1 is available on Pin Header J4, but this is shared with the ez430
programmer and requires removing/adding jumpers.  

* 1 - VCC
* 3 - RXD
* 5 - TXD
* 13 - GND

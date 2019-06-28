## About The Project
A reimplementation of the Arduino Wire library to allow connection to Amazon FreeRTOS Common IO library

## Locating Project
/demos/ti/cc3220_launchpad/common/application_code/

= Requirements, Targets and Design for Arudino Sensor Libraries with Common IO =

**Contents**

1. Objectives and Requirements
1. Decisions to Make
1. Features
1. HW Under Consideration
1. HW Decisions
1. Demo Planning
1. Project Schedule

= 1 Objectives and Requirements for Arduino Sensor Libraries to Work with Common IO Layer for a:FR =

== 1.1 Objective ==

The main objective of this project is to enable Arduino sensor libraries to work with the Common IO layer of a:FR. This is so that the wide customer base of Arudino Sensor Library can seamlessly use our team's Common IO library and seamlessly use our supported frameworks. Customers should be able to use our implementations of the Arduino libraries without having to change any code. 

== 1.2 Requirements ==

Supporting Arduino sensors to work in a:FR requires the library to be:

* flexible and generic enough to work across all the buses that are to be supported

From a usability perspective, our framework should

* allow customers to reuse Arduino sensor libraries without changing the driver code and compiling the Arduino drivers for low level buses and linking them with the Arduino driver's code

= 2 Decisions To Make =

== 2.1 Blocking vs. Non-blocking ==

6/18: Something to be considered~-~-depending on the implementation of the sensor, it may not be necessary to implement blocking and queuing of write results. However, according to the Ardunio Wire library, certain functions such as endTransmission() are expected to queue writes. 

However, in looking at the Arduino Wire implementation, they are able to perform certain reads with just blocking. For now, the library will be implemented with non-blocking, with potential for moving onto blocking. 

Update 6/24 on decision: Wire library has been written in blocking, with the ability to do non-blocking as necessary. Semaphores have been implemented, with callbacks after waiting for completion of a task. This allows for a more close version of the Arudino Wire library, with all expected functionalities now available. 

== 2.2 C vs. C++ ==

6/24: The Arduino Wire library is written in C++, while the Common IO library is written in C. This poses a series of fundamental issues with bridging the two, such as how the Wire library relies on C++ specific abilities such as function overloading or classes. Thus, we need to decide if our implementation of the Wire library is to be done in C or C++. If done in C, this will make the transition for customers less smooth, as the Wire code will not be able to be directly used. If written in C++, it requires another compiler for customers, which is also not ideal. 

= 3 Features =

In this section will analyze the features we intend to implement to support the scenarios presented above. 

== 3.1 Common IO I2C Implementation ==

(% style="color:inherit; font-family:inherit" %)Previously the I2C implementation of the Common IO library was not written. This project will start with implementing the Common IO implementation of I2C, following the conventions of the other existing versions, such as UART. As such, the same headers and functionalities are to be expected, including synchronous and asynchronous reads. 

[[image:1561670288146-334.png||height="450" width="493"]]

**Figure 1: **The boxes in green are demonstrating the layers to be implemented in this project. There are a few different ways users can communicate to the TI I2C library. If customers are choosing to use the Arduino library, previously, they were limited to the Arduino Wire library and its implementation. This confined them to only the drivers Arduino supports. By allowing customers to keep the same code but make calls to the a:FR library, this expands the customer's use case while also keeping them under the a:FR environment. 

== 3.2 Arduino Wire Library to A:FR Hal I2C ==

(% style="color:inherit; font-family:inherit" %)Code has been implemented so that users accustomed to the Arduino Wire library can make the same calls to directly connect to the A:FR library and connect to our supported devices.

== (% style="color:inherit; font-family:inherit" %)3.3 Connect to MQTT Cloud(%%) ==

Output of sensors should be able to be displayed on the MQTT console. 

= 4 HW Under Consideration =

For the purpose of establishing where we should start for which buses we should implement, we analyzed the offerings of Arduino sensor libraries available as of May 31st, 2019. We prioritized drivers that are both popular and appear in Bosch sensor libraries as well (to maximize use cases).The selected manufacturers and devices are 

* [[DHT11/DHT22>>https://github.com/adafruit/DHT-sensor-library]]
** The most commonly used temperature and humidity sensor for Arduino Projects
* [[ESP8266 WiFi Serial Connector>>https://github.com/esp8266/Arduino]]
** Used for connecting serial WiFi with the other sensors and the Arduino module, enables you to build a wireless web server-client platform
* [[Light Dependent Resistor (LDR)>>https://github.com/scorpHQ/LDR-and-Arduino]]
** Detects light in specific areas, photoresistor
* [[Ultrasonic distance sensor HC-SR04>>https://github.com/sparkfun/HC-SR04_UltrasonicSensor]]
** (((
This sensor automatically sends an audio signal of 40 kHz and detect whether there is a pulse signal back. It sends a signal and if received back it computes the distance traveled by the signal, thus the distance from the sensor to the object in front of the it. It can sense an object in the range: 2cm~~500cm
)))
* (((
[[BME280>>https://github.com/adafruit/Adafruit_BME280_Library]]

* The BME280 is an integrated environmental sensor developed specifically for mobile applications where size and low power consumption are key design constraints. The unit combines individual high linearity, high accuracy sensors for pressure, humidity and temperature in an 8-pin metal-lid 2.5 x 2.5 x 0.93 mm&sup3; LGA package, designed for low current consumption (3.6 μA @1Hz), long term stability and high EMC robustness.
)))
* (((
[[BMP680>>https://www.bosch-sensortec.com/bst/products/all_products/bme680]]
)))
* (((
[[BMI160>>https://github.com/hanyazou/BMI160-Arduino]]

* The BMI160 is a small, low power, low noise 16-bit inertial measurement unit designed for use in mobile applications like augmented reality or indoor navigation which require highly accurate, real-time sensor data.
)))
* (((
[[Nordic NRF52 BLE>>https://github.com/adafruit/Adafruit_nRF52_Arduino]]
)))
* (((
[[TLS2591>>https://github.com/adafruit/Adafruit_TSL2591_Library]]

* Digital light sensors
)))

**For now, we are considering that the DHT11 is the most popular Arduino sensor (but doesn't appear to have outputs that are SPI or I2C compatible), the BME680 has a variety of features which would make it good for a demo, and the BMP280 seems to be a good place to start because it has a non-blocking I2C/SPI library.**

**Table 1 - Considered devices from the Arduino Sensor Library**

(% border="1" %)
|**Manufacturer**|**Bus Name**|**Sensor Interfaces**|**Board**
|Adafruit|DHT11|None (Manchester-esque output)| 
|Espressif|ESP8266|SPI, I2C| 
| |LDR| |ArduinoUno, LaunchPadF5529
|ITEAD|HC-SR04| | 
|Bosch    |BME280|SPI 4-wire, I2C| 
|Bosch    |BME680|SPI, I2C| 
|Bosch    |BMP280|SPI, I2C| 
|Bosch|BMI160|I2C, SPI| 
|Nordic|NRF52|I2C, SPI| 
|Adafruit|TLS259|I2C| 

**Note about Bosch Sensors:**

**Table 2 - Bosch categorizes their available sensors in two main categories (motion sensors and environmental sensors): **

**Table 2.1: Arduino Motion Sensors**

|=**Motion Sensors**|= 
|Accelerometers|[[BMA222E>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA222E-DS004.pdf||target="_self"]], [[BMA250E>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA250E-DS004.pdf||target="_self"]], [[BMA253>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA253-DS000.pdf||target="_self"]], [[BMA255>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA255-DS004.pdf||target="_self"]], [[BMA280>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA280-DS000.pdf||target="_self"]],
[[BMA400>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA400-DS000.pdf||target="_self"]], [[BMA423>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA423-DS000.pdf||target="_self"]], [[BMA456>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMA456-DS000.pdf||target="_self"]]
|Gyroscopes|[[BMG160>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMG160-DS000.pdf||target="_self"]], [[BMG250>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMG250-DS000.pdf||target="_self"]]
|Geomagnetic Sensor|[[BMM150>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMM150-DS001.pdf||target="_self"]]
|eCompass|[[BMC050>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMC050-DS000.pdf||target="_self"]], [[BMC150>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMC150-DS000.pdf||target="_self"]], [[BMC156>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMC156-DS000.pdf||target="_self"]]
|IMUs|[[BMI055>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMI055-DS000.pdf||target="_self"]], [[BMI085>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMI085-DS001.pdf||target="_self"]], [[BMI088>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMI088-DS001.pdf||target="_self"]], [[BMI160>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMI160-DS000.pdf||target="_self"]], [[BMI270>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMI270-DS000.pdf||target="_self"]]
|Absolute Orientation Sensor|[[BMX055>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMX055-DS000.pdf||target="_self"]], [[BMX160>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMX160-DS000.pdf||target="_self"]]
|Hubs & Nodes|[[BHI160>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BHI160B-DS000.pdf||target="_self"]]
[[BHA250>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BHA250B-DS000.pdf||target="_self"]]
[[BNO055>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BNO055-DS000.pdf||target="_self"]]
[[BMF055>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMF055-DS000.pdf||target="_self"]]

**Table 2.2 Environmental Sensors**

|=**Environmental Sensors**|= 
|Pressure Sensors|[[BMP180>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP180-DS000.pdf||target="_self"]], [[BMP280>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001.pdf||target="_self"]], [[BMP388>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP388-DS001.pdf||target="_self"]]
|Integrated Environmental Unit|[[BME280>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf||target="_self"]], [[BME680>>url:https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680-DS001.pdf||target="_self"]]

= 5 HW Decisions =

We are going to be starting with the **BME680** because it is 1) closely related to the BME280 which the team is already familiar with 2) there are numerous features on it to create an engaging demo 3) it is a Bosch sensor, which maximizes cross application and use. From there, based on the knowledge gained, we can explore other sensor options, potentially outside of the Bosch library. 

The next sensors in order are:

* BME280 (for heavy use)
* BMI160 (interesting applications in gaming/augmented reality)
* DHT11 (will require extra implementation but is by far the most popular Arduino sensor)

= 6 Demo Planning =

Objective: To be able to showcase the Wire to CommonIO implementation, as well as the CommonIO implementation of the I2C library. 

Current working demo: Able to use the temperature sensor to show changes in output using the Wire library making calls to the CommonIO library.

Next step for demo: Connect to MQTT cloud, be able to show on dashboard. Be able to compose a story/narrative as a user.

= 7 Project Schedule =

(% border="1" %)
|**Week**|**Tasks**|**Status**
|Week 1|Familiarize with team codebase and Arduino library|Done
|Week 2|Inventory of Arduino libraries, and their synergies with Bosch|Done
|Week 3|Choose target board(s) (*) an familiarize with the native SDK(s) for those boards|Done
|Week 4|Import and build the Arduino drivers code for one of the selected sensors, build the common IO implementation for the needed bus (e.g. I2C)|Done
|Week 5|(((
(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)a.       Design distribution mechanism for code (could just be  directory in our repo)

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)b.       Enable build and tests (if applicable) 

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)c.       Verify tests do pass correctly

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)d.       Prepare end-of-sprint demo
)))|In Progress
|Week 6|(((
a.       (% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)Design distribution mechanism for code (could just be  directory in our repo)

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)b.       Enable build and tests (if applicable) 

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)c.       Verify tests do pass correctly

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)d.       Prepare end-of-sprint demo
)))| 
|Week 7|Move to the next sensor(s), may need one additional bus| 
|Week 8|Move to the next sensor(s), may need one additional bus| 
|Week 9|(((
a.        (% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)Evaluate porting more Arduino sensors or 

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)b.       Executing the same work with the one or more of the already supported sensor(s) with Bosch library
)))| 
|Week 10|(((
a.       (% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)Evaluate porting more Arduino sensors or 

(% style="-webkit-text-size-adjust:auto; caret-color:#000000; color:#000000; font-family:Calibri,sans-serif; font-size:11pt; font-style:normal; font-variant-caps:normal; font-weight:normal; letter-spacing:normal; orphans:auto; text-decoration:none; text-transform:none; white-space:normal; widows:auto; word-spacing:0px" %)b.       Executing the same work with the one or more of the already supported sensor(s) with Bosch library
)))| 
|Week 11|Wrap up and final demo| 
|Week 12| | 

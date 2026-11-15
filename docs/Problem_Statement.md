PROBLEM STATEMENT

Patient Health Monitoring System Using ESP32 and MQTT

1. Problem Statement
Monitoring a patient's health parameters regularly is important for identifying changes in their health condition. Traditional monitoring methods may require continuous manual observation and do not always provide an easy way to view sensor readings remotely.

The aim of this project is to develop an IoT-based Patient Health Monitoring System using an ESP32 microcontroller and MQTT protocol. The system collects temperature, humidity, and heart-rate-related readings through connected sensors and transmits the data to a cloud-based MQTT broker. A web-based dashboard is used to display the received readings for monitoring.
2. Objectives

> To collect sensor readings using ESP32.

> To measure temperature and humidity using the DHT11 sensor.

> To obtain heart-rate-related readings using the MAX30102 and HW-827 sensor modules.

> To transmit sensor data using the MQTT protocol.

> To use EMQX Cloud as the MQTT broker.

> To display received data on a web-based dashboard.

3. Proposed Solution

The proposed system uses ESP32 as the main controller to collect data from the connected sensors. The collected readings are published to the EMQX Cloud MQTT broker using the MQTT protocol. The web dashboard subscribes to the relevant MQTT topic and displays the received data in a user-friendly format.

4. Expected Outcome

The expected outcome is a working IoT-based prototype that can collect sensor readings, transmit them through MQTT, and display them on a web dashboard.

5. Project Scope

This project is developed for educational and demonstration purposes. It demonstrates the integration of sensors, ESP32, cloud-based MQTT communication, and a web dashboard. It is not intended to replace professional medical equipment or provide clinical diagnosis.

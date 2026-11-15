PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
[DEPARTMENT OF ELECTRONICS & COMMUNICATION ENGINEERING] 
[ABES ENGINEERING COLLEGE] 
[GHAZIABAD] 
SOFTWARE REQUIREMENTS 
SPECIFICATION 
Patient Health Monitoring System 
An ESP32-Based IoT Prototype with Cloud Dashboard 
Document detail 
Information 
Standard 
Version 
IEEE Std 830-1998 style academic SRS 
1.0 – Draft for academic review 
Academic year 
Degree / year 
2026–2027 
B.Tech, Third Year 
Prepared by 
Project guide 
Department / institute 
KUMAR GARV (2400320310096) 
[Supervisor name and designation] 
ELECTRONICS & COMMUNICATION ENGINEERING / ABES 
ENGINEERING COLLEGE 
Submission date 
15/11/2026 
Prepared for project planning, implementation, testing and academic evaluation. 
B.Tech Project Work  •  SRS  •  1 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
Document Revision and Approval History 
Version 
Date 
Description 
0.1 
01/10/2026 
Initial draft based on 
ESP32 and dashboard 
workflow 
Prepared by 
Reviewed by 
KUMAR GARV  
1.0 
15/11/2026 
Requirements baseline 
for academic review 
Project guide 
KUMAR GARV 
[Reviewer] 
This document follows the section structure of the supplied academic template. It describes the current prototype and 
distinguishes implemented functions from proposed enhancements. 
Table of Contents 
• 1. Introduction 
• 2. Overall Description 
• 3. System Features and Functional Requirements 
• 4. External Interface Requirements 
• 5. Non-Functional Requirements 
• 6. System Design and Analysis Models 
• 7. Academic Review and Sign-Off 
• Appendix A – Verification Checklist 
• Appendix B – Items to Confirm 
B.Tech Project Work  •  SRS  •  2 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
1. Introduction 
1.1 Purpose 
This Software Requirements Specification defines the requirements for the Patient Health Monitoring System, an 
educational Internet of Things (IoT) prototype. The system collects selected sensor measurements through an ESP32 
microcontroller, sends readings to Adafruit IO using MQTT, and presents received values on a web dashboard hosted with 
GitHub Pages. 
The document is intended for the student development team, project supervisor, evaluators and anyone reviewing or 
maintaining the prototype. It records expected behaviour, interfaces, constraints and tests for the current 
implementation. 
1.2 Scope of the System 
The project demonstrates remote viewing of selected environmental and pulse-related measurements. An ESP32 reads a 
DHT11 temperature/humidity sensor, a MAX30102 optical pulse sensor and an HW-827 pulse sensor. Firmware publishes 
supported readings to separate Adafruit IO feeds. The browser dashboard retrieves public feed values and displays the 
latest available measurements. 
In scope 
• Reading temperature and relative humidity from the DHT11 sensor. 
• Reading optical pulse data from the MAX30102 and deriving a heart-rate estimate when signal checks pass. 
• Reading the analogue output of the HW-827 and estimating pulse rate using the implemented threshold/timing 
method. 
• Connecting the ESP32 to Wi-Fi and Adafruit IO using MQTT over TLS. 
• Publishing readings to four Adafruit IO feeds: temperature, humidity, max30102-heart-rate and hw-827-heart-rate. 
• Retrieving public feed values in the GitHub Pages dashboard and presenting available readings. 
Out of scope 
• Clinical diagnosis, treatment recommendations or emergency response. 
• Certified medical-device measurement or clinically validated accuracy claims. 
• Patient account management, role-based access control or secure patient records unless implemented in a later 
version. 
• A relational database, Neon server or custom backend; these are not part of the described deployment. 
• SpO₂ reporting in the current firmware. It remains unavailable until a suitable algorithm is implemented and tested. 
• Guaranteed continuous monitoring when Wi-Fi, cloud service or power is unavailable. 
1.3 Definitions, Acronyms and Abbreviations 
Term 
Meaning 
ESP32 
Microcontroller board used to read sensors and communicate over 
Wi-Fi. 
DHT11 
MAX30102 
Digital temperature and relative-humidity sensor. 
Optical sensor module providing red and infrared measurements 
used for pulse-related estimation. 
HW-827 
MQTT 
Analogue pulse-sensor module used in this prototype. 
Message Queuing Telemetry Transport, a lightweight 
publish/subscribe protocol. 
Adafruit IO 
Cloud service used to receive, store and expose the project's feed 
data. 
Feed 
GitHub Pages 
Named Adafruit IO data stream associated with a measurement. 
Static website hosting used to publish the dashboard. 
B.Tech Project Work  •  SRS  •  3 
TLS 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
Telemetry 
Stale reading 
Transport Layer Security, used to protect the MQTT connection. 
Measurement data sent from the ESP32 to the cloud service. 
A value that has not been updated within the dashboard's 
configured freshness interval. 
SRS 
Software Requirements Specification. 
1.4 References 
• IEEE Std 830-1998, IEEE Recommended Practice for Software Requirements Specifications. 
• Espressif ESP32 Arduino Core: https://docs.espressif.com/projects/arduino-esp32/ 
• Adafruit IO documentation: https://io.adafruit.com/ 
• Adafruit IO MQTT API: https://io.adafruit.com/api/docs/mqtt.html 
• PubSubClient: https://pubsubclient.knolleary.net/ 
• SparkFun MAX3010x Sensor Library: https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library 
• GitHub Pages documentation: https://docs.github.com/en/pages 
1.5 Document Overview 
Section 2 describes the product context, users, operating environment and constraints. Section 3 lists functional 
requirements. Section 4 describes user, hardware, software and communication interfaces. Section 5 defines quality 
requirements and verification methods. Section 6 presents the system models. Section 7 provides approval fields. 
2. Overall Description 
2.1 Product Perspective 
The system has three main parts: an embedded sensing unit, a cloud messaging/data service, and a browser-based 
dashboard. The ESP32 publishes sensor readings. Adafruit IO receives MQTT messages and exposes feed values. The 
dashboard is a static web application hosted by GitHub Pages and reads public feed endpoints. 
Layer 
Components 
Sensing / device 
ESP32, DHT11, MAX30102, HW-827 
Responsibility 
Acquire samples, apply basic validity checks 
and prepare readings. 
Network / cloud 
Wi-Fi, MQTT over TLS, Adafruit IO 
Presentation 
HTML, CSS, JavaScript, GitHub Pages 
Transport and retain feed updates. 
Fetch public feed values and display 
readings and status. 
System context and data flow 
Step 
From → To 
Data / action 
1 
Sensors → ESP32 
Temperature, humidity, IR samples and HW
827 analogue samples. 
2 
ESP32 → Wi-Fi 
3 
ESP32 → Adafruit IO 
Connect to configured wireless network. 
Publish valid measurements to 
corresponding MQTT feed topics. 
4 
Adafruit IO → Dashboard 
Provide latest public feed values through its 
read API. 
5 
Dashboard → Viewer 
Display values, units, update status and 
unavailable/stale states. 
The current design does not include a separate application server or project database. Adafruit IO is the cloud feed service 
in the implemented data path. 
B.Tech Project Work  •  SRS  •  4 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
2.2 User Classes and Characteristics 
User class 
Expected familiarity 
Project operator / student developer 
Basic to intermediate technical skills 
Responsibilities / access 
Configure device, maintain firmware and 
feeds, inspect logs and update dashboard. 
Dashboard viewer 
Basic browser skills 
Open dashboard and view latest available 
readings. 
Project evaluator / guide 
General technical familiarity 
Review requirements, implementation 
evidence and test results. 
The current dashboard is intended for demonstration and read-only viewing. It should not be described as a patient 
identity or clinical record management platform. 
2.3 Operating Environment 
• Device: ESP32 development board with a supported Arduino-ESP32 core. 
• Sensors: DHT11, MAX30102 and HW-827 connected to the GPIO/I²C pins defined in firmware. 
• Firmware development: Arduino IDE with required ESP32 and sensor libraries. 
• Network: Wi-Fi with internet access; the selected ESP32 board and router must support the configured network. 
• Cloud service: Adafruit IO account and configured feeds; firmware uses MQTT over TLS on TCP port 8883. 
• Dashboard: Modern browser with JavaScript enabled; static files served through GitHub Pages over HTTPS. 
• Development computer: Windows PC or another system capable of running Arduino IDE and a modern browser. 
2.4 Design and Implementation Constraints 
• ESP32 requires Wi-Fi and internet connectivity to publish cloud telemetry. 
• Adafruit IO username, key and feed keys must match the configured account. Credentials must not be committed to 
a public repository. 
• The dashboard can read feeds without authentication only when those feeds are publicly accessible. 
• MQTT and dashboard polling must respect the service's applicable account limits. 
• Sensor readings depend on wiring, placement, sensor quality, ambient conditions and signal processing; software 
cannot guarantee clinical accuracy. 
• The current system has no local persistent database. Historical availability depends on the cloud feed service. 
• Power loss, Wi-Fi outage, cloud outage or browser connectivity loss can interrupt updates. 
2.5 Assumptions and Dependencies 
• Sensors are connected according to the verified project wiring and receive suitable power. 
• The device is configured with a valid Wi-Fi SSID and password. 
• Adafruit IO feeds exist and account credentials are valid. 
• Public feed data is accessible from the browser and cross-origin requests are permitted. 
• The dashboard viewer has internet access and a supported browser. 
• The project is an educational prototype and is not used to make clinical decisions. 
3. System Features and Functional Requirements 
Each requirement has a unique identifier. High priority requirements are necessary for the current demonstration; 
medium priority requirements support diagnostics and usability. 
3.1 Module 1: Sensor Acquisition 
ID 
Requirement 
Acceptance criteria 
FR-1.1 
ESP32 shall read temperature 
Priority 
Successful reads update both 
High 
B.Tech Project Work  •  SRS  •  5 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
B.Tech Project Work  •  SRS  •  6 
and humidity from DHT11. values; failed reads are not 
treated as new valid 
measurements. 
FR-1.2 
ESP32 shall acquire infrared 
samples from MAX30102 over 
I²C. 
Sensor initializes and samples 
are read during the main loop. High 
FR-1.3 
Firmware shall estimate heart 
rate from MAX30102 pulse 
events. 
Only values passing configured 
signal and rate checks are 
eligible for publishing. 
High 
FR-1.4 
ESP32 shall read the HW-827 
analogue output using its 
configured ADC pin. 
A numeric ADC sample is 
available to the pulse-detection 
routine. 
High 
FR-1.5 Firmware shall estimate pulse 
rate from HW-827 signal events. 
Estimate is produced only when 
beat timing passes configured 
validity checks. 
High 
 
3.2 Module 2: Connectivity and Cloud Publishing 
ID Requirement Acceptance criteria Priority 
FR-2.1 ESP32 shall connect to 
configured Wi-Fi. 
Serial output indicates 
connected or disconnected 
state. 
High 
FR-2.2 ESP32 shall connect to Adafruit 
IO using MQTT over TLS. 
Successful connection is 
reported; failed attempts are 
retried. 
High 
FR-2.3 
Firmware shall publish 
temperature to the temperature 
feed. 
Latest feed value matches the 
most recently published valid 
temperature. 
High 
FR-2.4 Firmware shall publish humidity 
to the humidity feed. 
Latest feed value matches the 
most recently published valid 
humidity. 
High 
FR-2.5 
Firmware shall publish 
MAX30102 heart rate to its 
dedicated feed when valid. 
No new heart-rate value is 
published when the reading is 
marked invalid. 
High 
FR-2.6 
Firmware shall publish HW-827 
heart rate to its dedicated feed 
when valid. 
No new heart-rate value is 
published when the reading is 
marked invalid. 
High 
FR-2.7 
Firmware shall retry network 
connections after connection 
loss. 
After network restoration, 
publishing resumes without 
manual reset, subject to service 
availability. 
High 
FR-2.8 
Firmware shall limit its 
publishing frequency to the 
configured schedule. 
Aggregate publishing rate 
remains within the applicable 
Adafruit IO account limit. 
High 
 
3.3 Module 3: Dashboard and Visualization 
ID Requirement Acceptance criteria Priority 
FR-3.1 
Dashboard shall retrieve the 
latest public temperature feed 
value. 
Temperature card displays value 
and unit, or an unavailable 
state. 
High 
FR-3.2 
Dashboard shall retrieve the 
latest public humidity feed 
value. 
Humidity card displays value and 
percent unit, or an unavailable 
state. 
High 
FR-3.3 
Dashboard shall retrieve the 
latest MAX30102 heart-rate 
feed value. 
Display reflects feed value and 
identifies the sensor source. High 
FR-3.4 
Dashboard shall retrieve the 
latest HW-827 heart-rate feed 
value. 
Display identifies the HW-827 
source separately. High 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
B.Tech Project Work  •  SRS  •  7 
FR-3.5 Dashboard shall refresh feed 
data periodically. 
Values refresh at the configured 
polling interval when online. High 
FR-3.6 Dashboard shall communicate 
unavailable or stale feed data. 
Missing or old data is not 
presented as a newly measured 
value. 
High 
FR-3.7 Dashboard shall display a 
prototype-use disclaimer. 
Page states that measurements 
are not a clinical diagnosis. High 
FR-3.8 
Dashboard shall not display 
SpO₂ as measured unless 
calculation and feed are 
implemented. 
SpO₂ remains unavailable/not 
implemented in the current 
version. 
High 
 
3.4 Module 4: Monitoring and Diagnostics 
ID Requirement Acceptance criteria Priority 
FR-4.1 
Firmware shall report Wi-Fi and 
MQTT connection states 
through serial monitor. 
Output distinguishes connected 
and disconnected states. Medium 
FR-4.2 
Firmware shall print sensor 
readings and publishing 
outcomes. 
Output includes values or an 
unavailable/error indication. Medium 
FR-4.3 
Dashboard shall distinguish 
current feed data from stale 
data. 
Status is based on feed 
timestamp and a documented 
freshness threshold. 
Medium 
 
4. External Interface Requirements 
4.1 User Interface 
The dashboard is a browser-based page for viewing current measurements. Each measurement should have a clear label, 
unit and source sensor. Connection or freshness status should be visible without requiring the viewer to inspect developer 
tools. 
• Temperature: degrees Celsius (°C). 
• Humidity: percentage (%RH). 
• Heart rate: beats per minute (BPM), with source sensor identified. 
• Unavailable values: show 'Unavailable' or 'No recent data'; do not substitute zero unless zero is a genuine 
measurement. 
• SpO₂: show 'Not implemented' or 'N/A' in the current version. 
• Page should remain usable on common desktop and mobile browser sizes. 
Evidence to insert in the final submission: screenshot of the deployed dashboard showing the feed cards, update status 
and prototype disclaimer. 
4.2 Hardware Interfaces 
Component Interface / pin mapping Purpose 
DHT11 Digital data: ESP32 GPIO 4 (as configured) Temperature and humidity acquisition. 
HW-827 Analogue output: ESP32 GPIO 34 (as 
configured) 
Pulse waveform sampling through ADC 
input. 
MAX30102 I²C SDA: GPIO 21; SCL: GPIO 22 (as 
configured) Optical sensor communication. 
ESP32 USB / UART serial connection Firmware upload and diagnostics. 
ESP32 Wi-Fi radio Cloud connectivity. 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
The actual board variant, supply voltage and sensor breakout requirements must be checked against the hardware 
documentation and physical wiring. Pin mapping above reflects the latest firmware configuration described during 
development. 
4.3 Software Interfaces 
Software / library 
Role 
Arduino IDE and ESP32 Arduino Core 
WiFi / WiFiClientSecure 
Firmware development, compilation and upload. 
Wireless networking and TLS-enabled client connection. 
PubSubClient 
DHT sensor library 
MQTT client used to publish feed values. 
DHT11 measurement acquisition. 
SparkFun MAX3010x library and heartRate helper 
Adafruit IO 
MAX30102 access and beat-detection support. 
Cloud MQTT broker and feed service. 
HTML, CSS and JavaScript 
GitHub Pages 
Dashboard structure, styling and data retrieval. 
Static hosting for dashboard files. 
No SQL database, ORM, custom REST backend or authentication service is specified for the current implementation. 
4.4 Communication Interfaces 
Communication path 
Protocol / endpoint 
ESP32 ↔ Adafruit IO 
Purpose 
MQTT over TLS, TCP port 8883 
Browser → GitHub Pages 
Authenticated publishing to cloud feeds. 
HTTPS 
Browser → Adafruit IO 
Delivery of static dashboard assets. 
HTTPS REST API for public feed data 
ESP32 ↔ sensors 
Retrieval of latest feed values. 
I²C / GPIO / ADC 
Local sensor communication and sampling. 
MQTT topic pattern: account-name/feeds/feed-key. Feed keys used by this project are temperature, humidity, max30102
heart-rate and hw-827-heart-rate. Dashboard requests must reference the corresponding account and feed keys. 
5. Non-Functional Requirements 
Attribute 
Requirement / target 
5.1 Performance 
Dashboard refreshes values at its configured 
polling interval. Firmware schedule stays 
within the cloud account's current rate limit. 
Verification method 
Observe feed timestamps and values during 
a continuous test. 
5.2 Security 
MQTT credentials are not stored in public 
source control. Broker certificate verification 
is used for TLS. 
Inspect repository and firmware 
configuration; verify TLS connection. 
5.3 Privacy 
Only public demonstration data is exposed. 
Do not publish names, addresses or other 
personal information. 
Review feed contents and dashboard 
requests. 
5.4 Reliability 
Device retries Wi-Fi and MQTT connections 
after temporary disconnection. Dashboard 
handles missing responses and network 
errors. 
Disconnect and restore Wi-Fi; simulate 
failed feed requests. 
5.5 Data integrity 
Feed labels, units and sensor mappings 
remain consistent between firmware and 
dashboard. 
Compare MQTT topics, feed keys and 
dashboard mappings. 
5.6 Usability 
Measurements have readable labels, units 
and clear unavailable/stale states. 
Review dashboard on desktop and mobile 
viewports. 
5.7 Portability 
Dashboard works in current Chrome, Edge 
and Firefox with JavaScript enabled. 
Cross-browser manual test. 
5.8 Maintainability 
Sensor acquisition, network publishing and 
Code review and repository structure 
B.Tech Project Work  •  SRS  •  8 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
dashboard retrieval remain separated into 
understandable modules/files. 
inspection. 
6. System Design and Analysis Models 
The following models describe the implemented prototype at a logical level. They can be converted into formal UML or 
DFD drawings for the final submission. 
6.1 Use Case Model 
Actor 
Use case 
Description 
Project operator 
Configure and start device 
Enters Wi-Fi and Adafruit IO settings, 
powers ESP32 and checks serial output. 
ESP32 device 
Acquire sensor readings 
Reads connected sensors and checks 
whether values are suitable for publishing. 
ESP32 device 
Publish telemetry 
Sends valid measurements to corresponding 
Adafruit IO feeds using MQTT. 
Dashboard viewer 
View latest readings 
Opens GitHub Pages dashboard and reviews 
available sensor values. 
Dashboard viewer 
Check freshness 
Uses status indicators to identify missing or 
stale readings. 
6.2 Data Flow Diagram – Level 0 (Context) 
External entity 
Data exchanged 
DHT11, MAX30102, HW-827 
Temperature, humidity, IR and analogue 
samples 
System process 
ESP32 sensing and processing 
Wi-Fi network 
Network connectivity 
ESP32 network interface 
Adafruit IO 
MQTT telemetry in; feed values available 
out 
Cloud feed service 
Dashboard viewer 
Views dashboard and receives displayed 
values 
GitHub Pages dashboard 
6.3 Data Flow Diagram – Level 1 
1. Sensor acquisition: ESP32 reads DHT11, MAX30102 and HW-827 interfaces. 
2. Validation and calculation: firmware checks sensor results and derives supported pulse-rate estimates. 
3. Cloud transmission: firmware connects to Adafruit IO and publishes valid values to configured feeds. 
4. Feed retrieval: dashboard JavaScript requests latest public feed values over HTTPS. 
5. Presentation: dashboard formats values, units, timestamps and freshness states for the viewer. 
6.4 Entity / Data Model 
Field 
The current implementation uses Adafruit IO feeds rather than a project-managed relational database. A conceptual feed
update record contains: 
Feed key 
Description 
Value 
Identifies the measurement stream, such as temperature. 
Numeric measurement published by ESP32. 
Created timestamp 
Source device 
Cloud timestamp associated with the feed update. 
Logical source of the update; include as a stored field only if 
configured. 
B.Tech Project Work  •  SRS  •  9 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
B.Tech Project Work  •  SRS  •  10 
A relational ERD with users, patients, devices and measurement tables is not applicable to the current architecture 
because no such database is implemented. It may be added if persistent patient records or user accounts become a future 
requirement. 
6.5 Sequence of Operations 
Order Interaction 
1 Sensors provide samples to ESP32. 
2 Firmware validates readings and calculates supported pulse-rate 
values. 
3 ESP32 connects to Wi-Fi and Adafruit IO using MQTT/TLS. 
4 ESP32 publishes eligible values to dedicated feed topics. 
5 Adafruit IO stores the feed update and timestamp. 
6 Dashboard JavaScript periodically requests latest public feed values 
over HTTPS. 
7 Dashboard updates measurement cards and freshness indicators. 
 
7. Academic Review and Sign-Off 
Project guide / coordinator Internal examiner Head of Department 
Name: __________________ 
 
Signature: _______________ 
 
Date: __________________ 
Name: __________________ 
 
Signature: _______________ 
 
Date: __________________ 
Name: __________________ 
 
Signature: _______________ 
 
Date: __________________ 
 
Appendix A – Verification Checklist 
Test ID Test procedure Expected result Evidence / status 
TC-01 Power on ESP32 with sensors 
connected. 
Sensors initialize; startup status 
appears in Serial Monitor. Record result / screenshot 
TC-02 Connect ESP32 to configured 
Wi-Fi. Wi-Fi status reports connected. Record result 
TC-03 Connect to Adafruit IO using 
MQTT/TLS. 
MQTT connection succeeds and 
is reported. 
Observed during development; 
retain screenshot 
TC-04 Publish DHT11 readings. Temperature and humidity 
feeds receive new values. Observed; retain feed evidence 
TC-05 Publish valid MAX30102 
estimate. 
MAX30102 feed receives a value 
when validity checks pass. Record test evidence 
TC-06 Publish valid HW-827 estimate. HW-827 feed receives a value 
when validity checks pass. Observed; retain feed evidence 
TC-07 Open dashboard and wait for 
refresh. 
Dashboard displays latest 
available values from four feeds. 
Reported working; retain 
screenshot 
TC-08 
Allow a feed timestamp to 
become old or simulate request 
failure. 
Dashboard indicates 
stale/unavailable state. To be tested 
TC-09 Disconnect and restore Wi-Fi. Firmware retries and resumes 
publishing after recovery. To be tested 
TC-10 Inspect public repository for 
secrets. 
No Wi-Fi password or Adafruit 
IO key is committed. To be verified 
 
PATIENT HEALTH MONITORING SYSTEM  |  SOFTWARE REQUIREMENTS SPECIFICATION 
Appendix B – Items to Confirm Before Final Submission 
• Enter exact institute, department, student names, roll numbers, supervisor name and submission date on the cover 
page. 
• Confirm final physical pin wiring; this draft uses GPIO 4, GPIO 34, GPIO 21 and GPIO 22 as described in the latest 
firmware discussion. 
• Confirm exact Adafruit IO account/feed keys in final code and dashboard. Do not include the private Adafruit IO key 
in the report or repository. 
• Insert screenshots of the physical prototype, Arduino Serial Monitor, Adafruit IO feeds and deployed GitHub Pages 
dashboard. 
• Record actual test outcomes and dates. Do not mark unperformed tests as passed. 
• Confirm whether the dashboard currently shows SpO₂ as a placeholder. The firmware described here does not 
implement SpO₂ calculation. 
• If authentication, patient profiles, alerts, data export, historical charts or a database have since been implemented, 
update this SRS to match the final system. 
End of document 
B.Tech Project Work  •  SRS  •  11 

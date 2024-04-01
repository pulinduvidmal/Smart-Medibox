# Smart Medibox

Smart Medibox is an innovative solution designed to ensure individuals adhere to their medication schedules efficiently. This device is not just a reminder; it actively engages in maintaining the optimal conditions for medication storage by constantly monitoring temperature and humidity levels. Alerts are generated to inform the user of any deviations from the desired conditions, ensuring medication efficacy is preserved.

## Key Features

- **Medication Reminders**: Automates medication schedules with timely alarms, ensuring doses are not missed.
- **Environmental Monitoring**: Continuously checks temperature and humidity, alerting the user to any conditions that might compromise medication quality.
- **User-friendly Interface**: Utilizes an ADAFRUIT SSD 1306 OLED Monochrome Display for clear, accessible interaction.

## Technology Stack

The project leverages the following hardware components:

- **ADAFRUIT SSD 1306 OLED Monochrome Display (128x64)** for crisp, readable output.
- **ESP32 Devkit V1** as the central processing unit, providing robust performance and connectivity.
- **DHT11 Temperature and Humidity Sensor** for accurate environmental readings. (Configurable for DHT22.)

Developed with flexibility in mind, the system supports easy adaptation to alternative components by adjusting parameters accordingly.

## Development Environment

This project was crafted using PlatformIO with the Arduino Framework, ensuring a streamlined development process from coding to deployment. For those interested in simulation before physical testing, a specific Wokwi simulation branch is available.

### Prerequisites

- Git for repository cloning.
- PlatformIO with Arduino Framework, set up and ready.
- Relevant hardware components for hands-on testing (optional for simulation).

### Algorithm
![Screenshot 2024-03-16 012407](https://github.com/pulinduvidmal/Smart-Medibox/assets/107745680/40df86ac-7b8b-4aec-b0e3-804ca2471bba)


### Getting Started

To clone the repository:

```bash
git clone https://github.com/pulinduvidmal/Smart-Medibox
```

For simulation purposes, use the Wokwi-specific branch:

```bash
https://wokwi.com/projects/390588406511806465
```

PlatformIO typically handles library dependencies automatically. Refer to `platform.ini` for manual installations if needed.

## About the Project

Originating as a semester project for the EN2853 - Embedded Systems & Applications module at UoM Smart Medibox aims to blend IoT convenience with healthcare needs, creating a smarter approach to medication management.

---

This project is a testament to the potential of embedded systems in enhancing daily life, particularly in critical areas like healthcare. Through continuous development and community feedback, Smart Medibox will evolve, pushing the boundaries of what smart devices can achieve in personal health management.

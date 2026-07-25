# 🎵 Arduino RFID Smart MP3 Player

An Arduino-based RFID Smart MP3 Player that authenticates users using RFID cards and automatically plays personalized audio tracks. The system also includes an LCD interface, manual music controls, LED status indicators, and an interactive Dinosaur Game that activates when the system is idle.

---

## 📖 Project Overview

This project was developed as a final Arduino project to demonstrate embedded systems programming, RFID authentication, audio playback, and user interaction.

The system recognizes authorized RFID cards, displays the user's name on an LCD screen, and plays an assigned MP3 track using a DFPlayer Mini module. Unauthorized RFID cards trigger an access denied message and an error sound.

To improve user experience, the project also includes an idle-mode Dinosaur Game inspired by the Chrome offline game.

---

## ✨ Features

- RFID Card Authentication
- Personalized User Recognition
- Automatic MP3 Playback
- LCD Display Interface
- Manual Music Controls
- Volume Control
- Status LEDs
- Idle Mode Dinosaur Game
- Access Control System
- Error Handling
- Startup Animation

---

## 🛠 Hardware Components

- Arduino Uno
- MFRC522 RFID Module
- RFID Cards / Tags
- DFPlayer Mini MP3 Module
- I2C LCD Display (16x2)
- Passive Speaker
- Push Buttons
- LEDs
- Breadboard
- Jumper Wires
- MicroSD Card

---

## 💻 Software

- Arduino IDE
- Arduino C++

---

## 📚 Libraries Used

- SoftwareSerial
- DFRobotDFPlayerMini
- LiquidCrystal_I2C
- SPI
- MFRC522

---

## 🔧 Hardware Connections

| Component | Purpose |
|-----------|---------|
| MFRC522 | RFID Authentication |
| DFPlayer Mini | Audio Playback |
| LCD 16x2 | User Interface |
| Push Buttons | Track and Volume Controls |
| LEDs | System Status Indicators |
| Speaker | Audio Output |

---

## 🚀 System Workflow

1. Power on the system.
2. Startup animation is displayed.
3. LCD prompts the user to scan an RFID card.
4. The system verifies the RFID UID.
5. If authorized:
   - Displays the user's name
   - Plays the assigned MP3 file
   - Lights the green LED
6. If unauthorized:
   - Displays "Access Denied"
   - Plays an error sound
   - Lights the red LED
7. During inactivity, the system automatically launches a Dinosaur Game.

---

## 🧠 Skills Demonstrated

- Embedded Systems Programming
- Arduino Programming
- RFID Technology
- Access Control Systems
- LCD Programming
- Audio Playback Integration
- Hardware Troubleshooting
- C/C++ Programming
- State Machine Logic
- User Interface Design

---

## 📂 Repository Contents

```text
RFID_Smart_MP3_Player.ino
README.md
```

---


## 📚 Learning Outcomes

Through this project, I gained practical experience in:

- RFID Authentication
- Microcontroller Programming
- Hardware Integration
- LCD Interface Development
- Audio Module Integration
- Event-driven Programming
- Embedded Software Design
- Debugging Electronic Systems

---

## 👨‍💻 Author

**Angelico Joshua**

Bachelor of Science in Information Technology (BSIT)


---

## 📄 License

This project is shared for educational and portfolio purposes.

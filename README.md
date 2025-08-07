# Glasses-Mounted Vision Assistance System

This is an Arduino-based assistive technology project that uses a distance sensor and DFPlayer Mini to play audio alerts based on obstacle proximity. Designed for the visually impaired.

## Features
- Ultrasonic distance detection
- Audio feedback via speaker
- Portable and glasses-mounted

## Components
- Arduino Uno
- DFPlayer Mini
- Ultrasonic Distance Sensor (HC-SR04)
- Speaker
- 9V Battery + Clip

## How It Works
The Arduino measures distance using the HC-SR04 sensor and plays one of three audio files (located in `/audio`) through the DFPlayer Mini depending on the proximity of detected obstacles.

## File Structure
- `audio/` - contains MP3 audio files
- `arduino.ino` - main Arduino sketch
- `README.md` - this file

## Future Plans
- Add object recognition using Raspberry Pi
- Custom PCB for compact design

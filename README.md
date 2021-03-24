# CronuSwim [![Build Status](https://travis-ci.com/antnolang/CronuSwim.svg?branch=master)](https://travis-ci.org/antnolang/CronuSwim) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=antnolang_CronuSwim&metric=alert_status)](https://sonarcloud.io/dashboard?id=antnolang_CronuSwim)

CronuSwim is an embedded software designed to help swimming enthusiasts to measure their times in the pool very accurately without the help of a coach.

It works with [Arduino](https://www.arduino.cc/) and its main advantage is that the timer starts and stops automatically thanks to an implementation of [Naive Bayes algorithm](https://en.wikipedia.org/wiki/Naive_Bayes_classifier). This software, through various motion sensors located on the arduino board, is capable of detecting when the swimmer starts and finishes.

In this way, the swimmer saves the time it would take to start the stopwatch just before leaving and stop it just after finishing.

Therefore, we achieve more precise measurements and that the swimmer can fully focus on his swim instead of being aware of starting and stopping the timer as quickly as possible.

## Requirements

### Hardware
- Laptop with software dependencies defined below
- Arduino Nano 33 BLE Sense
- USB - Micro USB cable
- Some old belt that you do not want anymore

### Software
- Linux system (tested with Manjaro XFCE, kernel 5.4)
- bash
- gcc
- make
- [arduino-cli](https://github.com/arduino/arduino-cli)
- [catch2](https://github.com/catchorg/Catch2)
- [emlearn](https://github.com/emlearn/emlearn)

## How to use it
***This system is a prototype, so the usability may be pretty poor for now.***

### Hardware preparation
1. Attach the arduino board to the belt. In my case, I sewed the board to the belt. ![[arduino_sewing_image]](/doc/arduino_sewing.jpg)
2. Connect your arduino board to your laptop using the USB-Micro USB cable. ![[arduino_sewed_image]](/doc/arduino_sewed.jpg)
3. Make sure your laptop has enough battery.

### Install
1. Open a terminal.
2. Clone the project to some directory in your system folder.
3. Change directory to the project.
4. Make the project with the following command: `make`
![[make_output_image]](/doc/make_output.png)

### Environment preparation
1. [Find](https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html) the port used for arduino. In my case the port is `/dev/ttyACM0`. If your arduino board uses a different port than `/dev/ttyACM0`, change the commands below accordingly.
2. Open two different terminals. The first one will be used to send signals to the arduino board. The second one will be used to see the output.
3. Put on your belt.
4. Push the reset button of the arduino board.
5. In the second terminal execute the following commands: 
```
$ stty 9600 -F /dev/ttyACM0 raw -echo
$ cat /dev/ttyACM0
```
6. Now we can send the signal to the arduino board using the following command in the first terminal: 
```
$ echo "X">/dev/ttyACM0
```
7. Every time we send the signal to the board, its state changes. Check out the wiki for more information about the different [states](https://github.com/antnolang/CronuSwim/wiki/Internal-states).

### Run
1. Make sure the state of the software is *Waiting*.
![[waiting_image]](/doc/waiting.png)
2. Make sure you have a straight clear path in front of you and there is a wall at the end.
3. Holding the laptop with your arms, get into the swimming starting position.
4. Send the signal to the arduino at the same time you start walking. 
![[reading_image]](/doc/reading.png)
5. Walk until the end of the path.
6. With one arm, hold the laptop and with the other make a swimming finish on the wall, at the same time you send again a signal to the arduino.
7. In the second terminal you should see the time estimation. 
![[use_case_image]](/doc/use_case.png)

## More
- This file does not explain how to train the system with your own moves. This will be documented in the future.
- Check out the [wiki](https://github.com/antnolang/CronuSwim/wiki) for more information about the project.

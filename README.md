# SCDTR

Project to a Distributed Real Time Control Systems course

This repository contains the course project done in MatLab/C++ and the its report. It consists of an automated lighting control system, with a distributed approach to the global optimisation problem.

It was tested using 2 Arduino boards to mimick a workstation, each with equipped with a LED and LDR as actuator and sensor respectively. The system was enclosed in a shoe box to reflect and accentuate inter-node coupling. A video test can be seen [here](https://www.youtube.com/watch?v=lx_5Uw84AJI&feature=youtu.be)

A TCP/IP server was also developed in order to remotely control and monitor the system, as well as calculate performance heuristics. The server itself was tested on a Raspberry Pi 3B. In order for it to extract information from the system an I2C connection to the Arduinos is required. A video test can be seen [here](https://www.youtube.com/watch?v=YitZQARzt44&feature=youtu.be)

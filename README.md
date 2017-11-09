# AGL demo for 96Boards

This repository consists of [AGL](https://www.automotivelinux.org/) demos for 96Boards.

# Table of Contents
- [1) Hardware](#1-hardware)
   - [1.1) Hardware requirements](#11-hardware-requirements)
- [2) Software](#2-software) 
   - [2.1) Operating System](#21-operating-system)
- [3) HVAC Demo](#3-hvac-demo)

# 1) Hardware

## 1.1) Hardware requirements

- [Dragonboard410c](https://www.96boards.org/product/dragonboard410c/)
- [96Boards Compliant Power Supply](http://www.96boards.org/product/power/)
- [Linksprite 96Boards Touch Screen](https://www.arrow.com/en/products/96boards-display-7/linksprite-technologies-inc)
- [Sensors Mezzanine](http://www.96boards.org/product/sensors-mezzanine/)
- [DC motor with Propellers](https://www.amazon.com/Piece-Propeller-Motor-Brushed-Small/dp/B00U53SGRQ)
- [5mm LED's](https://www.sparkfun.com/products/9590)

# 2) HVAC Demo

HVAC (Heat Ventilation and Air Control) demo focussed on demostrating the HVAC
GUI present in AGL demo platform. This involves controlling the fan speed and
temperature. 

For demonstration purposes, default HVAC GUI is modified as below:

1. Controlling Fan Speed
2. Controlling Left temperature - Controlling Light intensity
3. Controlling Right temperature - Controlling Light intensity

## 2.1) Hardware Setup

- Make sure the Dragonboard410c is powered off
- Connect DC motor and LEDs to Sensors Mezzanine as per above schematic
- Power on your 96Boards CE with compatible power supply

>Note: It is assumed that Sensors Mezzanine is placed on top of Dragonboard410c

## 2.2) Software Setup

Proceed to build AGL using the instructions specified in
[blog](https://www.96boards.org/blog/agl-on-dragonboard410c-part1/). Execute
all the commands before `bitbake` and follow the below instructions.

```shell
$ cd ~/Documents
$ git clone https://96boards-projects/agl-demo.git
$ cd agl-demo
```
Copy the custom HVAC recipie to AGL source

```shell
$ cp hvac_git.bb $(AGL_TOP)/meta-agl-demo/recipes-demo-hmi/hvac/hvac_git.bb
```
Execute `bitbake` command by moving to the AGL source directory

```shell
$ cd $(AGL_TOP)/build
$ bitbake agl-demo-platform
```
> Note: Replace AGL_TOP with the top level directory of cloned AGL source

Once build has been finished, follow the above mentioned blog to flash
AGL onto Dragonboard410c.


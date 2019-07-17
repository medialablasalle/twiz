~ Twiz ~
========

![twiz_top](https://raw.github.com/medialablasalle/twiz/master/docs/twiz_top.png) ![twiz_bottom](https://raw.github.com/medialablasalle/twiz/master/docs/twiz_bottom.png)


Abstract
--------

Twiz is a wireless motion sensor sending its data using Bluetooth Low energy.

Its Inertial Measurement Unit has 9 degrees of freedom and the fusion of its
sensors (3D accelerometer + 3D gyroscope + 3D magnetometer) is made on board.

The fusion result is sent as Euler angles, telling how it id tilted compared
to the gravity vector and how it is oriented compared to the north pole.

This repository contains the firmware but the hardware is open source too, it
can be found on [upverter](https://upverter.com/medialablasalle/cef9ff1aa5e13e42/twiz).


Details
-------

The data sent is as follow:

    * 3D accel values
    * 3D Euler angles

These values are advertisement in the manufacturer data packet as 12 bytes
representing 6 signed integers, each of them being normalized on 16 bits:

    * Order of the data: 0011 2233 4455  AABB CCDD EEFF
                         x    y    z     yaw pitch roll
                         [accel values]  [euler angles]

    * accel values range: [-2.0 g ; +2.0 g[

    * euler angles range: [-180.0 ; +180.0[

Note: the range of a 16 signed bit value is `[-2**15 ; 2**15 - 1]`


Examples:

1 ) If we look at the bytes #4 and #5, we get z = 0x4455.
To get the acceleration value, the calculation is:

    z_accel = 0x4455 * 4.0 / 2**16
            = 17493  * 4.0 / 2**16
            = 1.0677 g

Note: When the sensor is horizontal with the battery below, z should be -1.


2 ) To get the yaw angle, corresponding to 0xAABB in our example:

    yaw_angle = 0xAABB * 360.0 / 2**16
              = -21829 * 360.0 / 2**16
              = -119.9 degrees

Note: the accuracy of magnetometers is not that great yet, the fusion with the
gyroscope improves it but decimals are not hugely relevant.


BLE services
------------

* IMU data stream: accel + attitude (same as in advertizing)
* GPIO:
    * LED: write
    * button: read
* UART:
    * TX: write
    * RX: read


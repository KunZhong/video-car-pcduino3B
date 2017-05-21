## uvc-streamer

Simple V4L2 camera capture and streamer

video-car basedOn pcDuino

This is modification of original code part from [mjpg-streamer](https://sourceforge.net/projects/mjpg-streamer/)

### Build
````
$ sudo apt-get install libjpeg8-dev
$ make
````

### Cross compile raspberry pi
````
$ wget http://www.ijg.org/files/jpegsrc.v8.tar.gz
$ tar -xzvf jpegsrc.v8.tar.gz
$ cd jpeg-8
$ export PATH=$PATH:$HOME/cross/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin
$ ./configure --host=arm-linux CC=arm-linux-gnueabihf-gcc
$ make
$ cd ..
$ make CC=arm-linux-gnueabihf-gcc
````

### Usage
```
$ ./uvc_stream -d /dev/video0
$ ./uvc_stream -h
```

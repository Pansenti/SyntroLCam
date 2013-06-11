# SyntroLCam

SyntroLCam is a Linux only Syntro application for connecting USB webcams
to a Syntro cloud.


### Dependencies

1. Qt4 or Qt5 development libraries and headers
2. SyntroCore libraries and headers 


### Fetch

        git clone git://github.com/Syntro/SyntroLCam.git


### Build 

        qmake 
        make 
        sudo make install

### Run

#### GUI mode

        SyntroLCam

If you are seeing errors regarding extra bytes before JPEG markers,
those are coming from Qt's built-in libjpeg. There errors are in the
camera generated MJPEG data. The errors are harmless and can be discarded
by running this way

        SyntroLCam &> /dev/null



#### Console mode

        SyntroLCam -c


Enter h to get some help.


#### Daemon mode

        SyntroLCam -c -d

Only works in conjunction with console mode.




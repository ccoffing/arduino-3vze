# FreeBSD:
#   portmaster arduino arduino-mk
#   export ARDUINO_DIR=/usr/local/arduino
#   export ARDMK_DIR=/usr/local/arduino-mk
#   export AVR_TOOLS_DIR=/usr/local/
#   export MONITOR_PORT=/dev/cuaU1
#   gmake
#   gmake upload
#   gmake reset
#
# Debian:
#   apt-get install arduino arduino-mk screen
#   export ARDUINO_DIR=/usr/share/arduino
#   export ARDMK_DIR=/usr/share/arduino
#   export AVR_TOOLS_DIR=/usr/
#   export MONITOR_PORT=/dev/ttyACM0
#   make
#   make upload
#   make reset

BOARD_TAG = uno
ARDUINO_LIBS = SPI SD
include $(ARDMK_DIR)/Arduino.mk


#Adafruit 2.2" SPI TFT Test!
#Benchmark                Time (microseconds)
#Screen fill              2102312
#Text                     268404
#Lines                    2403728
#Horiz/Vert Lines         179356
#Rectangles (outline)     119412
#Rectangles (filled)      4367180
#Circles (filled)         923812
#Circles (outline)        1048032
#Triangles (outline)      526304
#Triangles (filled)       1844284
#Rounded rects (outline)  394372
#Rounded rects (filled)   4864400
#Benchmark                Time (microseconds)
#Screen fill              2102008
#Text                     268412
#Lines                    2405024
#Horiz/Vert Lines         179356
#Rectangles (outline)     119416
#Rectangles (filled)      4367184
#Circles (filled)         924960
#Circles (outline)        1048176
#Triangles (outline)      526296
#Triangles (filled)       1844304
#Rounded rects (outline)  394380
#Rounded rects (filled)   4864420
#Benchmark                Time (microseconds)


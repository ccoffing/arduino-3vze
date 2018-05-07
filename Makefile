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

EXTRA_CXXFLAGS=-std=gnu++11
BOARD_TAG = uno
ARDUINO_LIBS = SPI SD
include $(ARDMK_DIR)/Arduino.mk

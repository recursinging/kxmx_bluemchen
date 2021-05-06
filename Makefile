# Project Name
TARGET ?= hardware_test

DEBUG = 1
OPT = -O0

# Sources
CPP_SOURCES = src/kxmx_bluemchen.cpp examples/${TARGET}.cpp

USE_FATFS = 1

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
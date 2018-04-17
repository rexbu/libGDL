DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/
CC=/usr/bin/gcc
PP=/usr/bin/g++
SDKROOT=$(DEVROOT)/SDKs/MacOSX10.10.sdk
LDFLAGS=-L$(SDKROOT)/usr/lib/system --sysroot=$(SDKROOT)

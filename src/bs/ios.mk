#DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain
CC=$(DEVROOT)/usr/bin/clang -arch $(instruction)
PP=$(DEVROOT)/usr/bin/clang++ -arch $(instruction)
#SDKROOT=$(DEVROOT)/SDKs/iPhoneOS6.1.sdk
SDKROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
LDFLAGS=-L$(SDKROOT)/usr/lib/system --sysroot=$(SDKROOT)

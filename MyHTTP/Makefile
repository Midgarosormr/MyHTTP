CXX = g++
CFLAGS = -std=c++11  

TARGET = Myserver

OBJS = main.cpp ./http/*.cpp ./webserver/*.cpp ./log/*.cpp
 

all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ./bin/$(TARGET) -pthread -Wswitch -Wno-deprecated-declarations -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -Wall -fno-strict-aliasing -g2 -gdwarf-2 -O0 -fthreadsafe-statics -Wswitch -Wno-deprecated-declarations -Wempty-body -Wconversion -Wreturn-type -Wparentheses -Wno-format -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -frtti -fno-omit-frame-pointer -std=c11 

clean:
	rm -rf ./bin/$(TARGET)


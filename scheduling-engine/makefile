.PHONY : clean

CPPFLAGS= -std=c++17 -fPIC -Wall -Wextra -O3
LDFLAGS= -std=c++17 -shared

SOURCES = scheduler.cpp external.cpp profile.cpp history.cpp 
OBJECTS=$(SOURCES:.cpp=.o)

TARGET=libscheduler.so

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

$(TARGET) : $(OBJECTS)
	g++ $(CPPFLAGS) $(OBJECTS) -o $@ $(LDFLAGS) -lboost_filesystem -lboost_serialization -ltbb -lcryptopp


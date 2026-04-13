CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET   = smart_farming_app

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET) farms_db.txt crops_db.txt sensors_db.txt

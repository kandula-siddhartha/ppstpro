CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET   = smart_farming

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET) *.o farms_db.txt crops_db.txt sensors_db.txt

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

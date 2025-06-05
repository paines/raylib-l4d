CXX = g++
CXXFLAGS = $(shell pkg-config --cflags raylib) -std=c++11 -Wall -Wextra -O2
LDFLAGS = $(shell pkg-config --libs raylib)
SRC = main.cpp
TARGET = raylib-l4d

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: clean $(TARGET)
	./$(TARGET)

.PHONY: all clean run
CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/opt/sfml@2/include
LDFLAGS = -L/opt/homebrew/opt/sfml@2/lib
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = minecraft2d
SRCS = $(wildcard *.cpp)

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
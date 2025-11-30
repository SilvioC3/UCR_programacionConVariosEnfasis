TARGET = main

CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17 -O2

SRC = main.cc JuegoCon.cc Jugador.cc
OBJ = $(SRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cc JuegoCon.hh Jugador.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
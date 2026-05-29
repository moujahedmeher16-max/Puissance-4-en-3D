# ─── Makefile — Puissance 4 3D ───────────────────────────────────────────────

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = puissance4

SRCS = main.cpp \
       Game.cpp \
       Board.cpp \
       BitBoard.cpp \
       Cell.cpp \
       Move.cpp \
       Player.cpp \
       HumanPlayer.cpp \
       AIPlayer.cpp \
       Strategy.cpp \
       RandomStrategy.cpp \
       AlphaBetaStrategy.cpp \
       Evaluator.cpp

OBJS = $(SRCS:.cpp=.o)

# ─── Règles ───────────────────────────────────────────────────────────────────

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean

CXX = g++
CXXFLAGS = -Wall -std=c++17 #-fsanitize=address

SRCS = main.cc image.cpp image_io.cpp gaussian_blur.cc nl_mean.cc selective_gaussian.cc
OBJS = $(SRCS:.cc=.o)
OBJS := $(OBJS:.cpp=.o)
TARGET = tp2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean

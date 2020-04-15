

ROOT := $(shell pwd)

CXX ?= g++

TARGET := ifmoniter

SRC := main.cpp netif_info.cpp
OBJ := main.o netif_info.o

INC := -I$(ROOT)
CXXFLAGS := $(INC) -std=c++11 



all: $(TARGET)


$(OBJ):%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $^

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@  

clean:
	rm -rf $(OBJ)
	rm -rf $(TARGET)

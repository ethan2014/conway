CC	= g++
FLAGS	= -Wall -Werror -pedantic -lpthread -std=c++14
SOURCE	= main.cpp conway.cpp
TARGET	= main

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(FLAGS) -O3 $(SOURCE) -o $(TARGET)

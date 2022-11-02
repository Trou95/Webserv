NAME =
SRC = utils/FileReader/*.cpp *.cpp
CC = c++

all:
	$(CC) $(SRC) -o Server
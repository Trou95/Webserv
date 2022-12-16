NAME = Server
SRC = utils/FileReader/*.cpp utils/Logger/*.cpp utils/*.cpp Parser/cgiparser/*.cpp *.cpp
CC = c++

all:
	$(CC) $(SRC) -o $(NAME)
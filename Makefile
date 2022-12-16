NAME = Server
SRC = utils/FileReader/*.cpp utils/Logger/*.cpp utils/*.cpp Parser/cgiparser/*.cpp *.cpp
CC = c++ -g

all:
	$(CC) $(SRC) -o $(NAME)
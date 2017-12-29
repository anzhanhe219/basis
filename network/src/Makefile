CPP = g++
LINK = gcc
AR = ar rcs

CPPFLAG = -Wall -ggdb -march=x86-64 -DDEBUG -I../include -I../depend/include
LINKFLAG = -lm -lstdc++ -lpthread

CPPSRC = $(wildcard ./src/*.cpp)

NAME = server

all : $(NAME)
$(NAME) :  $(CPPSRC:.cpp=.o)
	$(LINK) -o $(NAME) $(CPPSRC:.cpp=.o) ../lib/libbasis.a   $(LINKFLAG)

%.o : %.cpp
	$(CPP) -c $(CPPFLAG) $< -o $@

clean:
	@-rm -rf ./src/*.o  $(NAME)

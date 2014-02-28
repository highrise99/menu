menu: main.o menu.o
	g++ -o menu main.o menu.o
main.o: main.cc
	g++ -c main.cc
menu.o: menu.cc menu.h
	g++ -c menu.cc

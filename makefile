menu: main.o new_menu.o
	g++ -o -std=c++0x menu main.o new_menu.o
main.o: main.cc
	g++ -c -std=c++0x main.cc
new_menu.o: new_menu.cc new_menu.h
	g++ -c -std=c++0x new_menu.cc

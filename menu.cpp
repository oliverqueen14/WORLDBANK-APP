#include "sources/data.cpp"

//g++ -o menus.exe menu.cpp sources/*.* -I gnuplot-iostream -lboost_iostreams -lcurl

int main() {
	Menu menu;
	menu.mainMenu();
	return 0;
}
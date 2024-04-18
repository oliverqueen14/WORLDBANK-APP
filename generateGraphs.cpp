#include "sources/data.cpp"

//g++ -o generateGraphs.exe generateGraphs.cpp sources/*.* -I gnuplot-iostream -lboost_iostreams -lcurl

int main(int argc, char* argv[]) {
	// Check if the correct number of arguments is provided
	if (argc != 3) {
		std::cout << "Usage: ./program.exe argument date" << std::endl;
		return 1;
	}

	// Get the argument from the command line
	std::string argument = argv[1];
	std::string date = argv[2];
	int dateInt = std::stoi(date);

	// Rest of your code goes here
	Menu menu;
	menu.findSimilarCountriesForGenerateGraphsForAllCountries(argument, dateInt);
	return 0;
}
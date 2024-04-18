#include "sources/data.cpp"
#include <cstdlib>
#include <string>
#include <thread>
#include <iostream>
#include <vector>

//g++ -o orderGeneration.exe orderGeneration.cpp sources/*.* -I gnuplot-iostream -lboost_iostreams -lcurl

using namespace std;

CountriesIndicatorsManager countriesIndicatorsManager;

void call(int i, int year) {
	string yearChar = to_string(year);
	string command = "./generateGraphs.exe " + countriesIndicatorsManager.countries.id[i] + " " + yearChar;
	cout << command << endl;
	cout << "\n";
	const char * c = command.c_str();
	system(c);
};

int main() {
	Tools tools;
	int year;
	std::vector<std::thread> threads;
	cout << "Enter the year for which you want to generate the graphs : ";
	cin >> year;
	string yearChar = to_string(year);
	int pas = countriesIndicatorsManager.countries.size/10;
	int rest = countriesIndicatorsManager.countries.size%10;
	for(int j = 0; j < pas; j++) {
		for(int i = j*pas; i < (j+1)*pas; i++) {
			threads.push_back(std::thread(call, i, year));
		}
		for(auto& thread : threads) {
			thread.join();
		}
		threads.clear();
	}
	for(int i = pas*10; i < pas*10+rest; i++) {
		threads.push_back(std::thread(call, i, year));
	}
	for(auto& thread : threads) {
		thread.join();
	}
	threads.clear();
	return 0;
}
#include <iostream>
#include <curl/curl.h>
#include <string>
#include "sources/data.cpp"
#include "sources/graphs.cpp"
#include <cstdlib>
#include "headers/structs.h"

//g++ -o GraphCreator.exe GraphCreator.cpp sources/*.cpp -I sources/gnuplot-iostream -lboost_iostreams -lcurl

using namespace std;


class 

int main() {
	DatasetManager datasetManager("macro");
	string countryName;
	cout << "Enter the country name : ";
	getline(cin, countryName);
	countryName = datasetManager.tools.findCountry(countryName);
	string countryId = datasetManager.tools.findCountryId(countryName);
	string indicatorName;
	cout << "Enter the indicator name : ";
	getline(cin, indicatorName);
	string indicatorId = datasetManager.tools.findIndicatorId(indicatorName);
	indicatorName = datasetManager.tools.findIndicatorName(indicatorId);
	cout << "Country name : " << countryName << " Indicator name : " << indicatorName << endl;
	int index = Tools::findCountryIndicatorIndexInDataset(datasetManager.dataManager, countryId, indicatorId, datasetManager.dataManagerSize);
	cout << "Index : " << index << endl;
	Data *data;
	data = Tools::getData(datasetManager.dataManager[index], 1980, 2024);
	cout << "Data size : " << (*data).size << endl;
	string title = countryName + " " + indicatorName;
	string nameFile = countryName + "_" + indicatorName + ".png";
	Graphs graphs(nameFile);
	graphs.plot(title, *data);
	graphs.send();
    return 0;
}
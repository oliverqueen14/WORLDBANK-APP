#include <iostream>
#include <curl/curl.h>
#include <string>
#include "sources/data.cpp"
#include <cstdlib>

using namespace std;

// Fonction de rappel utilisée par libcurl pour traiter les données reçues
size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

int main() {
	// DataManager dataManager3("FRA", "NY.GDP.MKTP.CD");
	// dataManager3.printData();
	// dataManager.printData();
	// DataManager dataManager2("MAF", "SP.POP.TOTL");
	// // dataManager2.printData();
	// Tools tools;
	// DataManager dataManager3("ZWE", "SP.POP.TOTL");
	// dataManager3 = DataManager("ZWE", "NY.GDP.MKTP.CD");
	// CountriesIndicatorsManager countriesIndicatorsManager;

	// GeneralIndicators generalIndicators;
	// BookGeneralIndicators macro = generalIndicators.getGeneralIndicators("macro");
	// int dataManagerSize = 210*macro.size;
	// DataManager dataManager[dataManagerSize];
	// Tools tools;
	// for(int i = 0; i < tools.countries.size; i++) {
	// 	for(int j = 0; j < macro.size; j++) {
	// 		dataManager[i*macro.size+j].updateDataManager(tools.countries.id[i], macro.generalIndicators[j][1]);
	// 	}
	// }
	// cout << "FRA Position following tools : " << Tools::findCountryIndicatorIndexInDataset(dataManager, tools.findCountryId("france"), "NY.GDP.MKTP.CD", dataManagerSize) << endl;
	// // dataManager[373].printData();
	// dataManager[Tools::findCountryIndicatorIndexInDataset(dataManager, tools.findCountryId("france"), "NY.GDP.MKTP.CD", dataManagerSize)].printData();

	// cout << dataManager[Tools::findCountryIndicatorIndexInDataset(dataManager, tools.findCountryId("france"), "NY.GDP.MKTP.CD", dataManagerSize)].countryId << endl;
	// cout << dataManager[Tools::findCountryIndicatorIndexInDataset(dataManager, tools.findCountryId("france"), "NY.GDP.MKTP.CD", dataManagerSize)].indicatorId << endl;
	// cout << GeneralTools::stringInStringEscapeCase("United States", "united s") << endl;
	// int arr[] = {6,3,1,9,0};
	// GeneralTools::printArray(GeneralTools::sortedArray(arr,5),5);
	//DatasetManager datasetManager("macro");
	//cout << "address of datasetManager : " << &datasetManager << endl;
	//CountriesComparison::findSimilarCountries(datasetManager, "FRA", "NY.GDP.MKTP.CD", 2000, 20);
	//cout << datasetManager.numberOfSimilarCountries << endl;
	//CountriesComparison::findSimilarCountries(datasetManager, "FRA", "NY.GDP.MKTP.CD", 2000, 40);
	//cout << datasetManager.numberOfSimilarCountries << endl;
	//CountriesComparison::findSimilarCountries(datasetManager, "FRA", "NY.GDP.MKTP.CD", 2000, 50);
	//cout << datasetManager.numberOfSimilarCountries << endl;
	DatasetManager datasetManager("macro");
	datasetManager.updateIndicator("gdp");
	datasetManager.updateIndicator("social");
	datasetManager.updateIndicator("expenses");
	datasetManager.updateIndicator("unemployment");
	int degree = 10;
	while(datasetManager.numberOfSimilarCountries<20)
	{
		CountriesComparison::findSimilarCountries(datasetManager, "FRA", "NY.GDP.MKTP.CD", 1980, degree);
		degree += 10;
		cout << "For degree : " << degree << " we have " << datasetManager.numberOfSimilarCountries << " similar countries" << endl;
	}
	string similarCountries[datasetManager.numberOfSimilarCountries];
	for(int i = 0; i < datasetManager.numberOfSimilarCountries; i++)
	{
		similarCountries[i] = datasetManager.similarCountries[i];
	}
	GeneralTools::printArray(similarCountries, datasetManager.numberOfSimilarCountries);
    return 0;
}

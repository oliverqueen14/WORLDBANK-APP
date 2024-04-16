#include <iostream>
#include <curl/curl.h>
#include <string>
#include "sources/data.cpp"

using namespace std;

// Fonction de rappel utilisée par libcurl pour traiter les données reçues
size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

int main() {
	// DataManager dataManager("FRA", "NY.GDP.MKTP.CD");
	// dataManager.printData();
	// DataManager dataManager2("MAF", "SP.POP.TOTL");
	// // dataManager2.printData();
	Tools tools;
	// DataManager dataManager3("ZWE", "SP.POP.TOTL");
	// dataManager3 = DataManager("ZWE", "NY.GDP.MKTP.CD");
    return 0;
}

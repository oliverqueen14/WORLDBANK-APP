#include <curl/curl.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <unistd.h> 
#include "../headers/structs.h"
#include "graphs.cpp"

using namespace std;
namespace fs = std::filesystem;



class GeneralTools {
	public:
		static int stringInStringEscapeCase (string longString, string shortString)
		{
			transform(longString.begin(), longString.end(), longString.begin(), ::tolower);
			transform(shortString.begin(), shortString.end(), shortString.begin(), ::tolower);
			if(longString.find(shortString) != string::npos)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		};
		static int wordsInStringEscapeCase(string longString, string words[], int sizeWords)
		{
			transform(longString.begin(), longString.end(), longString.begin(), ::tolower);
			bool allWordsFound = true;
			for(int i = 0; i < sizeWords; i++)
			{
				transform(words[i].begin(), words[i].end(), words[i].begin(), ::tolower);
				if(longString.find(words[i]) == string::npos)
				{
					allWordsFound = false;
					break; // Exit the loop early if any word is not found
				}
			}
			return allWordsFound ? 1 : 0;
		};
		static int * sortedArray(int array[], int size)
		{
			int * sortedArray = new int[size];
			for(int i = 0; i < size; i++)
			{
				sortedArray[i] = array[i];
			}
			for(int i = 0; i < size; i++)
			{
				for(int j = i+1; j < size; j++)
				{
					if(sortedArray[j] < sortedArray[i])
					{
						int temp = sortedArray[i];
						sortedArray[i] = sortedArray[j];
						sortedArray[j] = temp;
					}
				}
			}
			return sortedArray;
		};
		static Data sortedData(Data data)
		{
			Data sortedData;
			sortedData.date = new int[data.size];
			sortedData.value = new double[data.size];
			sortedData.size = data.size;
			int * sortedDate = sortedArray(data.date, data.size);
			for(int i = 0; i < data.size; i++)
			{
				for(int j = 0; j < data.size; j++)
				{
					if(data.date[j] == sortedDate[i])
					{
						sortedData.date[i] = data.date[j];
						sortedData.value[i] = data.value[j];
					}
				}
			}
			return sortedData;
		};
		static void printArray(int array[], int size)
		{
			for(int i = 0; i < size; i++)
			{
				cout << array[i] << " ";
			}
			cout << endl;
		};
		static void printArray(string array[], int size)
		{
			for(int i = 0; i < size; i++)
			{
				cout << array[i];
				if (i < size-1)
				{
					cout << ", ";
				}
			}
			cout << endl;
		};
		static int folderExists(string folder)
		{
			if(fs::exists(folder) && fs::is_directory(folder))
			{
				return 1;
			}
			else
			{
				fs::create_directory(folder);
				return 1;
			}
		};
};


class DataManager {
	public:
		Data data;
		string countryId;
		string indicatorId;
		string response;
		string url;
		int dataReady = 0;
		DataManager(string countryId, string indicatorId)
		{
			this->countryId = countryId;
			this->indicatorId = indicatorId;
			data.date = new int[200];
			data.value = new double[200];
			readCSV();
			
		};
		DataManager()
		{
			data.date = new int[200];
			data.value = new double[200];
		};
		void updateDataManager(string countryId, string indicatorId)
		{
			delete[] data.date;
			delete[] data.value;
			data.date = new int[200];
			data.value = new double[200];
			this->countryId = countryId;
			this->indicatorId = indicatorId;
			readCSV();
		};
		string setUrl()
		{
			url = "http://api.worldbank.org/v2/country/" + countryId + "/indicator/" + indicatorId + ".?format=json&per_page=10000";
			return url;
		};
		static size_t writeCallback(char* ptr, size_t size, size_t nmemb, string* data) {
			data->append(ptr, size * nmemb);
			return size * nmemb;
		};
		int readCSV()
		{
			if(!dataExists())
			{
				getData();
				saveData();
				convertData();
			}
			ifstream file;
			file.open("data/" + countryId + "/" + indicatorId + ".csv");
			if(file.is_open())
			{
				string line;
				int i = 0;
				while(getline(file, line))
				{
					stringstream ss(line);
					string date;
					string value;
					getline(ss, date, '|');
					getline(ss, value, '|');
					data.date[i] = stoi(date);
					data.value[i] = stod(value);
					i++;
				}
				data.size = i;
				file.close();
				sortData();
				dataReady = 1;
				return 1;
			}
			else
			{
				return 0;
			}
		};
		void printData()
		{
			if(dataReady)
			{
				//cout << "Country : " << countryId << " : Indicator : " << indicatorId << endl;
				for(int i = 0; i < data.size; i++)
				{
					cout << data.date[i] << " : " << data.value[i] << endl;
				}
			}
			else
			{
				readCSV();
				printData();
			}
		};
		void printData(Data data)
		{
			for(int i = 0; i < data.size; i++)
			{
				cout << data.date[i] << " : " << data.value[i] << endl;
			}
		};
		void printDates()
		{
			if(dataReady)
			{
				GeneralTools::printArray(data.date, data.size);
			}
			else
			{
				readCSV();
				printDates();
			}
		};
		Data getData(int start, int end)
		{
			Data newData;
			newData.size = 0;
			for(int i = 0; i < data.size; i++)
			{
				if(data.date[i] >= start && data.date[i] <= end)
				{
					newData.size++;
				}
			}
			//cout << "newData.size : " << newData.size << endl;
			newData.date = new int[newData.size];
			newData.value = new double[newData.size];
			int j = 0;
			for(int i = 0; i < data.size; i++)
			{
				if(data.date[i] >= start && data.date[i] <= end)
				{
					newData.date[j] = data.date[i];
					newData.value[j] = data.value[i];
					j++;
				}
			}
			return newData;
		};
		int getData()
		{
			CURL* curl;
			CURLcode res;
			response = "";
			curl_global_init(CURL_GLOBAL_DEFAULT);
			curl = curl_easy_init();
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, setUrl().c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

				res = curl_easy_perform(curl);
				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
					return 0;
				curl_easy_cleanup(curl);
				}
			}
			curl_global_cleanup();
			return 1;
		};
		int dataExists()
		{
			if(folderExists())
			{
				if(fs::exists("data/" + countryId + "/" + indicatorId + ".csv"))
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		};
		int folderExists()
		{
			if(fs::exists("data") && fs::is_directory("data"))
			{
				if(fs::exists("data/" + countryId) && fs::is_directory("data/" + countryId))
				{
					return 1;
				}
				else
				{
					fs::create_directory("data/" + countryId);
					return 1;
				}
			}
			else
			{
				fs::create_directory("data");
				fs::create_directory("data/" + countryId);
				return 1;
			}
		};
		int saveData()
		{
			if(folderExists())
			{
				ofstream file;
				file.open("data/" + countryId + "/" + indicatorId + ".json");
				file << response;
				file.close();
				return 1;
			}
			else
			{
				return 0;
			}
		};
		void sortData()
		{
			data = GeneralTools::sortedData(data);
		};
		int convertData()
		{
			string command = "python3 data/convertDataJsonToCsv.py " + countryId + " " + indicatorId;
			return system(command.c_str());
		};
		~DataManager()
		{
			// cout << "DataManager destructor called for " << countryId << " " << indicatorId << endl;
			delete[] data.date;
			delete[] data.value;
		};
};

class CountriesIndicatorsManager {
	public:
		string countriesJson;
		string indicatorsJson;
		Countries countries;
		Indicators indicators;
		string url;
		int dataReady;
		CountriesIndicatorsManager()
		{
			dataReady = 0;
			countries.id = new string[400];
			countries.name = new string[400];
			countries.region = new string[400];
			indicators.id = new string[25000];
			indicators.name = new string[25000];
			indicators.sourceNote = new string[25000];
			readCSV();
		};
		string setUrlCountries()
		{
			url = "http://api.worldbank.org/v2/country/?format=json&per_page=400";
			return url;
		};
		string setUrlIndicators()
		{
			url = "http://api.worldbank.org/v2/indicator/?format=json&per_page=25000";
			return url;
		};
		static size_t writeCallback(char* ptr, size_t size, size_t nmemb, string* data) {
			data->append(ptr, size * nmemb);
			return size * nmemb;
		};
		int getData()
		{
			CURL* curl;
			CURLcode res;

			curl_global_init(CURL_GLOBAL_DEFAULT);
			curl = curl_easy_init();
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, setUrlCountries().c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &countriesJson);

				res = curl_easy_perform(curl);
				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
					return 0;
				curl_easy_cleanup(curl);
				}
			}
			curl = curl_easy_init();
			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, setUrlIndicators().c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &indicatorsJson);

				res = curl_easy_perform(curl);
				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
					return 0;
				curl_easy_cleanup(curl);
				}
			}
			curl_global_cleanup();
			return 1;
		};
		int saveData()
		{

			ofstream file;
			file.open("data/countries.json");
			file << countriesJson;
			file.close();
			file.open("data/indicators.json");
			file << indicatorsJson;
			file.close();
			return 1;
		};
		int convertData()
		{
			string command = "python3 data/convertCountriesJsonToCsv.py";
			system(command.c_str());
			command = "python3 data/convertIndicatorsJsonToCsv.py";
			system(command.c_str());
			return 1;
		};
		int dataExists()
		{
			if(fs::exists("data/countries.csv") && fs::exists("data/indicators.csv"))
			{
				return 1;
			}
			else
			{
				return 0;
			}
		};
		int readCSV()
		{
			if(!dataExists())
			{
				getData();
				saveData();
				convertData();
			}
			ifstream file;
			file.open("data/countries.csv");
			if(file.is_open())
			{
				string line;
				getline(file, line);
				int i = 0;
				while(getline(file, line))
				{
					stringstream ss(line);
					string id;
					string name;
					string region;
					getline(ss, id, '|');
					getline(ss, name, '|');
					getline(ss, region, '|');
					countries.id[i] = id;
					countries.name[i] = name;
					countries.region[i] = region;
					i++;
				}
				countries.size = i;
				file.close();
				file.open("data/indicators.csv");
				if(file.is_open())
				{
					getline(file, line);
					i = 0;
					while(getline(file, line))
					{
						stringstream ss(line);
						string id;
						string name;
						string sourceNote;
						getline(ss, id, '|');
						getline(ss, name, '|');
						getline(ss, sourceNote, '|');
						indicators.id[i] = id;
						indicators.name[i] = name;
						indicators.sourceNote[i] = sourceNote;
						i++;
					}
					indicators.size = i;
					file.close();
					dataReady = 1;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		};
		void printCountries()
		{
			if(dataReady)
			{
				for(int i = 0; i < countries.size; i++)
				{
					cout << countries.id[i] << " : " << countries.name[i] << " : " << countries.region[i] << endl;
				}
			}
			else
			{
				readCSV();
				printCountries();
			}
		};
		void printIndicators()
		{
			if(dataReady)
			{
				for(int i = 0; i < indicators.size; i++)
				{
					cout << indicators.id[i] << " : " << indicators.name[i] << " : " << indicators.sourceNote[i] << endl;
				}
			}
			else
			{
				readCSV();
				printIndicators();
			}
		};
		void printData()
		{
			if(dataReady)
			{
				for(int i = 0; i < countries.size; i++)
				{
					cout << countries.id[i] << " : " << countries.name[i] << " : " << countries.region[i] << endl;
				}
				for(int i = 0; i < indicators.size; i++)
				{
					cout << indicators.id[i] << " : " << indicators.name[i] << " : " << indicators.sourceNote[i] << endl;
				}
			}
			else
			{
				readCSV();
				printData();
			}
		};
		void importData()
		{
			if(!dataReady)
			{
				readCSV();
			}
		};
};

class GeneralIndicators{
	public:
		BookGeneralIndicators bookGeneralMacroIndicators;
		BookGeneralIndicators bookGeneralGrowthIndicators;
		BookGeneralIndicators bookGeneralExpensesIndicators;
		BookGeneralIndicators bookGeneralSocialIndicators;
		BookGeneralIndicators bookGeneralUnemploymentIndicators;
		BookGeneralIndicators * bookGeneralIndicators[5] = {&bookGeneralMacroIndicators, &bookGeneralGrowthIndicators, &bookGeneralExpensesIndicators, &bookGeneralSocialIndicators, &bookGeneralUnemploymentIndicators};
		int size = 5;
		int totalIndicators = 0;
		string generalMacroIndicators[6][2] = {
			{"population","SP.POP.TOTL"},
			{"GDP (current US $)","NY.GDP.MKTP.CD"},
			{"GDP per capita (current US $)","NY.GDP.PCAP.CD"},
			{"inflation, consumer prices (annual %)" , "FP.CPI.TOTL.ZG"},
			{"Unemployment, total (% of total labor force) (national estimate)", "SL.UEM.TOTL.NE.ZS"},
			{"CPI Price, nominal", "CPTOTNSXN"}
		};
		string generalGrowthIndicators[6][2] = {
			{"gdp growth", "NY.GDP.MKTP.KD.ZG"},
			{"Agriculture, forestry, and fishing, value added (annual % growth)", "NV.AGR.TOTL.KD.ZG"},
			{"Manufacturing, value added (annual % growth)", "NV.IND.MANF.KD.ZG"},
			{"Industry (including construction), value added (annual % growth)", "NV.IND.TOTL.KD.ZG"},
			{"Services, value added (annual % growth)", "NV.SRV.TOTL.KD.ZG"},
			{"GDP per capita (current US $)","NY.GDP.PCAP.CD"}
		};
		string generalExpensesIndicators[4][2] = {
			{"General government final consumption expenditure (% of GDP)", "NE.CON.GOVT.ZS"},
			{"Households and NPISHs final consumption expenditure (% of GDP)", "NE.CON.PRVT.ZS"},
			{"Gross fixed capital formation (% of GDP)", "NE.GDI.FTOT.ZS"},
			{"GDP (current US $)", "NY.GDP.MKTP.CD"},
		};
		string generalSocialIndicators[6][2] = {
			{"Population, total", "SP.POP.TOTL"},
			{"Population density (people per sq. km of land area)", "EN.POP.DNST"},
			{"Urban population (% of total population)", "SP.URB.TOTL.IN.ZS"},
			{"Life expectancy at birth, total (years)", "SP.DYN.LE00.IN"},
			{"Mortality rate under-5 (per 1,000 live births)", "SH.DYN.MORT"},
			{"Fertility rate, total (births per woman)","SP.DYN.TFRT.IN"},
		};
		string generalUnemploymentIndicators[6][2] = {
			{"Employment to population ratio, 15+, total (%) (modeled ILO estimate)", "SL.EMP.TOTL.SP.ZS"},
			{"Unemployment, total (% of total labor force) (national estimate)", "SL.UEM.TOTL.NE.ZS"},
			{"Employment to population ratio, 15+, female (%) (national estimate)", "SL.EMP.TOTL.SP.FE.ZS"},
			{"Unemployment with advanced education (% of total labor force with advanced education)", "SL.UEM.ADVN.ZS"},
			{"Population, female (% of total population)", "SP.POP.TOTL.FE.ZS"},
			{"Unemployment with basic education (% of total labor force with basic education)", "SL.UEM.BASC.ZS"},
		};
		GeneralIndicators()
		{
			bookGeneralMacroIndicators.type = "macro economy";
			bookGeneralGrowthIndicators.type = "gdp growth";
			bookGeneralExpensesIndicators.type = "expenses";
			bookGeneralSocialIndicators.type = "social";
			bookGeneralUnemploymentIndicators.type = "unemployment";
			bookGeneralMacroIndicators.size = 6;
			bookGeneralGrowthIndicators.size = 6;
			bookGeneralExpensesIndicators.size = 4;
			bookGeneralSocialIndicators.size = 6;
			bookGeneralUnemploymentIndicators.size = 6;
			for(int i = 0; i < 6; i++)
			{
				if(i < bookGeneralMacroIndicators.size)
				{
					bookGeneralMacroIndicators.generalIndicators[i][0] = generalMacroIndicators[i][0];
					bookGeneralMacroIndicators.generalIndicators[i][1] = generalMacroIndicators[i][1];
				}
				if(i < bookGeneralGrowthIndicators.size)
				{
					bookGeneralGrowthIndicators.generalIndicators[i][0] = generalGrowthIndicators[i][0];
					bookGeneralGrowthIndicators.generalIndicators[i][1] = generalGrowthIndicators[i][1];
				}
				if(i < bookGeneralExpensesIndicators.size)
				{
					bookGeneralExpensesIndicators.generalIndicators[i][0] = generalExpensesIndicators[i][0];
					bookGeneralExpensesIndicators.generalIndicators[i][1] = generalExpensesIndicators[i][1];
				}
				if(i < bookGeneralSocialIndicators.size)
				{
					bookGeneralSocialIndicators.generalIndicators[i][0] = generalSocialIndicators[i][0];
					bookGeneralSocialIndicators.generalIndicators[i][1] = generalSocialIndicators[i][1];
				}
				if(i < bookGeneralUnemploymentIndicators.size)
				{
					bookGeneralUnemploymentIndicators.generalIndicators[i][0] = generalUnemploymentIndicators[i][0];
					bookGeneralUnemploymentIndicators.generalIndicators[i][1] = generalUnemploymentIndicators[i][1];
				}
			}
		};
		BookGeneralIndicators getGeneralIndicators(string type)
		{
			for(int i = 0; i < size; i++)
			{
				if(GeneralTools::stringInStringEscapeCase(bookGeneralIndicators[i]->type, type))
				{
					return *bookGeneralIndicators[i];
				}
			}
			return *bookGeneralIndicators[0];
		};
		string * getGeneralIndicatorsId(string type)
		{
			BookGeneralIndicators bookGeneralIndicators = getGeneralIndicators(type);
			string * generalIndicatorsId = new string[bookGeneralIndicators.size];
			for(int i = 0; i < bookGeneralIndicators.size; i++)
			{
				generalIndicatorsId[i] = bookGeneralIndicators.generalIndicators[i][1];
			}
			return generalIndicatorsId;
		};
};


class Tools {
	public:
		Countries countries;
		Indicators indicators;
		Tools()
		{
			CountriesIndicatorsManager countriesIndicatorsManager;
			countriesIndicatorsManager.importData();
			countries = countriesIndicatorsManager.countries;
			indicators = countriesIndicatorsManager.indicators;
			// DataManager dataManager;
			// for (int i = 0; i < countries.size; i++)
			// {
			// 	cout << countries.id[i] << " : " << countries.name[i] << " : " << countries.region[i] << endl;
			// 	dataManager.countryId = countries.id[i];
			// 	dataManager.indicatorId = "NY.GDP.MKTP.CD";
			// 	dataManager.readCSV();
			// 	dataManager.indicatorId = "SP.POP.TOTL";
			// 	dataManager.readCSV();
			// 	dataManager.indicatorId = "NY.GDP.PCAP.CD";
			// 	dataManager.readCSV();
			// }
		};
		string findCountry(string countryNameResidual)
		{
			string countriesFound[30];
			for (int i = 0; i < 30; i++)
			{
				countriesFound[i] = "";
			}
			string countryName;
			for(int i = 0; i < countries.size; i++)
			{
				//search without case sensitivity
				countryName = countries.name[i];
				if(GeneralTools::stringInStringEscapeCase(countryName, countryNameResidual))
				{
					for (int j = 0; j < 30; j++)
					{
						if(countriesFound[j] == "")
						{
							countriesFound[j] = countries.name[i];
							
							break;
						}
					}
				}
			}
			if(countriesFound[0] == "")
			{
				cout << "No country found" << endl;
				string countryNameResidual;
				cout << "Enter a country name : ";
				getline(cin, countryNameResidual);
				return findCountry(countryNameResidual);
			}
			if (countriesFound[1] == "")
			{
				return countriesFound[0];
			}
			for (int j = 0; j < 30; j++)
			{
				if(countriesFound[j] != "")
				{
					cout << j << " : " << countriesFound[j] << endl;
				}
			}
			int choice;
			cout << "Choose a country : ";
			cin >> choice;
			if(choice >= 0 && choice < 30 && countriesFound[choice] != "")
			{
				cout << countriesFound[choice] << endl;
				return countriesFound[choice];
			}
			else
			{
				cout << "Invalid choice" << endl;
				return findCountry(countryNameResidual);
			};
		};
		string findCountryId(string countryNameResidual)
		{
			string countriesIdFound[30];
			string countryName;
			countryName = findCountry(countryNameResidual);
			for (int i = 0; i < 30; i++)
			{
				countriesIdFound[i] = "";
			}
			for(int i = 0; i < countries.size; i++)
			{
				if(countries.name[i] == countryName)
				{
					for (int j = 0; j < 30; j++)
					{
						if(countriesIdFound[j] == "")
						{
							countriesIdFound[j] = countries.id[i];
							break;
						}
					}
				}
			}
			if(countriesIdFound[0] == "")
			{
				cout << "No country found" << endl;
				string countryNameResidual;
				cout << "Enter a country name : ";
				getchar();
				getline(cin, countryNameResidual);
				return findCountryId(countryNameResidual);
			}
			if (countriesIdFound[1] == "")
			{
				return countriesIdFound[0];
			}
			for (int j = 0; j < 30; j++)
			{
				if(countriesIdFound[j] != "")
				{
					cout << j << " : " << countriesIdFound[j] << endl;
				}
			}
			int choice;
			cout << "Choose a country : ";
			cin >> choice;
			if(choice >= 0 && choice < 30 && countriesIdFound[choice] != "")
			{
				cout << countriesIdFound[choice] << endl;
				return countriesIdFound[choice];
			}
			else
			{
				cout << "Invalid choice" << endl;
				return findCountryId(countryNameResidual);
			};
		};
		string findCountryName(string countryId)
		{
			string countryName = "Country not found";
			for(int i = 0; i < countries.size; i++)
			{
				if(countries.id[i] == countryId)
				{
					countryName = countries.name[i];
				}
			}
			string newName;
			for (char caractere : countryName) {
				if (caractere != '.' && caractere != '\'') {
					newName += caractere;
				};
			};
			return newName;
		};
		string findIndicatorId(string indicatorNameResidual)
		{
			string indicatorsIdFound[1000];
			string indicatorsName[1000];
			string * indicatorNameResidualSplit = new string[20];
			stringstream ss(indicatorNameResidual);
			int sizeWords = 0;
			string word;
			while(getline(ss, word, ' '))
			{
				indicatorNameResidualSplit[sizeWords] = word;
				sizeWords++;
			}
			for (int i = 0; i < 1000; i++)
			{
				indicatorsIdFound[i] = "";
				indicatorsName[i] = "";
			}
			string indicatorName;
			for(int i = 0; i < indicators.size; i++)
			{
				//search without case sensitivity
				indicatorName = indicators.name[i];
				if(GeneralTools::wordsInStringEscapeCase(indicatorName, indicatorNameResidualSplit, sizeWords))
				{
					for (int j = 0; j < 1000; j++)
					{
						if(indicatorsIdFound[j] == "")
						{
							indicatorsIdFound[j] = indicators.id[i];
							indicatorsName[j] = indicators.name[i];
							break;
						}
					}
				}
			}
			if(indicatorsIdFound[0] == "")
			{
				cout << "No indicator found" << endl;
				string indicatorNameResidual;
				cout << "Enter an indicator name : " << endl;
				getchar();
				getline(cin, indicatorNameResidual);
				return findIndicatorId(indicatorNameResidual);
			}
			if (indicatorsIdFound[1] == "")
			{
				return indicatorsIdFound[0];
			}
			for (int j = 0; j < 1000; j++)
			{
				if(indicatorsIdFound[j] != "")
				{
					cout << j << " : " << indicatorsIdFound[j]<< " : " << indicatorsName[j] << endl;
				}
			}
			int choice;
			cout << "Choose an indicator : ";
			cin >> choice;
			if(choice >= 0 && choice < 1000 && indicatorsIdFound[choice] != "")
			{
				cout << indicatorsIdFound[choice] << endl;
				return indicatorsIdFound[choice];
			}
			else
			{
				cout << "Invalid choice" << endl;
				cout << "Enter an indicator name : ";
				getchar();
				getline(cin, indicatorNameResidual);
				return findIndicatorId(indicatorNameResidual);
			};
		};
		string findIndicatorName(string indicatorId){
			for(int i = 0; i < indicators.size; i++)
			{
				if(indicators.id[i] == indicatorId)
				{
					return indicators.name[i];
				}
			}
			return "Indicator not found";
		};	
		static int findCountryIndicatorIndexInDataset(DataManager dataManager[], string countryId, string indicatorId, int size)
		{
			for(int i = 0; i < size; i++)
			{
				if(dataManager[i].countryId == countryId && dataManager[i].indicatorId == indicatorId)
				{
					return i;
				}
			}
			return -1;
		};
		static int dateExists(DataManager dataManager, int date)
		{
			for(int i = 0; i < dataManager.data.size; i++)
			{
				if(dataManager.data.date[i] == date)
				{
					return i;
				}
			}
			return -1;
		};
		static Data * getData(DataManager dataManager, int start, int end)
		{
			GeneralTools::printArray(dataManager.data.date, dataManager.data.size);
			Data data = dataManager.data;
			Data * dataNew = new Data;
			dataNew->size = 0;
			for(int i = 0; i < data.size; i++)
			{
				if(data.date[i] >= start && data.date[i] <= end)
				{
					dataNew->size++;
				}
			}
			cout << "Size in The New Data : " << dataNew->size << endl;
			(*dataNew).date = new int[dataNew->size];
			(*dataNew).value = new double[dataNew->size];
			(*dataNew).size = dataNew->size;
			cout << "Start in getData: " << start << endl;
			int j = 0;
			for(int i = 0; i < data.size; i++)
			{
				if(data.date[i] >= start && data.date[i] <= end)
				{
					(*dataNew).date[j] = data.date[i];
					(*dataNew).value[j] = data.value[i];
					cout << "Date in getData: " << (*dataNew).date[j] << " : " << (*dataNew).value[j] << endl;
					j++;
				}
			}
			cout << "Data in getData: " << (*dataNew).date[0] << " : " << (*dataNew).value[0] << endl;
			GeneralTools::printArray((*dataNew).date, (*dataNew).size);
			return dataNew;
		};
		static double getDateValue(DataManager dataManager, int date)
		{
			DataManager * dataManagerCopy;
			dataManagerCopy = new DataManager(dataManager.countryId, dataManager.indicatorId);
			//cout << "Date in getDateValue: " << date << endl;
			int index = dateExists(*dataManagerCopy, date);
			//cout << "Index in getDateValue: " << index << endl;
			if(index != -1)
			{
				double value = (*dataManagerCopy).data.value[index];
				//cout << "Value in getDateValue: " << value << endl;
				dataManagerCopy = NULL;
				return value;
			}
			else
			{
				return -814.2003;
			}
		};
};

class DatasetManager {
	public:
		string * indicatorsId;
		string indicatorType;
		int sizeIndicators;
		int dataManagerSize;
		DataManager * dataManager;
		Tools tools;
		GeneralIndicators generalIndicators;
		string similarCountries[100];
		int numberOfSimilarCountries = 0;
		DatasetManager()
		{
		};
		DatasetManager(string indicatorType)
		{
			this->indicatorType = indicatorType;
			this->indicatorsId = generalIndicators.getGeneralIndicatorsId(indicatorType);
			this->sizeIndicators = generalIndicators.getGeneralIndicators(indicatorType).size;
			dataManagerSize = tools.countries.size*this->sizeIndicators;
			dataManager = new DataManager[dataManagerSize];
			for(int i = 0; i < tools.countries.size; i++)
			{
				for(int j = 0; j < sizeIndicators; j++)
				{
					dataManager[i*sizeIndicators+j].updateDataManager(tools.countries.id[i], this->indicatorsId[j]);
					//cout << "DataManager updated for1 " << tools.countries.id[i] << " " << this->indicatorsId[j] << endl; //<< "%\r";
					//cout << this->indicatorsId[j] << endl;
				}
				//cout << "                                                                                                      						\r";
			}
		};
		DatasetManager(string * countriesIdList, int sizeCountriesIdList,string indicatorType)
		{
			//cout << "DataManagerBRUH " << sizeCountriesIdList << " " << countriesIdList[sizeCountriesIdList-1] << endl;
			this->indicatorType = indicatorType;
			//cout << "something " << countriesIdList[sizeCountriesIdList-1] << endl;
			this->indicatorsId = generalIndicators.getGeneralIndicatorsId(indicatorType);
			this->sizeIndicators = generalIndicators.getGeneralIndicators(indicatorType).size;
			dataManagerSize = sizeCountriesIdList*this->sizeIndicators;
			dataManager = new DataManager[dataManagerSize];
			for(int i = 0; i < sizeCountriesIdList; i++)
			{
				for(int j = 0; j < sizeIndicators; j++)
				{
					dataManager[i*sizeIndicators+j].updateDataManager(countriesIdList[i], this->indicatorsId[j]);
					//cout << "DataManager updated for2 " << countriesIdList[i] << " " << this->indicatorsId[j] << endl;// "%\r";
					//cout << this->indicatorsId[j] << endl;
				}
				cout << "                                                                                                      						\r";
			}
		}
		void updateIndicator(string indicatorType)
		{
			this->indicatorsId = generalIndicators.getGeneralIndicatorsId(indicatorType);
			this->sizeIndicators = generalIndicators.getGeneralIndicators(indicatorType).size;
			dataManagerSize = tools.countries.size*this->sizeIndicators;
			delete[] dataManager;
			//cout << "DataManager deleted" << endl;
			dataManager = new DataManager[dataManagerSize];
			//cout << "DataManager created" << endl;
			for(int i = 0; i < tools.countries.size; i++)
			{
				for(int j = 0; j < sizeIndicators; j++)
				{
					dataManager[i*sizeIndicators+j].updateDataManager(tools.countries.id[i], this->indicatorsId[j]);
					//cout << "DataManager updated for3 " << tools.countries.id[i] << " " << this->indicatorsId[j] << endl;//"%\r";
					//cout << this->indicatorsId[j] << endl;
				}
				//cout << "                                                                                                      						\r";
			}
		};
		void changeNumberOfSimilarCountries(int number)
		{
			//cout << "Number of similar countries : " << number << endl;
			numberOfSimilarCountries = number;
		};
		void changeSimilarCountries(string similarCountries[],  int size)
		{
			for(int i = 0; i < 100; i++)
			{
				this->similarCountries[i] = "";
			}
			for(int i = 0; i < size; i++)
			{
				this->similarCountries[i] = similarCountries[i];
			}
			numberOfSimilarCountries = size;
		};
		~DatasetManager()
		{
			delete[] dataManager;
			//cout << "DatasetManager destructor called" << endl;
		};
		

};



class CountriesComparison {
	public:
		CountriesComparison()
		{
		};
		static void findSimilarCountries(DatasetManager &datasetManager ,string countryId, string indicatorId, int year, int degree)
		{
			string * countriesId;
			countriesId = new string[datasetManager.dataManagerSize];
			int sizeCountriesId = 0;
			for(int i = 0; i < datasetManager.dataManagerSize; i++)
			{
				if(datasetManager.dataManager[i].indicatorId == indicatorId)
				{
					//cout << "Country : " << datasetManager.dataManager[i].countryId << " : " << datasetManager.dataManager[i].indicatorId << endl;
					countriesId[sizeCountriesId] = datasetManager.dataManager[i].countryId;
					sizeCountriesId++;
				}
			}
			countriesId[sizeCountriesId-1] = countryId;
			DatasetManager * datasetManagerCopy = new DatasetManager(countriesId, sizeCountriesId, datasetManager.indicatorType);
			int index = Tools::findCountryIndicatorIndexInDataset(datasetManagerCopy->dataManager, countryId, indicatorId, datasetManagerCopy->dataManagerSize);
			string * similarCountriesNew = new string[datasetManagerCopy->tools.countries.size];
			double diff;
			double diffArray[datasetManagerCopy->dataManagerSize];
			int sizeSimilarCountriesNew = 0;
			if(index == -1)
			{
				cout << "Country or indicator not found" << endl;
				exit(0);
			}
			double value = datasetManagerCopy->dataManager[index].data.value[Tools::dateExists(datasetManagerCopy->dataManager[index], year)];
			if (value == -814.2003)
			{
				cout << "No data found for this year" << endl;
				exit(0);
			}
			for(int i = 0; i < datasetManagerCopy->dataManagerSize; i++)
			{
				if (i != index && datasetManagerCopy->dataManager[i].indicatorId == indicatorId)
				{
					diff = abs(value - datasetManagerCopy->dataManager[i].data.value[Tools::dateExists(datasetManagerCopy->dataManager[i], year)]);
					diffArray[i] = diff;
				}
				else
				{
					diffArray[i] = 0;
				}
			};
			double degreeOfComparison = (double)degree;
			for(int i = 0; i < datasetManagerCopy->dataManagerSize; i++)
			{
				if(diffArray[i] != 0)
				{
					if (diffArray[i]/value < degreeOfComparison/100.0)
					{
						//cout << "Country : " << datasetManagerCopy->dataManager[i].countryId << " : " << diffArray[i] / value << " : " << degreeOfComparison/100.0 << " : " << degreeOfComparison << endl;
						similarCountriesNew[sizeSimilarCountriesNew] = datasetManagerCopy->dataManager[i].countryId;
						sizeSimilarCountriesNew++;
					};
				};
			};
			//GeneralTools::printArray(similarCountriesNew, sizeSimilarCountriesNew);
			(&datasetManager)->changeSimilarCountries(similarCountriesNew, sizeSimilarCountriesNew);
			(&datasetManager)->changeNumberOfSimilarCountries(sizeSimilarCountriesNew);
			datasetManagerCopy = NULL;
			delete datasetManagerCopy;
		};
		static DatasetManager findSimilarCountries(string * countriesIdList, int sizeCountriesIdList, string countryId, string indicatorType, string indicatorId, int year, int degree)
		{
			string countriesIdListNew[sizeCountriesIdList+1];
			for(int i = 0; i < sizeCountriesIdList; i++)
			{
				countriesIdListNew[i] = countriesIdList[i];
				//cout << i <<"CountryId pay attention : " << countriesIdListNew[i] << endl;
			}
			countriesIdListNew[sizeCountriesIdList] = countryId;
			//cout << sizeCountriesIdList << "CountryId pay attention : " << countriesIdListNew[sizeCountriesIdList] << endl;
			sizeCountriesIdList++;
			DatasetManager datasetManager(countriesIdListNew, sizeCountriesIdList, indicatorType);
			findSimilarCountries(datasetManager, countryId, indicatorId, year, degree);
			return datasetManager;
		};
};

class Menu{
	public:
		GeneralIndicators generalIndicators;
		DatasetManager datasetManager;
		Tools tools;
		Menu()
		{
			mainMenu();
		};
		void mainMenu()
		{
			int choice;
			cout << "1 : Find similar countries" << endl;
			cout << "2 : Change indicator" << endl;
			cout << "3 : Exit" << endl;
			cout << "Enter a choice : ";
			cin >> choice;
			if(choice == 1)
			{
				findSimilarCountries();
			}
			else if(choice == 2)
			{
				changeIndicator();
			}
			else if(choice == 3)
			{
				exit(0);
			}
			else
			{
				cout << "Invalid choice" << endl;
				mainMenu();
			}
		};
		void findSimilarCountries()
		{
			DatasetManager datasetManager = DatasetManager("macro economy");
			string countryId;
			string indicatorId;
			int year;
			int degree;
			cout << "Enter a country name : ";
			getchar();
			getline(cin, countryId);
			countryId = tools.findCountryId(countryId);
			int index = Tools::findCountryIndicatorIndexInDataset(datasetManager.dataManager, countryId, "NY.GDP.MKTP.CD", datasetManager.dataManagerSize);
			indicatorId = "NY.GDP.MKTP.CD";
			cout << "Enter a year for Comparaison : ";
			cin >> year;
			while(Tools::getDateValue(datasetManager.dataManager[index], year) == -814.2003)
			{
				cout << "No data found for this year" << endl;
				cout << "Enter a year : ";
				cin >> year;
			}
			//cout << "BOOM" << endl;
			degree = 20;
			showSimilarToCountry(datasetManager, countryId, indicatorId, year, degree);
			mainMenu();
		};
		void showSimilarToCountry(DatasetManager datasetManager, string countryId, string indicatorId, int year, int degree)
		{
			CountriesComparison::findSimilarCountries(datasetManager, countryId, indicatorId, year, degree);
			while(datasetManager.numberOfSimilarCountries < 40 && degree < 80)
			{
				//cout << "Number of similar countries : " << datasetManager.numberOfSimilarCountries << endl;
				//cout << "Degree : " << degree << endl;
				CountriesComparison::findSimilarCountries(datasetManager, countryId, indicatorId, year, degree);
				degree += 10;
			}
			string similarCountries[datasetManager.numberOfSimilarCountries];
			cout << "--------------------------------" << endl;
			cin.get();
			GeneralTools::printArray(datasetManager.similarCountries, datasetManager.numberOfSimilarCountries);
			for(int i = 0; i < datasetManager.numberOfSimilarCountries; i++)
			{
				similarCountries[i] = datasetManager.similarCountries[i];
			}
			degree = 5;
			DatasetManager datasetManagerNew = CountriesComparison::findSimilarCountries(similarCountries, datasetManager.numberOfSimilarCountries, countryId, datasetManager.indicatorType, "NY.GDP.PCAP.CD", year, degree);
			while (datasetManagerNew.numberOfSimilarCountries < 5)
			{
				//cout << "Number of similar countries : " << datasetManagerNew.numberOfSimilarCountries << endl;
				//cout << "Degree : " << degree << endl;
				datasetManagerNew = CountriesComparison::findSimilarCountries(similarCountries, datasetManager.numberOfSimilarCountries, countryId, datasetManager.indicatorType, "NY.GDP.PCAP.CD", year, degree);
				degree += 2;
			}
			GeneralTools::printArray(datasetManagerNew.similarCountries, datasetManagerNew.numberOfSimilarCountries);
			datasetManager = datasetManagerNew;
			int start;
			cout << "Enter the year to start graph : ";
			cin >> start;
			generateGraphs(datasetManager, countryId, start);
		};
		void showSimilarToCountry(DatasetManager datasetManager, string countryId, string indicatorId, int year, int degree, int start)
		{
			CountriesComparison::findSimilarCountries(datasetManager, countryId, indicatorId, year, degree);
			while(datasetManager.numberOfSimilarCountries < 40 && degree < 80)
			{
				//cout << "Number of similar countries : " << datasetManager.numberOfSimilarCountries << endl;
				//cout << "Degree : " << degree << endl;
				CountriesComparison::findSimilarCountries(datasetManager, countryId, indicatorId, year, degree);
				degree += 10;
			}
			string similarCountries[datasetManager.numberOfSimilarCountries];
			cout << "--------------------------------" << endl;
			cin.get();
			GeneralTools::printArray(datasetManager.similarCountries, datasetManager.numberOfSimilarCountries);
			for(int i = 0; i < datasetManager.numberOfSimilarCountries; i++)
			{
				similarCountries[i] = datasetManager.similarCountries[i];
			}
			degree = 5;
			DatasetManager datasetManagerNew = CountriesComparison::findSimilarCountries(similarCountries, datasetManager.numberOfSimilarCountries, countryId, datasetManager.indicatorType, "NY.GDP.PCAP.CD", year, degree);
			while (datasetManagerNew.numberOfSimilarCountries < 5)
			{
				//cout << "Number of similar countries : " << datasetManagerNew.numberOfSimilarCountries << endl;
				//cout << "Degree : " << degree << endl;
				datasetManagerNew = CountriesComparison::findSimilarCountries(similarCountries, datasetManager.numberOfSimilarCountries, countryId, datasetManager.indicatorType, "NY.GDP.PCAP.CD", year, degree);
				degree += 2;
			}
			GeneralTools::printArray(datasetManagerNew.similarCountries, datasetManagerNew.numberOfSimilarCountries);
			datasetManager = datasetManagerNew;
			generateGraphs(datasetManager, countryId, start);
		};
		void generateGraphs(DatasetManager datasetManager, string mainCountryId, int start)
		{
			GeneralTools::folderExists("graphs");
			GeneralTools::folderExists("graphs/" + tools.findCountryName(mainCountryId));
			DataManager * dataManager;
			for(int i = 0; i < 5; i++)
			{
				for(int j = 0; j < generalIndicators.bookGeneralIndicators[i]->size; j++)
				{
					GeneralTools::folderExists("graphs/" + tools.findCountryName(mainCountryId) + "/" + generalIndicators.bookGeneralIndicators[i]->type);
					string folder = "graphs/" + tools.findCountryName(mainCountryId) + "/" + generalIndicators.bookGeneralIndicators[i]->type + "/";
					dataManager = new DataManager[datasetManager.numberOfSimilarCountries+1];
					for(int k = 0; k < datasetManager.numberOfSimilarCountries; k++)
					{
						dataManager[k].updateDataManager(datasetManager.similarCountries[k], generalIndicators.bookGeneralIndicators[i]->generalIndicators[j][1]);
					}
					dataManager[datasetManager.numberOfSimilarCountries].updateDataManager(mainCountryId, generalIndicators.bookGeneralIndicators[i]->generalIndicators[j][1]);
					string fileName = folder + tools.findCountryName(mainCountryId) + "_" + generalIndicators.bookGeneralIndicators[i]->generalIndicators[j][0];
					Graphs graphs(fileName, generalIndicators.bookGeneralIndicators[i]->generalIndicators[j][0], datasetManager.numberOfSimilarCountries+1);
					for(int k = 0; k < datasetManager.numberOfSimilarCountries+1; k++)
					{
						//cout << "Country : " << dataManager[k].countryId << " : " << dataManager[k].indicatorId << endl;
						
						graphs.plot(tools.findCountryName(dataManager[k].countryId), dataManager[k].getData(start, 2024));
					}
					graphs.send();
					delete[] dataManager;
				}
			}
		};
		void changeIndicator()
		{
			string indicatorType;
			cout << "Enter an indicator type : ";
			getchar();
			getline(cin, indicatorType);
			datasetManager.updateIndicator(indicatorType);
			mainMenu();
		};
};












//class CountriesComparison {
//	public:
//		string * countriesId;
//		string * indicatorsId;
//		int sizeCountries;
//		int sizeIndicators;
//		int dataManagerSize;
//		DataManager * dataManager;
//		Tools tools;
//		string similarCountries[100];
//		int numberOfSimilarCountries = 0;
//		CountriesComparison(string *countries, string *indicators, int sizeCountries, int sizeIndicators)
//		{
//			this->sizeCountries = sizeCountries;
//			this->sizeIndicators = sizeIndicators;
//			dataManagerSize = sizeCountries*sizeIndicators;
//			countriesId = new string[sizeCountries];
//			indicatorsId = new string[sizeIndicators];
//			dataManager = new DataManager[sizeCountries*sizeIndicators];
//			for(int i = 0; i < sizeCountries; i++)
//			{
//				countriesId[i] = countries[i];
//			}
//			for(int i = 0; i < sizeIndicators; i++)
//			{
//				indicatorsId[i] = indicators[i];
//			}
//			for(int i = 0; i < sizeCountries; i++)
//			{
//				for(int j = 0; j < sizeIndicators; j++)
//				{
//					cout << "DataManager created for " << countriesId[i] << " " << indicatorsId[j] << endl;
//					dataManager[i*sizeIndicators+j].updateDataManager(countriesId[i], indicatorsId[j]);
//				}
//			}
//		};
//		void findSimilarCountries(string countryId, string indicatorId, int year, int degree)
//		{
//			int index = Tools::findCountryIndicatorIndexInDataset(dataManager, countryId, indicatorId, sizeCountries*sizeIndicators);
//			string * similarCountriesNew = new string[sizeCountries];
//			double diff;
//			double diffArray[sizeCountries*sizeIndicators];
//			int sizeSimilarCountriesNew = 0;
//			if(index == -1)
//			{
//				cout << "Country or indicator not found" << endl;
//				exit(0);
//			}
//			double value = dataManager[index].data.value[Tools::dateExists(dataManager[index], year)];
//			if (value == -814.2003)
//			{
//				cout << "No data found for this year" << endl;
//				exit(0);
//			}
//			cout << "Value : " << value << " for " << countryId << " " << indicatorId << " in " << year << endl;
//			for(int i = 0; i < sizeCountries*sizeIndicators; i++)
//			{
//				if (i != index && dataManager[i].indicatorId == indicatorId)
//				{
//					diff = abs(value - dataManager[i].data.value[Tools::dateExists(dataManager[i], year)]);
//					diffArray[i] = diff;
//					cout << dataManager[i].countryId << " : " << diff << endl;
//				}
//				else
//				{
//					diffArray[i] = 0;
//				}
//				
//			}
//			for(int i = 0; i < sizeCountries*sizeIndicators; i++)
//			{
//				if(diffArray[i] != 0)
//				{
//					if (diffArray[i]/value < (double)degree/100)
//					{
//						similarCountriesNew[sizeSimilarCountriesNew] = dataManager[i].countryId;
//						sizeSimilarCountriesNew++;
//						cout << dataManager[i].countryId << " : " << diffArray[i] << endl;
//					}
//				}
//
//			}
//			GeneralTools::printArray(similarCountriesNew, sizeSimilarCountriesNew);
//			for(int i = 0; i < sizeSimilarCountriesNew; i++)
//			{
//				similarCountries[i] = similarCountriesNew[i];
//			}
//			this->numberOfSimilarCountries = sizeSimilarCountriesNew;
//			delete[] similarCountriesNew;
//			cout << "Number of similar countries : " << numberOfSimilarCountries << endl;
//		};
//		static void findSimilarCountries(CountriesComparison countriesComparison,string countryId, string indicatorId, int year, int degree)
//		{
//			DataManager * dataManager = countriesComparison.dataManager;
//			int sizeCountries = countriesComparison.sizeCountries;
//			int sizeIndicators = countriesComparison.sizeIndicators;
//			int index = Tools::findCountryIndicatorIndexInDataset(dataManager, countryId, indicatorId, sizeCountries*sizeIndicators);
//			string * similarCountriesNew = new string[sizeCountries];
//			double diff;
//			double diffArray[sizeCountries*sizeIndicators];
//			int sizeSimilarCountriesNew = 0;
//			if(index == -1)
//			{
//				cout << "Country or indicator not found" << endl;
//				exit(0);
//			}
//			double value = dataManager[index].data.value[Tools::dateExists(dataManager[index], year)];
//			if (value == -814.2003)
//			{
//				cout << "No data found for this year" << endl;
//				exit(0);
//			}
//			cout << "Value : " << value << " for " << countryId << " " << indicatorId << " in " << year << endl;
//			for(int i = 0; i < sizeCountries*sizeIndicators; i++)
//			{
//				if (i != index && dataManager[i].indicatorId == indicatorId)
//				{
//					diff = abs(value - dataManager[i].data.value[Tools::dateExists(dataManager[i], year)]);
//					diffArray[i] = diff;
//					cout << dataManager[i].countryId << " : " << diff << endl;
//				}
//				else
//				{
//					diffArray[i] = 0;
//				}
//				
//			}
//			for(int i = 0; i < sizeCountries*sizeIndicators; i++)
//			{
//				if(diffArray[i] != 0)
//				{
//					if (diffArray[i]/value < (double)degree/100)
//					{
//						similarCountriesNew[sizeSimilarCountriesNew] = dataManager[i].countryId;
//						sizeSimilarCountriesNew++;
//						cout << dataManager[i].countryId << " : " << diffArray[i] << endl;
//					}
//				}
//
//			}
//			GeneralTools::printArray(similarCountriesNew, sizeSimilarCountriesNew);
//			for(int i = 0; i < sizeSimilarCountriesNew; i++)
//			{
//				countriesComparison.similarCountries[i] = similarCountriesNew[i];
//			}
//			countriesComparison.numberOfSimilarCountries = sizeSimilarCountriesNew;
//			delete[] similarCountriesNew;
//			cout << "Number of similar countries : " << countriesComparison.numberOfSimilarCountries << endl;
//		};
//		~CountriesComparison()
//		{
//			cout << "CountriesComparison destructor called" << endl;
//		};
//};
//
//			
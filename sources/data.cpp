#include <curl/curl.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

typedef struct Data {
	int * date;
	double * value;
	int size;
} Data;

typedef struct Countries {
	string * id;
	string * name;
	string * region;
	int size;
} Countries;

typedef struct Indicators {
	string * id;
	string * name;
	string * sourceNote;
	int size;
} Indicators;

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
				getline(file, line);
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
		int getData()
		{
			CURL* curl;
			CURLcode res;

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
				if(fs::exists("data/" + countryId + "/" + indicatorId + ".json"))
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
		int convertData()
		{
			string command = "python data/convertDataJsonToCsv.py " + countryId + " " + indicatorId;
			return system(command.c_str());
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
			string command = "python data/convertCountriesJsonToCsv.py";
			system(command.c_str());
			command = "python data/convertIndicatorsJsonToCsv.py";
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
		};
		string findCountry(string countryNameResidual)
		{
			string countriesFound[30];
			for (int i = 0; i < 30; i++)
			{
				countriesFound[i] = "";
			}
			transform(countryNameResidual.begin(), countryNameResidual.end(), countryNameResidual.begin(), ::tolower);
			string countryName;
			for(int i = 0; i < countries.size; i++)
			{
				//search without case sensitivity
				countryName = countries.name[i];
				transform(countryName.begin(), countryName.end(), countryName.begin(), ::tolower);
				if(countryName.find(countryNameResidual) != string::npos)
				{
					for (int j = 0; j < 30; j++)
					{
						if(countriesFound[j] == "")
						{
							countriesFound[j] = countries.name[i];
							
							break;
						}
					}
					cout << countries.id[i] << " : " << countries.name[i] << " : " << countries.region[i] << endl;
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
				cout << "Only one country found" << endl;
				cout << countriesFound[0] << endl;
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
					cout << countries.id[i] << " : " << countries.name[i] << " : " << countries.region[i] << endl;
				}
			}
			if(countriesIdFound[0] == "")
			{
				cout << "No country found" << endl;
				string countryNameResidual;
				cout << "Enter a country name : ";
				getline(cin, countryNameResidual);
				return findCountryId(countryNameResidual);
			}
			if (countriesIdFound[1] == "")
			{
				cout << "Only one country found" << endl;
				cout << countriesIdFound[0] << endl;
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
		string findIndicatorId(string indicatorNameResidual)
		{
			string indicatorsIdFound[1000];
			string indicatorsName[1000];
			for (int i = 0; i < 1000; i++)
			{
				indicatorsIdFound[i] = "";
				indicatorsName[i] = "";
			}
			transform(indicatorNameResidual.begin(), indicatorNameResidual.end(), indicatorNameResidual.begin(), ::tolower);
			string indicatorName;
			for(int i = 0; i < indicators.size; i++)
			{
				//search without case sensitivity
				indicatorName = indicators.name[i];
				transform(indicatorName.begin(), indicatorName.end(), indicatorName.begin(), ::tolower);
				if(indicatorName.find(indicatorNameResidual) != string::npos)
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
				cout << "Enter an indicator name : ";
				getline(cin, indicatorNameResidual);
				return findIndicatorId(indicatorNameResidual);
			}
			if (indicatorsIdFound[1] == "")
			{
				cout << "Only one indicator found" << endl;
				cout << indicatorsIdFound[0] << " "<< indicatorsName[0] << endl;
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
				return findIndicatorId(indicatorNameResidual);
			};
		}			

};


	

			
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include<unistd.h> 

#ifndef STRUCTS_H
#define STRUCTS_H

using namespace std;
namespace fs = std::filesystem;

typedef struct Data {
	int * date;
	double * value;
	int size;
} Data;

typedef Data ** Dataset;

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

typedef struct BookGeneralIndicators {
	string type;
	string generalIndicators[6][2];
	int size;
} BookGeneralIndicators;

#endif
#include <iostream>
#include <gnuplot-iostream.h>
#include <string>
#include "../headers/structs.h"

using namespace std;
#ifndef GRAPHS_H
#define GRAPHS_H

class Graphs{
	public:
		Gnuplot gp;
		std::vector<std::pair<int, double>> plotted[20];
		int plotSize = 0;
		Graphs(string nameFile)
		{
		    gp << "set terminal pngcairo enhanced font 'Arial,10' size 1280, 720\n"; // UHD resolution
			gp << "set output '"<< nameFile << ".png" <<"'\n";  // Nom du fichier de sortie
			gp << "set key font ',20'\n";
			gp << "set key left top\n";
			gp << "set style line 1 lc rgb '#0060ad' lt 1 lw 6 pt 7 ps 1.5\n"; // Blue line
			gp << "set style line 2 lc rgb '#dd181f' lt 1 lw 3 pt 7 ps 1.5\n"; // Red line
			gp << "set style line 3 lc rgb '#FFA500' lt 1 lw 3 pt 7 ps 1.5\n"; // Orange line
			gp << "set style line 4 lc rgb '#00FF00' lt 1 lw 3 pt 7 ps 1.5\n"; // Green line
			gp << "set style line 5 lc rgb '#FF00FF' lt 1 lw 3 pt 7 ps 1.5\n"; // Purple line
			gp << "set style line 6 lc rgb '#FFFF00' lt 1 lw 3 pt 7 ps 1.5\n"; // Yellow line
			gp << "set style line 7 lc rgb '#00FFFF' lt 1 lw 3 pt 7 ps 1.5\n"; // Cyan line
			gp << "set style line 8 lc rgb '#000000' lt 1 lw 3 pt 7 ps 1.5\n"; // Black line
		};
		void plot(string title, Data data)
		{
			std::vector<std::pair<int, double>> dataPlot;
			for (int i = 0; i < data.size; i++) {
				cout << "date : " << data.date[i] << " value : " << data.value[i] << endl;
				dataPlot.push_back(std::make_pair(data.date[i], data.value[i]));
			}
			gp << "plot '-' with lines linestyle " << plotSize + 1 <<" title '"<< title <<",";
			plotted[plotSize] = dataPlot;
			plotSize++;
		}
		void plotXaxe(Data data)
		{
			std::vector<std::pair<int, double>> dataPlot;
			for (int i = 0; i < data.size; i++) {
				dataPlot.push_back(std::make_pair(data.date[i], 0));
			}
			gp << "plot '-' with lines linestyle " << 8 <<" title 'X axe',";
			plotted[plotSize] = dataPlot;
			plotSize++;
		};
		void send()
		{
			gp << "\n";
			for (int i = 0; i < plotSize; i++) {
				gp.send(plotted[i]);
			}
			gp << "e\n";
		};
};

#endif
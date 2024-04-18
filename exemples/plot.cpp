#include <iostream>
#include <gnuplot-iostream.h>

//g++ -o plt.exe plot.cpp -I gnuplot-iostream -lboost_iostreams
int main() {
    Gnuplot gp;

    // Paramètres de la fonction
    int x_min = 0, x_max = 50;
    int num_pts = 100;
    int dx = 1;

    // Données à tracer
    std::vector<std::pair<int, double>> data[3];
    for (int x = x_min; x <= x_max; x += dx) {
        double y1 = x * x*100000;          // Exemple de fonction 1
        double y2 = 2 * x*100000 + 3;      // Exemple de fonction 2
        double y3 = 0.5 * x*100000 + 1.5;  // Exemple de fonction 3
        data[1].push_back(std::make_pair(x, y1));
        data[2].push_back(std::make_pair(x, y2));
        data[0].push_back(std::make_pair(x, y3));
    }

    // Tracé du graphique
    gp << "set terminal pngcairo enhanced font 'Arial,10' size 1280, 720\n"; // UHD resolution
    gp << "set output 'output.png'\n";  // Nom du fichier de sortie
    gp << "set key font ',20'\n";
    gp << "set key left top\n";
    gp << "set style line 1 lc rgb '#0060ad' lt 1 lw 6 pt 7 ps 1.5\n"; // Blue line
    gp << "set style line 2 lc rgb '#dd181f' lt 1 lw 3 pt 7 ps 1.5\n"; // Red line
    gp << "set style line 3 lc rgb '#FFA500' lt 1 lw 3 pt 7 ps 1.5\n"; // Orange line
    gp << "set style line 4 lc rgb '#00FF00' lt 1 lw 3 pt 7 ps 1.5\n"; // Green line
    gp << "set style line 5 lc rgb '#FF00FF' lt 1 lw 3 pt 7 ps 1.5\n"; // Purple line
    gp << "set style line 6 lc rgb '#FFFF00' lt 1 lw 3 pt 7 ps 1.5\n"; // Yellow line
    gp << "set style line 7 lc rgb '#00FFFF' lt 1 lw 3 pt 7 ps 1.5\n"; // Cyan line
    gp << "plot '-' with lines linestyle 1 title 'x^2', '-' with lines linestyle 2 title '2x+3', '-' with lines linestyle 3 title '0.5x+1.5'\n";
    gp.send(data[1]);  // Envoie des données à gnuplot pour la première courbe
    gp.send(data[2]);  // Envoie des données à gnuplot pour la deuxième courbe
    gp.send(data[0]);  // Envoie des données à gnuplot pour la troisième courbe
    // gp << "e\n";      // Marque la fin des données pour chaque courbe

    return 0;
}

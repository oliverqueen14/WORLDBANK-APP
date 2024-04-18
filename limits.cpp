#include <iostream>
#include <limits>

int main() {
    std::cout << "Taille de double en octets : " << sizeof(double) << std::endl;
    std::cout << "Valeur maximale de double : " << std::numeric_limits<double>::max() << std::endl;
    std::cout << "Valeur minimale de double : " << std::numeric_limits<double>::min() << std::endl;

    return 0;
}

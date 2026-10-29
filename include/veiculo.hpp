#pragma once

#include <string>
class Veiculo {
    private:
        std::string placa;
        std::string modelo;
        int ano;
    public:
        Veiculo(std::string p, std::string m, int a)  : placa(p), modelo(m), ano(a) {}
        Veiculo(const Veiculo& vehicle) = default;
        Veiculo& operator=(const Veiculo& other) = default;
        ~Veiculo() = default;
        std::string getPlate() { return placa; }
        std::string getModel() { return modelo; }
        int getYear() { return ano; }
        void setPlate(std::string p) { placa = p; }
        void setModel(std::string m) { modelo = m; }
        void setYear(int y) { ano = y; }
};
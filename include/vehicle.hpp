#pragma once

#include <string>
class Vehicle {
    private:
        std::string placa;
        std::string modelo;
        int ano;
    public:
        Vehicle(std::string plate, std::string model, int year)  : placa(plate), modelo(model), ano(year) {}
        std::string plate() const { return placa; }
        std::string model() const { return modelo; }
        int year() const { return ano; }
        void setPlate(std::string p) { placa = p; }
        void setModel(std::string m) { modelo = m; }
        void setYear(int y) { ano = y; }
};
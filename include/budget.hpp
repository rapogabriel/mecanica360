#pragma once

class Budget {
    private:
        double valorPecas;
        double valorMaoDeObra;
        int prazoDias;
    public:
        Budget(double partValue, double laborValue, int deadline): valorPecas(partValue), valorMaoDeObra(laborValue), prazoDias(deadline) {}
        double total() { return valorPecas + valorMaoDeObra; }
};
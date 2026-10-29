#pragma once

#include <string>
class NecessaryPart {
    private:
        std::string descricao;
        int quantidade;
    public:
        NecessaryPart(std::string description, int quantity) : descricao(description), quantidade(quantity) {}
        std::string description() const { return descricao; }
        int quantity() const { return quantidade; }
        void setDescription(std::string d){ descricao = d; }
        void setQuantity(int q){ quantidade = q; }

};
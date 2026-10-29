#pragma once

#include "order_status.hpp"
#include <ctime>
class StatusHistory {
    private:
        OrderStatus estadoAnterior;
        OrderStatus estadoNovo;
        int responsavelId;
        time_t instante;
    public:
        StatusHistory(OrderStatus previous, OrderStatus current, int id, time_t instant) : estadoAnterior(previous), estadoNovo(current), responsavelId(id), instante(instant) {}
        OrderStatus getPrevious() { return estadoAnterior; }
        OrderStatus getCurrent() { return estadoNovo; }
        int getId() { return responsavelId; }
        time_t getInstant() { return instante; }
        void setPrevious(OrderStatus newPrevious) { estadoAnterior = newPrevious; }
        void setCurrent(OrderStatus newCurrent) { estadoNovo = newCurrent; }
        void setId(int newId) { responsavelId = newId; }
        void setInstant(time_t newInstant) { instante = newInstant; }
};
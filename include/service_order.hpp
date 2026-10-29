#pragma once

#include "order_status.hpp"

class ServiceOrder {
    int id_;
    int clienteId_;
    int mecanicoId_;
    OrderStatus estado_;

public:
    ServiceOrder(int id, int clienteId, int mecanicoId, OrderStatus estado)
        : id_(id), clienteId_(clienteId), mecanicoId_(mecanicoId), estado_(estado) {}

    int id() const { return id_; }
    int clientId() const { return clienteId_; }
    int mechanicId() const { return mecanicoId_; }
    OrderStatus status() const { return estado_; }
};

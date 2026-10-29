#pragma once
#include <map>
#include "ServiceOrderRepository.hpp"

// Repositorio em memoria, usado apenas para testes (emula um banco de dados).
class RepositorioFake : public ServiceOrderRepository {
    std::map<int, ServiceOrder> ordens_;
    int proximoId_ = 1;

public:
    int insert(const ServiceOrder& order) override {
        int novoId = proximoId_++;
        ordens_.insert_or_assign(novoId, ServiceOrder(novoId, order.clientId(),
                                                      order.mechanicId(), order.status()));
        return novoId;
    }

    void update(const ServiceOrder& order) override {
        auto it = ordens_.find(order.id());
        if (it != ordens_.end()) it->second = order;
    }

    std::optional<ServiceOrder> findById(int id) const override {
        auto it = ordens_.find(id);
        if (it == ordens_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<ServiceOrder> listAll() const override {
        std::vector<ServiceOrder> resultado;
        for (const auto& [id, os] : ordens_) resultado.push_back(os);
        return resultado;
    }

    // TODO (#6): implementar as buscas abaixo
    std::vector<ServiceOrder> listByStatus(OrderStatus) const override { return {}; }
    std::vector<ServiceOrder> listByClient(int) const override { return {}; }
    std::vector<ServiceOrder> listByMechanic(int) const override { return {}; }
};

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "FakeRepository.h"
#include "OpeningData.h"
#include "ServiceOrder.h"
#include "ServiceOrderRepository.h"
#include "Types.h"
#include "ValueClasses.h"

namespace fx {

using namespace mecanica360;

inline User customer(int id = 10) { return User(id, "Cliente", Role::Customer); }
inline User mechanic(int id = 7) { return User(id, "Mecanico", Role::Mechanic); }
inline User stockroom(int id = 5) { return User(id, "Almoxarife", Role::Stockroom); }
inline User manager(int id = 1) { return User(id, "Gerente", Role::Manager); }

/// Dados do CT-01.1: cliente 10, Gol ABC1D23, freios desgastados, 4 pastilhas, R$ 300, 3 dias.
inline OpeningData validOpening(int clientId = 10) {
    OpeningData d;
    d.clientId = clientId;
    d.vehicle = Vehicle("ABC1D23", "Gol", 2015);
    d.diagnosis = "freios desgastados";
    d.parts = {NecessaryPart("pastilha de freio", 4)};
    d.labor = 300.0;
    d.deadline = 3;
    return d;
}

/// Insere uma OS ja cadastrada e a leva ao estado pedido (via dominio). Devolve o id gerado.
inline int seed(ServiceOrderRepository& repo, int clientId, int mechanicId,
                OrderStatus status = OrderStatus::PendingBudget) {
    ServiceOrder so = ServiceOrder::create(validOpening(clientId), mechanicId);
    if (status == OrderStatus::PendingAcceptance) {
        so.attachBudget(Budget(800.0, 300.0, 3));
        so.transition(status, 5);
    } else if (status == OrderStatus::RejectedLackOfStock) {
        so.transition(status, 5);
    }
    return repo.insert(so);
}

/// Resumo comparavel de uma OS (estado, orcamento e historico).
struct Snapshot {
    OrderStatus state;
    bool hasBudget;
    double total;
    std::size_t historySize;
    bool operator==(const Snapshot&) const = default;
};

inline Snapshot snap(const ServiceOrder& so) {
    auto b = so.budget();
    return {so.state(), b.has_value(), b ? b->total() : 0.0, so.history().size()};
}

inline Snapshot snapOf(const ServiceOrderRepository& repo, int id) {
    return snap(*repo.findById(id));
}

inline std::vector<int> ids(const std::vector<ServiceOrder>& v) {
    std::vector<int> out;
    for (const auto& so : v) out.push_back(so.id());
    return out;
}

}  // namespace fx

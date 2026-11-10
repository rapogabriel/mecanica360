#pragma once

#include <optional>
#include <vector>

#include "ServiceOrder.h"

namespace mecanica360 {

// Contrato de armazenamento das ordens (classe abstrata em C++).
class ServiceOrderRepository {
public:
    virtual ~ServiceOrderRepository() = default;

    // Atribui o ID unico, guarda a ordem e devolve o ID.
    virtual int insert(ServiceOrder order) = 0;
    virtual void update(const ServiceOrder& order) = 0;
    virtual std::optional<ServiceOrder> findById(int id) const = 0;
    virtual std::vector<ServiceOrder> listByState(OrderStatus status) const = 0;
    virtual std::vector<ServiceOrder> listByCustomer(int clientId) const = 0;
    virtual std::vector<ServiceOrder> listByMechanic(int mechanicId) const = 0;
    virtual std::vector<ServiceOrder> listAll() const = 0;
};

}  // namespace mecanica360
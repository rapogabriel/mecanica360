#pragma once
#include <optional>
#include <vector>
#include "ServiceOrder.hpp"

class ServiceOrderRepository {
public:
    virtual ~ServiceOrderRepository() = default;

    virtual int insert(const ServiceOrder& order) = 0;
    virtual void update(const ServiceOrder& order) = 0;
    virtual std::optional<ServiceOrder> findById(int id) const = 0;
    virtual std::vector<ServiceOrder> listByStatus(OrderStatus status) const = 0;
    virtual std::vector<ServiceOrder> listByClient(int clientId) const = 0;
    virtual std::vector<ServiceOrder> listByMechanic(int mechanicId) const = 0;
    virtual std::vector<ServiceOrder> listAll() const = 0;
};

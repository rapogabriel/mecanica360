#include "FakeRepository.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace mecanica360 {

int FakeRepository::insert(ServiceOrder order) {
    const int id = nextId_++;
    order.assignId(id);
    orders_.push_back(std::move(order));
    return id;
}

void FakeRepository::update(const ServiceOrder& order) {
    auto it = std::find_if(orders_.begin(), orders_.end(),
                           [&](const ServiceOrder& o) { return o.id() == order.id(); });
    if (it == orders_.end()) {
        throw std::out_of_range("FakeRepository::update: id inexistente");
    }
    *it = order;
}

std::optional<ServiceOrder> FakeRepository::findById(int id) const {
    auto it = std::find_if(orders_.begin(), orders_.end(),
                           [&](const ServiceOrder& o) { return o.id() == id; });
    if (it == orders_.end()) return std::nullopt;
    return *it;
}

std::vector<ServiceOrder> FakeRepository::listByState(OrderStatus status) const {
    std::vector<ServiceOrder> out;
    std::copy_if(orders_.begin(), orders_.end(), std::back_inserter(out),
                 [&](const ServiceOrder& o) { return o.state() == status; });
    return out;
}

std::vector<ServiceOrder> FakeRepository::listByCustomer(int clientId) const {
    std::vector<ServiceOrder> out;
    std::copy_if(orders_.begin(), orders_.end(), std::back_inserter(out),
                 [&](const ServiceOrder& o) { return o.clientId() == clientId; });
    return out;
}

std::vector<ServiceOrder> FakeRepository::listByMechanic(int mechanicId) const {
    std::vector<ServiceOrder> out;
    std::copy_if(orders_.begin(), orders_.end(), std::back_inserter(out),
                 [&](const ServiceOrder& o) { return o.mechanicId() == mechanicId; });
    return out;
}

std::vector<ServiceOrder> FakeRepository::listAll() const { return orders_; }

}  // namespace mecanica360

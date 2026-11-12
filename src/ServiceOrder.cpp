#include "ServiceOrder.h"

#include <ctime>
#include <stdexcept>

namespace mecanica360 {

ServiceOrder ServiceOrder::create(const OpeningData& data, int mechanicId) {
    ServiceOrder order;
    order.clientId_ = data.clientId;
    order.mechanicId_ = mechanicId;
    order.vehicle_ = data.vehicle;
    order.diagnosis_ = data.diagnosis;
    order.parts_ = data.parts;
    order.labor_ = data.labor;
    order.deadline_ = data.deadline;
    order.state_ = OrderStatus::PendingBudget;
    return order;
}

int ServiceOrder::id() const { return id_; }
int ServiceOrder::clientId() const { return clientId_; }
int ServiceOrder::mechanicId() const { return mechanicId_; }
const Vehicle& ServiceOrder::vehicle() const { return vehicle_; }
const std::string& ServiceOrder::diagnosis() const { return diagnosis_; }
const std::vector<NecessaryPart>& ServiceOrder::parts() const { return parts_; }
double ServiceOrder::labor() const { return labor_; }
int ServiceOrder::deadline() const { return deadline_; }
OrderStatus ServiceOrder::state() const { return state_; }
std::optional<Budget> ServiceOrder::budget() const { return budget_; }
const std::vector<StatusHistory>& ServiceOrder::history() const { return history_; }

void ServiceOrder::assignId(int id) { id_ = id; }

bool ServiceOrder::canTransition(OrderStatus target) const {
    return state_ == OrderStatus::PendingBudget &&
           (target == OrderStatus::PendingAcceptance ||
            target == OrderStatus::RejectedLackOfStock);
}

void ServiceOrder::transition(OrderStatus target, int responsibleId) {
    if (!canTransition(target)) {
        throw std::logic_error("ServiceOrder::transition: transicao nao permitida");
    }
    history_.emplace_back(state_, target, responsibleId, std::time(nullptr));
    state_ = target;
}

void ServiceOrder::attachBudget(const Budget& budget) {
    if (state_ != OrderStatus::PendingBudget) {
        throw std::logic_error("ServiceOrder::attachBudget: OS fora de PendingBudget");
    }
    budget_ = budget;
}

}  // namespace mecanica360

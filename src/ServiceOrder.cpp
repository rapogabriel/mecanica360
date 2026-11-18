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

void ServiceOrder::serialize(std::ofstream& out) const {
    using namespace serialization_utils;

    writePrimitive(out, id_);
    writePrimitive(out, clientId_);
    writePrimitive(out, mechanicId_);

    vehicle_.serialize(out);

    writeString(out, diagnosis_);

    // Serializando vetor de peças
    size_t partsCount = parts_.size();
    writePrimitive(out, partsCount);
    for (const auto& part : parts_) {
        part.serialize(out);
    }

    writePrimitive(out, labor_);
    writePrimitive(out, deadline_);
    writePrimitive(out, state_);

    // Serializando std::optional<Budget>
    bool hasBudget = budget_.has_value();
    writePrimitive(out, hasBudget);
    if (hasBudget) {
        budget_->serialize(out);
    }

    // Serializando vetor de histórico
    size_t historyCount = history_.size();
    writePrimitive(out, historyCount);
    for (const auto& hist : history_) {
        hist.serialize(out);
    }
}

void ServiceOrder::deserialize(std::ifstream& in) {
    using namespace serialization_utils;

    readPrimitive(in, id_);
    readPrimitive(in, clientId_);
    readPrimitive(in, mechanicId_);

    vehicle_.deserialize(in);

    readString(in, diagnosis_);

    // Desserializando vetor de peças
    size_t partsCount;
    readPrimitive(in, partsCount);
    parts_.resize(partsCount);
    for (size_t i = 0; i < partsCount; ++i) {
        parts_[i].deserialize(in);
    }

    readPrimitive(in, labor_);
    readPrimitive(in, deadline_);
    readPrimitive(in, state_);

    // Desserializando std::optional<Budget>
    bool hasBudget;
    readPrimitive(in, hasBudget);
    if (hasBudget) {
        Budget b;
        b.deserialize(in);
        budget_ = b;
    } else {
        budget_ = std::nullopt;
    }

    // Desserializando vetor de histórico
    size_t historyCount;
    readPrimitive(in, historyCount);
    history_.clear();
    for (size_t i = 0; i < historyCount; ++i) {
        StatusHistory hist;
        hist.deserialize(in);
        history_.push_back(hist);
    }
}

}  // namespace mecanica360

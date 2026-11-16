#include "ServiceOrderService.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

namespace mecanica360 {

namespace {

bool isBlank(const std::string& s) {
    return std::all_of(s.begin(), s.end(),
                       [](unsigned char c) { return std::isspace(c) != 0; });
}

bool nonNegative(double v) { return std::isfinite(v) && v >= 0.0; }

/** Cliente > 0, placa e diagnostico informados, >= 1 peca valida, mao de obra >= 0, prazo > 0. */
bool validOpening(const OpeningData& d) {
    if (d.clientId <= 0) return false;
    if (isBlank(d.vehicle.plate())) return false;
    if (isBlank(d.diagnosis)) return false;
    if (d.parts.empty()) return false;
    for (const auto& p : d.parts) {
        if (isBlank(p.description()) || p.quantity() <= 0) return false;
    }
    return nonNegative(d.labor) && d.deadline > 0;
}

/** Valores nao negativos e prazo positivo. */
bool validBudget(const Budget& b) {
    return nonNegative(b.partsValue()) && nonNegative(b.laborValue()) && b.deadline() > 0;
}

/** Visibilidade individual de uma OS por perfil. */
bool canSee(const User& user, const ServiceOrder& so) {
    switch (user.role()) {
        case Role::Manager:
            return true;
        case Role::Customer:
            return so.clientId() == user.id() &&
                   so.state() != OrderStatus::RejectedLackOfStock;
        case Role::Mechanic:
            return so.mechanicId() == user.id();
        case Role::Stockroom:
            return so.state() == OrderStatus::PendingBudget;
    }
    return false;
}

}  // namespace

// #13 ------------------------------------------------------------------------

ServiceOrderService::ServiceOrderService(ServiceOrderRepository& repository)
    : repository_(repository) {}

// #16 CU-01 ------------------------------------------------------------------

std::expected<int, OrderError> ServiceOrderService::openSO(const User& user,
                                                           const OpeningData& data) {
    if (user.role() != Role::Mechanic) return std::unexpected(OrderError::AcessDenied);
    if (!validOpening(data)) return std::unexpected(OrderError::InvalidData);
    return repository_.insert(ServiceOrder::create(data, user.id()));
}

// #18 CU-02 ------------------------------------------------------------------

std::expected<std::vector<ServiceOrder>, OrderError> ServiceOrderService::searchNewSO(
    const User& user) {
    if (user.role() != Role::Stockroom) return std::unexpected(OrderError::AcessDenied);
    return repository_.listByState(OrderStatus::PendingBudget);
}

std::expected<void, OrderError> ServiceOrderService::sendBudget(const User& user, int soId,
                                                                const Budget& budget) {
    if (user.role() != Role::Stockroom) return std::unexpected(OrderError::AcessDenied);
    if (!validBudget(budget)) return std::unexpected(OrderError::InvalidData);

    auto found = repository_.findById(soId);
    if (!found) return std::unexpected(OrderError::ServiceOrderNotFound);

    ServiceOrder order = *found;  // trabalha numa copia: erros nao alteram o repositorio
    if (!order.canTransition(OrderStatus::PendingAcceptance)) {
        return std::unexpected(OrderError::InvalidTransition);
    }
    order.attachBudget(budget);
    order.transition(OrderStatus::PendingAcceptance, user.id());
    repository_.update(order);
    return {};
}

// #20 CU-03 ------------------------------------------------------------------

std::expected<void, OrderError> ServiceOrderService::rejectDueLackOfStock(const User& user,
                                                                           int soId) {
    if (user.role() != Role::Stockroom) return std::unexpected(OrderError::AcessDenied);

    auto found = repository_.findById(soId);
    if (!found) return std::unexpected(OrderError::ServiceOrderNotFound);

    ServiceOrder order = *found;
    if (!order.canTransition(OrderStatus::RejectedLackOfStock)) {
        return std::unexpected(OrderError::InvalidTransition);
    }
    order.transition(OrderStatus::RejectedLackOfStock, user.id());
    repository_.update(order);
    return {};
}

// #23 CU-04 ------------------------------------------------------------------

std::expected<ServiceOrder, OrderError> ServiceOrderService::checkSOById(const User& user,
                                                                         int soId) {
    auto found = repository_.findById(soId);
    if (!found) return std::unexpected(OrderError::ServiceOrderNotFound);
    if (!canSee(user, *found)) return std::unexpected(OrderError::AcessDenied);
    return *found;
}

std::expected<ServiceOrder, OrderError> ServiceOrderService::checkSO(const User& user,
                                                                     int soId) {
    return checkSOById(user, soId);
}

std::expected<std::vector<ServiceOrder>, OrderError> ServiceOrderService::checkSO(
    const User& user) {
    switch (user.role()) {
        case Role::Manager:
            return repository_.listAll();
        case Role::Customer: {
            auto orders = repository_.listByCustomer(user.id());
            std::erase_if(orders, [](const ServiceOrder& so) {
                return so.state() == OrderStatus::RejectedLackOfStock;
            });
            return orders;
        }
        case Role::Mechanic:
            return repository_.listByMechanic(user.id());
        case Role::Stockroom:
            break;
    }
    return std::unexpected(OrderError::AcessDenied);
}

std::expected<std::vector<ServiceOrder>, OrderError>
ServiceOrderService::listRejectedDueLackOfStock(const User& user) {
    if (user.role() != Role::Manager) return std::unexpected(OrderError::AcessDenied);
    return repository_.listByState(OrderStatus::RejectedLackOfStock);
}

}  // namespace mecanica360

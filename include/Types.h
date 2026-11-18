#pragma once

namespace mecanica360 {

// Papeis dos usuarios do sistema.
enum class Role {
    Customer,
    Mechanic,
    Stockroom,
    Manager
};

// Estados possiveis de uma ordem de servico na Sprint 2.
enum class OrderStatus {
    PendingBudget,
    PendingAcceptance,
    RejectedLackOfStock
};

// Erros de negocio 
enum class OrderError {
    AcessDenied,
    InvalidData,
    ServiceOrderNotFound,
    InvalidTransition
};

}  // namespace mecanica360

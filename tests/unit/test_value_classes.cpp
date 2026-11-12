// Tarefa #4: classes de valor, User e Budget::total.
#include "Fixtures.h"
#include "TestFramework.h"

using namespace mecanica360;

TEST("Budget::total soma pecas e mao de obra") {
    Budget b(800.0, 300.0, 3);
    CHECK_NEAR(b.total(), 1100.0);
    CHECK_NEAR(b.partsValue(), 800.0);
    CHECK_NEAR(b.laborValue(), 300.0);
    CHECK(b.deadline() == 3);
}

TEST("Budget::total com valores zero e decimais") {
    CHECK_NEAR(Budget(0.0, 0.0, 1).total(), 0.0);
    CHECK_NEAR(Budget(1e6, 0.5, 2).total(), 1000000.5);
    CHECK_NEAR(Budget(10.5, 0.25, 1).total(), 10.75);
}

TEST("Vehicle guarda placa, modelo e ano") {
    Vehicle v("ABC1D23", "Gol", 2015);
    CHECK(v.plate() == "ABC1D23");
    CHECK(v.model() == "Gol");
    CHECK(v.year() == 2015);
    Vehicle vazio;
    CHECK(vazio.plate().empty());
    CHECK(vazio.year() == 0);
}

TEST("NecessaryPart guarda descricao e quantidade") {
    NecessaryPart p("pastilha", 4);
    CHECK(p.description() == "pastilha");
    CHECK(p.quantity() == 4);
}

TEST("StatusHistory guarda estados, responsavel e instante") {
    StatusHistory h(OrderStatus::PendingBudget, OrderStatus::PendingAcceptance, 5, 1234);
    CHECK(h.previousState() == OrderStatus::PendingBudget);
    CHECK(h.newState() == OrderStatus::PendingAcceptance);
    CHECK(h.responsibleId() == 5);
    CHECK(h.instant() == 1234);
}

TEST("User expoe id, nome e perfil") {
    User u(7, "Rayssa", Role::Mechanic);
    CHECK(u.id() == 7);
    CHECK(u.name() == "Rayssa");
    CHECK(u.role() == Role::Mechanic);
}

TEST("OpeningData agrega os dados de abertura") {
    OpeningData d;
    d.clientId = 10;
    d.vehicle = Vehicle("ABC1D23", "Gol", 2015);
    d.diagnosis = "freios";
    d.parts.push_back(NecessaryPart("pastilha", 4));
    d.labor = 300.0;
    d.deadline = 3;
    CHECK(d.clientId == 10);
    CHECK(d.parts.size() == 1);
    CHECK(d.vehicle.model() == "Gol");
}

TEST("Enums de estado e erro possuem os valores do UML") {
    CHECK(OrderStatus::PendingBudget != OrderStatus::PendingAcceptance);
    CHECK(OrderStatus::PendingAcceptance != OrderStatus::RejectedLackOfStock);
    CHECK(OrderError::AcessDenied != OrderError::InvalidData);
    CHECK(OrderError::ServiceOrderNotFound != OrderError::InvalidTransition);
}

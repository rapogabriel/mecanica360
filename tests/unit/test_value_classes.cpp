// Tarefa #4: classes de valor, User e Budget::total.
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Fixtures.h"

using namespace mecanica360;

TEST_CASE("Budget::total soma pecas e mao de obra", "[unit][value_classes]") {
    Budget b(800.0, 300.0, 3);
    CHECK(b.total() == Catch::Approx(1100.0));
    CHECK(b.partsValue() == Catch::Approx(800.0));
    CHECK(b.laborValue() == Catch::Approx(300.0));
    CHECK(b.deadline() == 3);
}

TEST_CASE("Budget::total com valores zero e decimais", "[unit][value_classes]") {
    CHECK(Budget(0.0, 0.0, 1).total() == Catch::Approx(0.0));
    CHECK(Budget(1e6, 0.5, 2).total() == Catch::Approx(1000000.5));
    CHECK(Budget(10.5, 0.25, 1).total() == Catch::Approx(10.75));
}

TEST_CASE("Vehicle guarda placa, modelo e ano", "[unit][value_classes]") {
    Vehicle v("ABC1D23", "Gol", 2015);
    CHECK(v.plate() == "ABC1D23");
    CHECK(v.model() == "Gol");
    CHECK(v.year() == 2015);
    Vehicle vazio;
    CHECK(vazio.plate().empty());
    CHECK(vazio.year() == 0);
}

TEST_CASE("NecessaryPart guarda descricao e quantidade", "[unit][value_classes]") {
    NecessaryPart p("pastilha", 4);
    CHECK(p.description() == "pastilha");
    CHECK(p.quantity() == 4);
}

TEST_CASE("StatusHistory guarda estados, responsavel e instante", "[unit][value_classes]") {
    StatusHistory h(OrderStatus::PendingBudget, OrderStatus::PendingAcceptance, 5, 1234);
    CHECK(h.previousState() == OrderStatus::PendingBudget);
    CHECK(h.newState() == OrderStatus::PendingAcceptance);
    CHECK(h.responsibleId() == 5);
    CHECK(h.instant() == 1234);
}

TEST_CASE("User expoe id, nome e perfil", "[unit][value_classes]") {
    User u(7, "Rayssa", Role::Mechanic);
    CHECK(u.id() == 7);
    CHECK(u.name() == "Rayssa");
    CHECK(u.role() == Role::Mechanic);
}

TEST_CASE("OpeningData agrega os dados de abertura", "[unit][value_classes]") {
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

TEST_CASE("Enums de estado e erro possuem os valores do UML", "[unit][value_classes]") {
    CHECK(OrderStatus::PendingBudget != OrderStatus::PendingAcceptance);
    CHECK(OrderStatus::PendingAcceptance != OrderStatus::RejectedLackOfStock);
    CHECK(OrderError::AcessDenied != OrderError::InvalidData);
    CHECK(OrderError::ServiceOrderNotFound != OrderError::InvalidTransition);
}
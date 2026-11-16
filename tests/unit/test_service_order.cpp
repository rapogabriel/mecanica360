// Tarefa #6: ServiceOrder (criacao, transicoes, historico e orcamento).
#include <ctime>
#include <stdexcept>

#include "Fixtures.h"
#include "TestFramework.h"

using namespace fx;

TEST("ServiceOrder::create inicia em PendingBudget, sem orcamento, sem historico e sem id") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    CHECK(so.id() == 0);
    CHECK(so.clientId() == 10);
    CHECK(so.mechanicId() == 7);
    CHECK(so.vehicle().plate() == "ABC1D23");
    CHECK(so.vehicle().model() == "Gol");
    CHECK(so.diagnosis() == "freios desgastados");
    REQUIRE(so.parts().size() == 1);
    CHECK(so.parts()[0].description() == "pastilha de freio");
    CHECK(so.parts()[0].quantity() == 4);
    CHECK_NEAR(so.labor(), 300.0);
    CHECK(so.deadline() == 3);
    CHECK(so.state() == OrderStatus::PendingBudget);
    CHECK(!so.budget().has_value());
    CHECK(so.history().empty());
}

TEST("ServiceOrder::assignId define o identificador") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    so.assignId(42);
    CHECK(so.id() == 42);
}

TEST("ServiceOrder::canTransition so admite PendingBudget -> PendingAcceptance/Rejected") {
    const OrderStatus all[] = {OrderStatus::PendingBudget, OrderStatus::PendingAcceptance,
                               OrderStatus::RejectedLackOfStock};
    for (OrderStatus from : all) {
        ServiceOrder so = ServiceOrder::create(validOpening(), 7);
        if (from != OrderStatus::PendingBudget) so.transition(from, 5);
        for (OrderStatus to : all) {
            const bool expected =
                from == OrderStatus::PendingBudget && to != OrderStatus::PendingBudget;
            CHECK(so.canTransition(to) == expected);
        }
    }
}

TEST("ServiceOrder::transition registra historico com responsavel e instante") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    const std::time_t before = std::time(nullptr);
    so.transition(OrderStatus::PendingAcceptance, 5);
    const std::time_t after = std::time(nullptr);
    CHECK(so.state() == OrderStatus::PendingAcceptance);
    REQUIRE(so.history().size() == 1);
    const auto& h = so.history()[0];
    CHECK(h.previousState() == OrderStatus::PendingBudget);
    CHECK(h.newState() == OrderStatus::PendingAcceptance);
    CHECK(h.responsibleId() == 5);
    CHECK(h.instant() >= before && h.instant() <= after);
}

TEST("ServiceOrder::transition para RejectedLackOfStock mantem OS sem orcamento") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    so.transition(OrderStatus::RejectedLackOfStock, 5);
    CHECK(so.state() == OrderStatus::RejectedLackOfStock);
    CHECK(so.history().size() == 1);
    CHECK(!so.budget().has_value());
}

TEST("ServiceOrder::transition invalida lanca logic_error e nao altera a OS") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    so.attachBudget(Budget(800.0, 300.0, 3));
    so.transition(OrderStatus::PendingAcceptance, 5);
    const auto before = snap(so);

    CHECK_THROWS(so.transition(OrderStatus::RejectedLackOfStock, 5), std::logic_error);
    CHECK_THROWS(so.transition(OrderStatus::PendingBudget, 5), std::logic_error);
    CHECK_THROWS(so.transition(OrderStatus::PendingAcceptance, 5), std::logic_error);
    CHECK(snap(so) == before);
}

TEST("ServiceOrder::attachBudget anexa o orcamento e budget() o devolve") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    CHECK(!so.budget().has_value());
    so.attachBudget(Budget(800.0, 300.0, 3));
    REQUIRE(so.budget().has_value());
    CHECK_NEAR(so.budget()->total(), 1100.0);
    CHECK(so.budget()->deadline() == 3);
}

TEST("ServiceOrder::attachBudget fora de PendingBudget lanca logic_error e nao anexa") {
    ServiceOrder so = ServiceOrder::create(validOpening(), 7);
    so.transition(OrderStatus::RejectedLackOfStock, 5);
    CHECK_THROWS(so.attachBudget(Budget(1.0, 1.0, 1)), std::logic_error);
    CHECK(!so.budget().has_value());
}

TEST("ServiceOrder: copia e independente da original") {
    ServiceOrder a = ServiceOrder::create(validOpening(), 7);
    ServiceOrder b = a;
    b.transition(OrderStatus::PendingAcceptance, 5);
    CHECK(a.state() == OrderStatus::PendingBudget);
    CHECK(a.history().empty());
}

#pragma once

#include <ctime>
#include <string>

#include "Types.h"

namespace mecanica360 {

// Classes de valor e User (apenas declaracoes).

class User {
public:
    User(int id, std::string name, Role role);
    int id() const;
    const std::string& name() const;
    Role role() const;

private:
    int id_;
    std::string name_;
    Role role_;
};

class Vehicle {
public:
    Vehicle();  // necessario: OpeningData e criado vazio nos testes
    Vehicle(std::string plate, std::string model, int year);
    const std::string& plate() const;
    const std::string& model() const;
    int year() const;

private:
    std::string plate_;
    std::string model_;
    int year_;
};

class NecessaryPart {
public:
    NecessaryPart(std::string description, int quantity);
    const std::string& description() const;
    int quantity() const;

private:
    std::string description_;
    int quantity_;
};

class Budget {
public:
    Budget(double partsValue, double laborValue, int deadline);
    double partsValue() const;
    double laborValue() const;
    int deadline() const;
    double total() const;  // pecas + mao de obra

private:
    double partsValue_;
    double laborValue_;
    int deadline_;
};

class StatusHistory {
public:
    StatusHistory(OrderStatus previousState, OrderStatus newState,
                  int responsibleId, std::time_t instant);
    OrderStatus previousState() const;
    OrderStatus newState() const;
    int responsibleId() const;
    std::time_t instant() const;

private:
    OrderStatus previousState_;
    OrderStatus newState_;
    int responsibleId_;
    std::time_t instant_;
};

}  // namespace mecanica360
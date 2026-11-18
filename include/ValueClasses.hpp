#pragma once

#include <ctime>
#include <string>
#include "Types.h"

#include "Serializable.hpp"

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

class Vehicle: public ISerializable{
public:
    Vehicle() = default;  // necessario: OpeningData e criado vazio nos testes
    Vehicle(std::string plate, std::string model, int year);
    const std::string& plate() const;
    const std::string& model() const;
    int year() const;

    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;

private:
    std::string plate_;
    std::string model_;
    int year_;
};

class NecessaryPart : public ISerializable{
public:
    NecessaryPart() = default;
    NecessaryPart(std::string description, int quantity);
    const std::string& description() const;
    int quantity() const;

    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;

private:
    std::string description_;
    int quantity_;
};

class Budget : public ISerializable{
public:
    Budget() = default;
    Budget(double partsValue, double laborValue, int deadline);
    double partsValue() const;
    double laborValue() const;
    int deadline() const;
    double total() const;  // pecas + mao de obra

    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;

private:
    double partsValue_;
    double laborValue_;
    int deadline_;
};

class StatusHistory : public ISerializable{
public:
    StatusHistory() = default;
    StatusHistory(OrderStatus previousState, OrderStatus newState,
                  int responsibleId, std::time_t instant);
    OrderStatus previousState() const;
    OrderStatus newState() const;
    int responsibleId() const;
    std::time_t instant() const;

    void serialize(std::ofstream& out) const override;
    void deserialize(std::ifstream& in) override;

private:
    OrderStatus previousState_;
    OrderStatus newState_;
    int responsibleId_;
    std::time_t instant_;
};

}  // namespace mecanica360
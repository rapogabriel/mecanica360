#include "ValueClasses.h"

#include <utility>

namespace mecanica360 {

// ---- User ------------------------------------------------------------------

User::User(int id, std::string name, Role role)
    : id_(id), name_(std::move(name)), role_(role) {}

int User::id() const { return id_; }
const std::string& User::name() const { return name_; }
Role User::role() const { return role_; }

// ---- Vehicle ---------------------------------------------------------------

Vehicle::Vehicle() : plate_(), model_(), year_(0) {}

Vehicle::Vehicle(std::string plate, std::string model, int year)
    : plate_(std::move(plate)), model_(std::move(model)), year_(year) {}

const std::string& Vehicle::plate() const { return plate_; }
const std::string& Vehicle::model() const { return model_; }
int Vehicle::year() const { return year_; }

// ---- NecessaryPart ---------------------------------------------------------

NecessaryPart::NecessaryPart(std::string description, int quantity)
    : description_(std::move(description)), quantity_(quantity) {}

const std::string& NecessaryPart::description() const { return description_; }
int NecessaryPart::quantity() const { return quantity_; }

// ---- Budget ----------------------------------------------------------------

Budget::Budget(double partsValue, double laborValue, int deadline)
    : partsValue_(partsValue), laborValue_(laborValue), deadline_(deadline) {}

double Budget::partsValue() const { return partsValue_; }
double Budget::laborValue() const { return laborValue_; }
int Budget::deadline() const { return deadline_; }
double Budget::total() const { return partsValue_ + laborValue_; }

// ---- StatusHistory ---------------------------------------------------------

StatusHistory::StatusHistory(OrderStatus previousState, OrderStatus newState,
                             int responsibleId, std::time_t instant)
    : previousState_(previousState),
      newState_(newState),
      responsibleId_(responsibleId),
      instant_(instant) {}

OrderStatus StatusHistory::previousState() const { return previousState_; }
OrderStatus StatusHistory::newState() const { return newState_; }
int StatusHistory::responsibleId() const { return responsibleId_; }
std::time_t StatusHistory::instant() const { return instant_; }

}  // namespace mecanica360

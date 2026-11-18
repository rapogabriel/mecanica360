#include "ValueClasses.hpp"

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

void Vehicle::serialize(std::ofstream& out) const {
    using namespace serialization_utils;
    writeString(out, plate_);
    writeString(out, model_);
    writePrimitive(out, year_);
}

void Vehicle::deserialize(std::ifstream& in) {
    using namespace serialization_utils;
    readString(in, plate_);
    readString(in, model_);
    readPrimitive(in, year_);
}

// ---- NecessaryPart ---------------------------------------------------------

NecessaryPart::NecessaryPart() : description_(), quantity_(0) {}
NecessaryPart::NecessaryPart(std::string description, int quantity)
    : description_(std::move(description)), quantity_(quantity) {}

const std::string& NecessaryPart::description() const { return description_; }
int NecessaryPart::quantity() const { return quantity_; }

void NecessaryPart::serialize(std::ofstream& out) const {
    using namespace serialization_utils;
    writeString(out, description_);
    writePrimitive(out, quantity_);
}

void NecessaryPart::deserialize(std::ifstream& in) {
    using namespace serialization_utils;
    readString(in, description_);
    readPrimitive(in, quantity_);
}

// ---- Budget ----------------------------------------------------------------
Budget::Budget() : partsValue_(0.0), laborValue_(0.0), deadline_(0) {}
Budget::Budget(double partsValue, double laborValue, int deadline)
    : partsValue_(partsValue), laborValue_(laborValue), deadline_(deadline) {}

double Budget::partsValue() const { return partsValue_; }
double Budget::laborValue() const { return laborValue_; }
int Budget::deadline() const { return deadline_; }
double Budget::total() const { return partsValue_ + laborValue_; }

void Budget::serialize(std::ofstream& out) const {
    using namespace serialization_utils;
    writePrimitive(out, partsValue_);
    writePrimitive(out, laborValue_);
    writePrimitive(out, deadline_);
}

void Budget::deserialize(std::ifstream& in) {
    using namespace serialization_utils;
    readPrimitive(in, partsValue_);
    readPrimitive(in, laborValue_);
    readPrimitive(in, deadline_);
}

// ---- StatusHistory ---------------------------------------------------------

StatusHistory::StatusHistory()
    : previousState_(OrderStatus::PendingBudget),
      newState_(OrderStatus::PendingBudget),
      responsibleId_(0),
      instant_(0) {}


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



void StatusHistory::serialize(std::ofstream& out) const {
    using namespace serialization_utils;
    writePrimitive(out, previousState_);
    writePrimitive(out, newState_);
    writePrimitive(out, responsibleId_);
    writePrimitive(out, instant_);
}

void StatusHistory::deserialize(std::ifstream& in) {
    using namespace serialization_utils;
    readPrimitive(in, previousState_);
    readPrimitive(in, newState_);
    readPrimitive(in, responsibleId_);
    readPrimitive(in, instant_);
}

}  // namespace mecanica360

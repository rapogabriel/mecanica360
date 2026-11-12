#include "FakeRepository.h"
#include <algorithm>

namespace mecanica360{

    int FakeRepository::insert(ServiceOrder order){
        nextId_++;
        orders_.push_back(order);
        return nextId_;
    }

    void FakeRepository::update(const ServiceOrder &order){
        auto it = std::find_if(orders_.begin(),orders_.end(),[&order](const ServiceOrder o){ return o.getId() == order.getId();});
          
        if(it != orders_.end()){
            *it = order;     
        }   
    }

    std::optional<ServiceOrder> FakeRepository::findById(int id) const {
        auto it = std::find_if(orders_.begin(), orders_.end(), [id](const ServiceOrder& o) {
            return o.getId() == id;
        });

        if (it != orders_.end()) {
            return *it;
        }
    
    return std::nullopt;
    }

    std::vector<ServiceOrder>FakeRepository::listByState(OrderStatus status) const{
        std::vector<ServiceOrder> result;
            for (const auto& order : orders_) {
            // Ajuste para o método que retorna o status atual da OS (ex: order.getStatus())
            if (order.getStatus() == status) {
                result.push_back(order);
            }
        }
        return result;
    }

    std::vector<ServiceOrder>FakeRepository::listByCustomer(int clienteId) const{
        std::vector<ServiceOrder> result;
            for (const auto& order : orders_) {
            // Ajuste para o método que retorna o status atual da OS (ex: order.getStatus())
            if (order.getClientId() == clienteId) {
                result.push_back(order);
            }
        }
        return result;
    }

    std::vector<ServiceOrder>FakeRepository::listByMechanic(int mechanicId) const{
        std::vector<ServiceOrder> result;
            for (const auto& order : orders_) {
            // Ajuste para o método que retorna o status atual da OS (ex: order.getStatus())
            if (order.getMechanicId() == mechanicId) {
                result.push_back(order);
            }
        }
        return result;
    }

    std::vector<ServiceOrder>FakeRepository::listAll() const{
        return orders_;
    }

}
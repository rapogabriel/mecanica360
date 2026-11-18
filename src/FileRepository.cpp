#include "FileRepository.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <functional>
namespace mecanica360 {

    
    static size_t batchSize =20;
    

    FileRepository::FileRepository(const std::string& dataPath, const std::string& indexPath)
        : dataPath_(dataPath), indexPath_(indexPath) {}

    int FileRepository::creatNextId() const {
        std::ifstream indexFile(indexPath_, std::ios::binary | std::ios::ate);
        if (!indexFile.is_open()) {
            return 1;
        
        }
        return (indexFile.tellg()/(sizeof(int)+ sizeof(long long)))+1;

    }

    int FileRepository::insert(ServiceOrder order) {
        int newId = creatNextId();
        order.assignId(newId);

        // 1. Escreve a ordem serializada no final do arquivo de dados (.dat)
        std::ofstream dataFile(dataPath_, std::ios::binary | std::ios::app);
        if (!dataFile.is_open()) 
            return -1;

        dataFile.seekp(0, std::ios::end);
        long long offset = dataFile.tellp();

        order.serialize(dataFile);
        dataFile.close();

        std::ofstream indexFile(indexPath_, std::ios::binary | std::ios::app);
        if (indexFile.is_open()) {
            indexFile.write(reinterpret_cast<const char*>(&newId), sizeof(newId));
            indexFile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
            indexFile.close();
        }

        return newId;
    }

    void FileRepository::update(const ServiceOrder& order) {
        // Na estratégia Append-Only, atualizar significa anexar a nova versão 
        // e adicionar um novo registro no índice apontando para o novo offset.
        std::ofstream dataFile(dataPath_, std::ios::binary | std::ios::app);
        if (!dataFile.is_open()) 
            return;

        dataFile.seekp(0, std::ios::end);
        long long offset = dataFile.tellp();
        order.serialize(dataFile);
        dataFile.close();

        int id = order.id();
        std::fstream indexFile(indexPath_,std::ios::in | std::ios::out | std::ios::binary);
        if (indexFile.is_open()) {
            int idTemp;
            long long offSetGetted;
            long long currentLocation = 0;
            while (indexFile.read(reinterpret_cast<char*>(&idTemp), sizeof(idTemp)) &&
              indexFile.read(reinterpret_cast<char*>(&offSetGetted), sizeof(offSetGetted))){
                if(order.id() == idTemp){
                    indexFile.seekp(-static_cast<std::streamoff>(sizeof(long long)), std::ios::cur);
                    indexFile.write(reinterpret_cast<const char*>(&offset), sizeof(long long));
                    break;
                }
                currentLocation = indexFile.tellg();
            }

            indexFile.close();
        }
    }

    
    std::optional<ServiceOrder> FileRepository::findById(int id) const {
        std::ifstream indexFile(indexPath_, std::ios::binary| std::ios::ate);
        if (!indexFile.is_open()) 
            return std::nullopt;


        if(id*sizeof(IndexTuple) > indexFile.tellg())
            return std::nullopt;

        int currentId;
        long long offset;
        IndexTuple tupla;
        indexFile.seekg((id-1)*sizeof(IndexTuple));
        indexFile.read(reinterpret_cast<char*>(&tupla),(sizeof(IndexTuple)));

        indexFile.close();

        std::ifstream dataFile(dataPath_, std::ios::binary);

        if (!dataFile.is_open()) 
            return std::nullopt;

        dataFile.seekg(tupla.offset);
        ServiceOrder order;
        order.deserialize(dataFile);

        return order;
    }

    std::vector<ServiceOrder> FileRepository::listAll() const {
        std::ifstream indexFile(indexPath_, std::ios::binary);
        if (!indexFile.is_open()) 
            return {};

        std::vector<ServiceOrder> results;
        std::streampos currentPos = 0;
        BatchResult batch;
        while(true){

            batch = readBatch(currentPos,20);
            results.insert(results.end(),batch.orders.begin(),batch.orders.end());

            if (batch.reachEnd) break;
            currentPos = batch.nextIdPos;
            
        }
        return results;
        
    }

    std::vector<ServiceOrder> FileRepository::findBy(std::function<bool(const ServiceOrder&)> predicate, size_t batchSize) const {
            std::vector<ServiceOrder> results;
            std::ifstream indexFile(indexPath_, std::ios::binary);
            if (!indexFile.is_open()) return results;

            std::streampos currentPos = 0;

            while (true) {
                // Usa o tamanho do batch escolhido pelo usuário
                BatchResult batch = readBatch(currentPos, batchSize);
                
                for (const auto& order : batch.orders) {
                    if (predicate(order)) {
                        results.push_back(order);
                    }
                }

                if (batch.reachEnd) break;
                currentPos = batch.nextIdPos;
            }

            return results;
        }

    std::vector<ServiceOrder> FileRepository::listByCustomer(int clientId) const {
        return findBy([clientId](const ServiceOrder& order) {
            return order.clientId() == clientId;
        }, batchSize);
    }

    std::vector<ServiceOrder> FileRepository::listByMechanic(int mechanicId) const {
        return findBy([mechanicId](const ServiceOrder& order) {
            return order.mechanicId() == mechanicId;
        }, batchSize);
    }

    std::vector<ServiceOrder> FileRepository::listByState(OrderStatus status) const {
        return findBy([status](const ServiceOrder& order) {
            return order.state() == status;
        }, batchSize);
    }
    
    FileRepository::BatchResult FileRepository::readBatch(std::streampos startOffset,size_t batchSize) const{
        BatchResult result;
        result.nextIdPos = startOffset;
        result.reachEnd = true;


        std::vector<IndexTuple> offsets(batchSize);

        
        std::ifstream indexFile(indexPath_,std::ios::binary);


        if(!indexFile.is_open()){
            result.reachEnd = true;
            return result;
        }
        
        indexFile.seekg(startOffset);
        
        indexFile.read(reinterpret_cast<char*>(offsets.data()),batchSize*sizeof(IndexTuple));
        std::streamsize bytesRead = indexFile.gcount();
        size_t quantityRead = bytesRead/sizeof(IndexTuple);
        result.nextIdPos = indexFile.tellg();
        indexFile.close();


        result.reachEnd = (quantityRead < batchSize);

        std::ifstream dataFile(dataPath_,std::ios::binary);

        if(!dataFile.is_open()){
            result.reachEnd = true;
            return result;
        }

        result.orders.resize(quantityRead);

        for(int i=0;i<quantityRead;i++){
            dataFile.seekg(offsets.at(i).offset,std::ios::beg);
            result.orders.at(i).deserialize(dataFile);

        }
        

        return result;

    }



    //metodo opara pergar x quantidade de SOs do arquivo


}



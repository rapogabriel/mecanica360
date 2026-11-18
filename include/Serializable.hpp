#pragma once

#include <fstream>
#include <string>

namespace mecanica360 {

// 1. INTERFACE (Contrato) que obriga as classes a implementarem serialização
class ISerializable {
public:
    virtual ~ISerializable() = default;
    
    virtual void serialize(std::ofstream& out) const = 0;
    virtual void deserialize(std::ifstream& in) = 0;
};

// 2. NAMESPACE COM AS FUNÇÕES UTILITÁRIAS TEMPLATE
namespace serialization_utils {

    // Escreve qualquer tipo primitivo (int, double, enums, etc.)
    template <typename T>
    inline void writePrimitive(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    // Lê qualquer tipo primitivo
    template <typename T>
    inline void readPrimitive(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    // Escreve string de tamanho variável ([Tamanho] + [Dados])
    inline void writeString(std::ofstream& out, const std::string& str) {
        size_t size = str.size();
        writePrimitive(out, size);
        out.write(str.data(), size);
    }

    // Lê string de tamanho variável
    inline void readString(std::ifstream& in, std::string& str) {
        size_t size;
        readPrimitive(in, size);
        str.resize(size);
        in.read(&str[0], size);
    }

} // namespace serialization_utils

} // namespace mecanica360
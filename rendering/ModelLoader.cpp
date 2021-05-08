#include "ModelLoader.hpp"

#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

std::vector<Vertex> loadObj(const std::string& filePath) {
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec2> vertexTexcoords;
    std::vector<glm::vec3> vertexNormals;

    std::vector<Vertex> vertices;

    std::ifstream file{filePath};
    if (!file.is_open())
        throw std::runtime_error("Couldn't open the file");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss{line};

        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertexPositions.push_back(vertex);
        } else if (prefix == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            vertexTexcoords.push_back(texCoord);
        } else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            vertexNormals.push_back(normal);
        } else if (prefix == "f") {
            break;
        }
    }

    vertices.reserve(vertexPositions.size() * 2);

    while (std::getline(file, line)) {
        std::istringstream ss{line};

        std::string prefix;
        ss >> prefix;

        for (uint8_t i = 0; i < 3; ++i) {
            uint32_t positionIndex, texCoordIndex, normalIndex;
            ss >> positionIndex; ss.ignore();
            ss >> texCoordIndex; ss.ignore();
            ss >> normalIndex;

            vertices.emplace_back(
                vertexPositions[positionIndex - 1],
                vertexTexcoords[texCoordIndex - 1],
                vertexNormals[normalIndex - 1]
            );
        }
    }

    // make sure it's not copied
    return std::move(vertices);
}

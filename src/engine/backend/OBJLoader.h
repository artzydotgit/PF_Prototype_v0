#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace tinyobj {
    struct attrib_t;
    struct shape_t;
    struct material_t;
}

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    
    Vertex() : position(0.0f), normal(0.0f), texCoord(0.0f), tangent(0.0f) {}
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex) 
        : position(pos), normal(norm), texCoord(tex), tangent(0.0f) {}
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, const glm::vec3& tan) 
        : position(pos), normal(norm), texCoord(tex), tangent(tan) {}
};

struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float transparency;
    float refractiveIndex;
    std::string diffuseTex;
    std::string normalTex;
    std::string specularTex;
    
    Material() : ambient(0.2f), diffuse(0.8f), specular(1.0f), 
                 shininess(32.0f), transparency(1.0f), refractiveIndex(1.0f) {}
};

struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int materialIndex;
    
    Mesh() : materialIndex(-1) {}
    
    void CalculateBounds(glm::vec3& min, glm::vec3& max) const;
    void CalculateNormals();
    void CalculateTangents();
};

struct Model {
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    
    Model() : boundsMin(0.0f), boundsMax(0.0f) {}
    
    void CalculateBounds();
    size_t GetTotalVertexCount() const;
    size_t GetTotalIndexCount() const;
};

class OBJLoader {
public:
    OBJLoader();
    ~OBJLoader();
    
    bool LoadModel(const std::string& filename, Model& model);
    bool LoadModel(const std::string& filename, Model& model, const Material& material);
    bool LoadModels(const std::vector<std::string>& filenames, std::vector<Model>& models);
    const std::string& GetLastError() const { return m_lastError; }
    void ClearError() { m_lastError.clear(); }
    
    void SetGenerateNormals(bool enable) { m_generateNormals = enable; }
    void SetGenerateTangents(bool enable) { m_generateTangents = enable; }
    void SetFlipUVs(bool enable) { m_flipUVs = enable; }
    
private:
    bool ProcessShapes(const std::vector<tinyobj::shape_t>& shapes,
                      const std::vector<tinyobj::material_t>& materials,
                      const tinyobj::attrib_t& attrib,
                      Model& model);
    
    bool ProcessShape(const tinyobj::shape_t& shape,
                     const std::vector<tinyobj::material_t>& materials,
                     const tinyobj::attrib_t& attrib,
                     Model& model);
    
    Material ConvertMaterial(const tinyobj::material_t& mat);
    
    glm::vec3 GetVertexPosition(const tinyobj::attrib_t& attrib, int index);
    glm::vec3 GetVertexNormal(const tinyobj::attrib_t& attrib, int index);
    glm::vec2 GetVertexTexCoord(const tinyobj::attrib_t& attrib, int index);
    glm::vec3 GetVertexTangent(const tinyobj::attrib_t& attrib, int index);
    
    void SetError(const std::string& error);
    
    bool m_generateNormals;
    bool m_generateTangents;
    bool m_flipUVs;
    
    std::string m_lastError;
};

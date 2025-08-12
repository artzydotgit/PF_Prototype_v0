#include "OBJLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../external/tiny_obj_loader.h"
#include <iostream>
#include <algorithm>
#include <glm/gtc/constants.hpp>

void Mesh::CalculateBounds(glm::vec3& min, glm::vec3& max) const {
    if (vertices.empty()) {
        min = max = glm::vec3(0.0f);
        return;
    }
    
    min = max = vertices[0].position;
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex.position);
        max = glm::max(max, vertex.position);
    }
}

void Mesh::CalculateNormals() {
    if (vertices.empty()) return;
    
    for (auto& vertex : vertices) {
        vertex.normal = glm::vec3(0.0f);
    }
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;
        
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) continue;
        
        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        
        vertices[i0].normal += normal;
        vertices[i1].normal += normal;
        vertices[i2].normal += normal;
    }
    
    for (auto& vertex : vertices) {
        if (glm::length(vertex.normal) > 0.0f) {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }
}

void Mesh::CalculateTangents() {
    if (vertices.empty()) return;
    
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
    }
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        if (i + 2 >= indices.size()) break;
        
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) continue;
        
        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        
        glm::vec2 t0 = vertices[i0].texCoord;
        glm::vec2 t1 = vertices[i1].texCoord;
        glm::vec2 t2 = vertices[i2].texCoord;
        
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec2 deltaUV1 = t1 - t0;
        glm::vec2 deltaUV2 = t2 - t0;
        
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);
        
        vertices[i0].tangent += tangent;
        vertices[i1].tangent += tangent;
        vertices[i2].tangent += tangent;
    }
    
    for (auto& vertex : vertices) {
        if (glm::length(vertex.tangent) > 0.0f) {
            vertex.tangent = glm::normalize(vertex.tangent);
        }
    }
}

void Model::CalculateBounds() {
    if (meshes.empty()) {
        boundsMin = boundsMax = glm::vec3(0.0f);
        return;
    }
    
    meshes[0].CalculateBounds(boundsMin, boundsMax);
    
    for (size_t i = 1; i < meshes.size(); ++i) {
        glm::vec3 min, max;
        meshes[i].CalculateBounds(min, max);
        
        boundsMin = glm::min(boundsMin, min);
        boundsMax = glm::max(boundsMax, max);
    }
}

size_t Model::GetTotalVertexCount() const {
    size_t count = 0;
    for (const auto& mesh : meshes) {
        count += mesh.vertices.size();
    }
    return count;
}

size_t Model::GetTotalIndexCount() const {
    size_t count = 0;
    for (const auto& mesh : meshes) {
        count += mesh.indices.size();
    }
    return count;
}

OBJLoader::OBJLoader() 
    : m_generateNormals(true)
    , m_generateTangents(false)
    , m_flipUVs(false) {
}

OBJLoader::~OBJLoader() {
}

bool OBJLoader::LoadModel(const std::string& filename, Model& model) {
    ClearError();
    
    std::string mtl_basedir;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        mtl_basedir = filename.substr(0, lastSlash + 1);
    }
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), mtl_basedir.c_str());
    
    if (!success) {
        SetError("Failed to load OBJ file: " + err);
        return false;
    }
    
    if (!warn.empty()) {
        std::cout << "Warning while loading " << filename << ": " << warn << std::endl;
    }
    
    model.meshes.clear();
    model.materials.clear();
    model.name = filename;
    
    for (const auto& mat : materials) {
        model.materials.push_back(ConvertMaterial(mat));
    }
    
    if (model.materials.empty()) {
        Material defaultMat;
        defaultMat.name = "default";
        model.materials.push_back(defaultMat);
    }
    
    if (!ProcessShapes(shapes, materials, attrib, model)) {
        return false;
    }
    model.CalculateBounds();    
    return true;
}

bool OBJLoader::LoadModel(const std::string& filename, Model& model, const Material& material) {
    ClearError();
    
    std::string mtl_basedir;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        mtl_basedir = filename.substr(0, lastSlash + 1);
    }
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), mtl_basedir.c_str());
    
    if (!success) {
        SetError("Failed to load OBJ file: " + err);
        return false;
    }
    
    model.meshes.clear();
    model.materials.clear();
    model.name = filename;
    
    model.materials.push_back(material);
    
    if (!ProcessShapes(shapes, materials, attrib, model)) {
        return false;
    }
    
    model.CalculateBounds();    
    return true;
}

bool OBJLoader::LoadModels(const std::vector<std::string>& filenames, std::vector<Model>& models) {
    models.clear();
    models.reserve(filenames.size());
    
    for (const auto& filename : filenames) {
        Model model;
        if (!LoadModel(filename, model)) {
            SetError("Failed to load model: " + filename + " - " + GetLastError());
            return false;
        }
        models.push_back(std::move(model));
    }
    
    return true;
}

bool OBJLoader::ProcessShapes(const std::vector<tinyobj::shape_t>& shapes,
                             const std::vector<tinyobj::material_t>& materials,
                             const tinyobj::attrib_t& attrib,
                             Model& model) {
    for (const auto& shape : shapes) {
        if (!ProcessShape(shape, materials, attrib, model)) {
            return false;
        }
    }
    
    return true;
}

bool OBJLoader::ProcessShape(const tinyobj::shape_t& shape,
                            const std::vector<tinyobj::material_t>& materials,
                            const tinyobj::attrib_t& attrib,
                            Model& model) {
    Mesh mesh;
    mesh.name = shape.name;
    
    if (!materials.empty() && shape.mesh.material_ids.size() > 0) {
        mesh.materialIndex = shape.mesh.material_ids[0];
        if (mesh.materialIndex < 0 || mesh.materialIndex >= static_cast<int>(materials.size())) {
            mesh.materialIndex = 0;
        }
    } else {
        mesh.materialIndex = 0;
    }
    
    size_t index_offset = 0;
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
        int fv = shape.mesh.num_face_vertices[f];
        
        if (fv != 3) {
            std::cout << "Warning: Non-triangle face found, skipping" << std::endl;
            index_offset += fv;
            continue;
        }
        
        for (int v = 0; v < fv; v++) {
            tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
            
            Vertex vertex;
            vertex.position = GetVertexPosition(attrib, idx.vertex_index);
            vertex.normal = GetVertexNormal(attrib, idx.normal_index);
            vertex.texCoord = GetVertexTexCoord(attrib, idx.texcoord_index);
            vertex.tangent = GetVertexTangent(attrib, idx.vertex_index);
            
            mesh.vertices.push_back(vertex);
        }
        
        size_t baseIndex = mesh.vertices.size() - 3;
        mesh.indices.push_back(static_cast<unsigned int>(baseIndex));
        mesh.indices.push_back(static_cast<unsigned int>(baseIndex + 1));
        mesh.indices.push_back(static_cast<unsigned int>(baseIndex + 2));
        
        index_offset += fv;
    }
    
    if (m_generateNormals) {
        bool hasNormals = false;
        for (const auto& vertex : mesh.vertices) {
            if (glm::length(vertex.normal) > 0.0f) {
                hasNormals = true;
                break;
            }
        }
        
        if (!hasNormals) {
            mesh.CalculateNormals();
        }
    }
    
    if (m_generateTangents) {
        mesh.CalculateTangents();
    }
    
    model.meshes.push_back(std::move(mesh));
    
    return true;
}

Material OBJLoader::ConvertMaterial(const tinyobj::material_t& mat) {
    Material material;
    
    material.name = mat.name;
    material.ambient = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
    material.diffuse = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
    material.specular = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
    material.shininess = mat.shininess;
    material.transparency = mat.dissolve;
    material.refractiveIndex = mat.ior;
    
    if (!mat.diffuse_texname.empty()) {
        material.diffuseTex = mat.diffuse_texname;
    }
    if (!mat.bump_texname.empty()) {
        material.normalTex = mat.bump_texname;
    }
    if (!mat.specular_texname.empty()) {
        material.specularTex = mat.specular_texname;
    }
    
    return material;
}

glm::vec3 OBJLoader::GetVertexPosition(const tinyobj::attrib_t& attrib, int index) {
    if (index < 0 || index >= static_cast<int>(attrib.vertices.size() / 3)) {
        return glm::vec3(0.0f);
    }
    
    return glm::vec3(
        attrib.vertices[3 * index + 0],
        attrib.vertices[3 * index + 1],
        attrib.vertices[3 * index + 2]
    );
}

glm::vec3 OBJLoader::GetVertexNormal(const tinyobj::attrib_t& attrib, int index) {
    if (index < 0 || index >= static_cast<int>(attrib.normals.size() / 3)) {
        return glm::vec3(0.0f);
    }
    
    return glm::vec3(
        attrib.normals[3 * index + 0],
        attrib.normals[3 * index + 1],
        attrib.normals[3 * index + 2]
    );
}

glm::vec2 OBJLoader::GetVertexTexCoord(const tinyobj::attrib_t& attrib, int index) {
    if (index < 0 || index >= static_cast<int>(attrib.texcoords.size() / 2)) {
        return glm::vec2(0.0f);
    }
    
    glm::vec2 texCoord(
        attrib.texcoords[2 * index + 0],
        attrib.texcoords[2 * index + 1]
    );
    
    if (m_flipUVs) {
        texCoord.y = 1.0f - texCoord.y;
    }
    
    return texCoord;
}

glm::vec3 OBJLoader::GetVertexTangent(const tinyobj::attrib_t& attrib, int index) {
    if (index < 0 || index >= static_cast<int>(attrib.vertices.size() / 3)) {
        return glm::vec3(0.0f);
    }
    return glm::vec3(0.0f);
}

void OBJLoader::SetError(const std::string& error) {
    m_lastError = error;
    std::cerr << "OBJLoader Error: " << error << std::endl;
}
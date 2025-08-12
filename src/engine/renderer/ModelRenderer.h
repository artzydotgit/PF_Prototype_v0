#pragma once

#include "../backend/OBJLoader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

class Shader;

class ModelRenderer {
public:
    ModelRenderer();
    ~ModelRenderer();

    bool Initialize();
    void Shutdown();
    void SetCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
    void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f);
    void RenderModel(const Model& model, const glm::mat4& modelMatrix = glm::mat4(1.0f));
    void RenderMesh(const Mesh& mesh, const Material& material, const glm::mat4& modelMatrix);
    bool IsInitialized() const { return m_initialized; }

private:
    struct MeshData {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        size_t indexCount;
        bool initialized;
        
        MeshData() : VAO(0), VBO(0), EBO(0), indexCount(0), initialized(false) {}
    };
    
    bool CreateMeshBuffers(const Mesh& mesh, MeshData& meshData);
    void DeleteMeshBuffers(MeshData& meshData);
    bool CreateShaders();
    void SetShaderUniforms(const Material& material, const glm::mat4& modelMatrix);
    void SetMaterialUniforms(const Material& material);

private:
    bool m_initialized;
    
    std::unique_ptr<Shader> m_shader;
    
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    
    glm::vec3 m_lightPosition;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    
    glm::vec3 m_cameraPosition;
    glm::vec3 m_cameraTarget;
    glm::vec3 m_cameraUp;
    
    std::vector<MeshData> m_meshBuffers;
    
    Material m_defaultMaterial;
}; 
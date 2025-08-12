#include "ModelRenderer.h"
#include "Shader.h"
#include <iostream>

ModelRenderer::ModelRenderer() 
    : m_initialized(false)
    , m_lightPosition(5.0f, 5.0f, 5.0f)
    , m_lightColor(1.0f, 1.0f, 1.0f)
    , m_lightIntensity(1.0f)
    , m_cameraPosition(0.0f, 0.0f, 5.0f)
    , m_cameraTarget(0.0f, 0.0f, 0.0f)
    , m_cameraUp(0.0f, 1.0f, 0.0f) {
    
    m_defaultMaterial.name = "default";
    m_defaultMaterial.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    m_defaultMaterial.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_defaultMaterial.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    m_defaultMaterial.shininess = 32.0f;
    
    m_viewMatrix = glm::lookAt(m_cameraPosition, m_cameraTarget, m_cameraUp);
    m_projectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f);
}

ModelRenderer::~ModelRenderer() {
    Shutdown();
}

bool ModelRenderer::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    if (!CreateShaders()) {
        std::cerr << "Failed to create shaders" << std::endl;
        return false;
    }
    
    m_initialized = true;
    return true;
}

void ModelRenderer::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    for (auto& meshData : m_meshBuffers) {
        DeleteMeshBuffers(meshData);
    }
    m_meshBuffers.clear();
    
    m_initialized = false;
}

void ModelRenderer::SetCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    m_cameraPosition = position;
    m_cameraTarget = target;
    m_cameraUp = up;
    m_viewMatrix = glm::lookAt(position, target, up);
}

void ModelRenderer::SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane) {
    m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void ModelRenderer::SetLight(const glm::vec3& position, const glm::vec3& color, float intensity) {
    m_lightPosition = position;
    m_lightColor = color;
    m_lightIntensity = intensity;
}

void ModelRenderer::RenderModel(const Model& model, const glm::mat4& modelMatrix) {
    if (!m_initialized || !m_shader || !m_shader->IsValid()) {
        return;
    }
    
    while (m_meshBuffers.size() < model.meshes.size()) {
        m_meshBuffers.emplace_back();
    }
    
    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];
        
        const Material* material = &m_defaultMaterial;
        if (mesh.materialIndex >= 0 && mesh.materialIndex < static_cast<int>(model.materials.size())) {
            material = &model.materials[mesh.materialIndex];
        }
        
        if (!m_meshBuffers[i].initialized) {
            if (!CreateMeshBuffers(mesh, m_meshBuffers[i])) {
                std::cerr << "Failed to create buffers for mesh: " << mesh.name << std::endl;
                continue;
            }
        }
        
        RenderMesh(mesh, *material, modelMatrix);
    }
}

void ModelRenderer::RenderMesh(const Mesh& mesh, const Material& material, const glm::mat4& modelMatrix) {
    if (!m_initialized || !m_shader || !m_shader->IsValid()) {
        return;
    }
    
    MeshData* meshData = nullptr;
    for (auto& md : m_meshBuffers) {
        if (md.initialized) {
            meshData = &md;
            break;
        }
    }
    
    if (!meshData || !meshData->initialized) {
        return;
    }
    
    m_shader->Use();
    SetShaderUniforms(material, modelMatrix);
    
    glBindVertexArray(meshData->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(meshData->indexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool ModelRenderer::CreateMeshBuffers(const Mesh& mesh, MeshData& meshData) {
    if (meshData.initialized) {
        DeleteMeshBuffers(meshData);
    }
    
    glGenVertexArrays(1, &meshData.VAO);
    glBindVertexArray(meshData.VAO);
    
    glGenBuffers(1, &meshData.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    
    glGenBuffers(1, &meshData.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    meshData.indexCount = mesh.indices.size();
    
    glBindVertexArray(0);
    
    meshData.initialized = true;
    return true;
}

void ModelRenderer::DeleteMeshBuffers(MeshData& meshData) {
    if (meshData.VAO != 0) {
        glDeleteVertexArrays(1, &meshData.VAO);
        meshData.VAO = 0;
    }
    
    if (meshData.VBO != 0) {
        glDeleteBuffers(1, &meshData.VBO);
        meshData.VBO = 0;
    }
    
    if (meshData.EBO != 0) {
        glDeleteBuffers(1, &meshData.EBO);
        meshData.EBO = 0;
    }
    
    meshData.indexCount = 0;
    meshData.initialized = false;
}

bool ModelRenderer::CreateShaders() {
    m_shader = std::make_unique<Shader>();    
    if (!m_shader->CreateFromFiles("assets/shaders/default/default.vert", "assets/shaders/default/default.frag")) {
        std::cerr << "Failed to create shader program from files" << std::endl;
        return false;
    }
    return true;
}

void ModelRenderer::SetShaderUniforms(const Material& material, const glm::mat4& modelMatrix) {
    if (!m_shader || !m_shader->IsValid()) {
        return;
    }
    
    m_shader->SetMat4("model", modelMatrix);
    m_shader->SetMat4("view", m_viewMatrix);
    m_shader->SetMat4("projection", m_projectionMatrix);
    
    m_shader->SetVec3("lightPos", m_lightPosition);
    m_shader->SetVec3("lightColor", m_lightColor * m_lightIntensity);
    m_shader->SetVec3("viewPos", m_cameraPosition);
    
    SetMaterialUniforms(material);
}

void ModelRenderer::SetMaterialUniforms(const Material& material) {
    if (!m_shader || !m_shader->IsValid()) {
        return;
    }
    
    m_shader->SetVec3("ambient", material.ambient);
    m_shader->SetVec3("diffuse", material.diffuse);
    m_shader->SetVec3("specular", material.specular);
    m_shader->SetFloat("shininess", material.shininess);
} 
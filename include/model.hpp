//
// Created by xmmmmmovo on 12/16/2023.
//

#ifndef INCLUDE_MODEL_B1692832D36D42B987EEEDD2E69B1C48
#define INCLUDE_MODEL_B1692832D36D42B987EEEDD2E69B1C48

#include <glad/glad.h>

#include <cstdint>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader.hpp"
#include "texture.hpp"

namespace cg {

class ModelMatrix {
    glm::mat4 model_matrix{glm::mat4{1.0f}};
    glm::mat3 normal_mat33{glm::mat3{1.0f}};

    glm::vec3 trans{};
    glm::vec3 scale{};
    glm::vec3 rotation{};

    void updateNormalMat33() {
        normal_mat33 = glm::transpose(glm::inverse(glm::mat3{model_matrix}));
    }

    void updateModelMatrix() {
        // apply trans normal scale
        glm::mat4 ret_mat4{1.0f};

        ret_mat4 = glm::scale(ret_mat4, scale);
        ret_mat4 = glm::eulerAngleYXZ(rotation.x, rotation.y, rotation.z) *
                   ret_mat4;
        ret_mat4 = glm::translate(glm::mat4(1.0f), trans) * ret_mat4;

        model_matrix = ret_mat4;
        updateNormalMat33();
    }

public:
    [[nodiscard]] glm::mat4 const& getModelMatrix() const {
        return model_matrix;
    }

    [[nodiscard]] glm::mat3 const& getNormalMat33() const {
        return normal_mat33;
    }

    template<typename T>
    explicit ModelMatrix(T&& trans, T&& scale, T&& rotation)
        : trans(std::forward<T>(trans)), scale(std::forward<T>(scale)),
          rotation(std::forward<T>(rotation)) {
        updateModelMatrix();
    }

    void applyTranslate(glm::vec3 const& translate) {
        trans += translate;
        updateModelMatrix();
    }

    void applyRotate(glm::vec3 const& axis, float angle) {
        rotation += angle * axis;
        updateModelMatrix();
    }

    void applyScale(glm::vec3 const& scale) {
        this->scale *= scale;
        updateModelMatrix();
    }

    void setTranslate(glm::vec3 const& translate) {
        trans = translate;
        updateModelMatrix();
    }

    void setRotate(glm::vec3 const& axis, float angle) {
        rotation = angle * axis;
        updateModelMatrix();
    }
};

class Model {
public:
    struct Mesh {
        uint32_t vao{};
        uint32_t position{};
        uint32_t texCoord{};
        uint32_t normal{};

        uint32_t indices{};

        uint32_t material_index{0};
        uint32_t indices_num{0};

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoords;

        std::vector<uint32_t> indices_vec;

        void uploadToGPU() {
            // transfer to GPU
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            glGenBuffers(1, &position);
            glBindBuffer(GL_ARRAY_BUFFER, position);
            glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3),
                         positions.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glGenBuffers(1, &texCoord);
            glBindBuffer(GL_ARRAY_BUFFER, texCoord);
            glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2),
                         texCoords.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glGenBuffers(1, &normal);
            glBindBuffer(GL_ARRAY_BUFFER, normal);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
                         normals.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glGenBuffers(1, &indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indices_vec.size() * sizeof(uint32_t),
                         indices_vec.data(), GL_STATIC_DRAW);

            glBindVertexArray(0);
        }
    };

    struct Material {
        uint32_t diffuse_texture{0};
    };

private:
    std::vector<Mesh>     meshes;
    std::vector<Material> materials;

private:
    Mesh loadMeshNode(aiMesh* mesh, const aiScene* scene) {
        // data to fill
        Mesh mesh_ret;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            glm::vec3 vector;// we declare a placeholder vector since assimp
                             // uses its own vector class that doesn't directly
                             // convert to glm's vec3 class, so we transfer the
                             // data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            mesh_ret.positions.push_back(vector);

            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                mesh_ret.normals.push_back(vector);
            }
            // texture coordinates
            if (mesh->mTextureCoords[0])// does the mesh contain texture
                                        // coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates.
                // We thus make the assumption that we won't use models where a
                // vertex can have multiple texture coordinates so we always
                // take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;

                mesh_ret.texCoords.push_back(vec);
            } else {
                mesh_ret.texCoords.emplace_back(0.0f, 0.0f);
            }
        }
        // now wak through each of the mesh's faces (a face is a mesh its
        // triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices
            // vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                mesh_ret.indices_vec.push_back(face.mIndices[j]);
        }

        mesh_ret.indices_num    = mesh_ret.indices_vec.size();
        mesh_ret.material_index = mesh->mMaterialIndex;

        mesh_ret.uploadToGPU();

        // return a mesh object created from the extracted mesh data
        return mesh_ret;
    }

    // from https://learnopengl.com/Model-Loading/Model
    // processes a node in a recursive fashion. Processes each individual mesh
    // located at the node and repeats this process on its children nodes (if
    // any).
    void processNode(aiNode* node, const aiScene* scene) {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            // the node object only contains indices to index the actual objects
            // in the scene. the scene contains all the data, node is just to
            // keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(loadMeshNode(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively
        // process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void loadMaterialNode(const aiScene*               scene,
                          std::filesystem::path const& directory) {
        for (int i = 0; i < scene->mNumMaterials; i++) {
            Material material;

            aiMaterial* mat = scene->mMaterials[i];

            // check for textures
            aiString diffuse_path;
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_path) ==
                AI_SUCCESS) {
                material.diffuse_texture =
                        loadTexture(directory / diffuse_path.C_Str());
            }

            materials.push_back(material);
        }
    }

public:
    explicit Model(std::filesystem::path const& path) {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene*   scene = importer.ReadFile(
                path.generic_string(),
                aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                        aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
            !scene->mRootNode)// if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString()
                      << '\n';
            exit(1);
        }

        loadMaterialNode(scene, path.parent_path());

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    ~Model() {
        //        for (auto& mesh : meshes) {
        //            glDeleteVertexArrays(1, &mesh.vao);
        //            glDeleteBuffers(1, &mesh.position);
        //            glDeleteBuffers(1, &mesh.texCoord);
        //            glDeleteBuffers(1, &mesh.normal);
        //            glDeleteBuffers(1, &mesh.indices);
        //        }
        //
        //        for (auto& material : materials) {
        //            glDeleteTextures(1, &material.diffuse_texture);
        //        }
    }

    void draw(Shader& shader, glm::vec3 const& diffuse_color,
              ModelMatrix const& model_matrix) {
        shader.setMat4("model", model_matrix.getModelMatrix());
        shader.setMat3("normMat33", model_matrix.getNormalMat33());

        shader.setVec3("diffuseColor", diffuse_color);

        for (auto mesh : meshes) {
            glBindVertexArray(mesh.vao);
            glDrawElements(GL_TRIANGLES, mesh.indices_num, GL_UNSIGNED_INT,
                           nullptr);
        }
        //
//        glBindVertexArray(meshes[1].vao);
//        glDrawElements(GL_TRIANGLES, meshes[1].indices_num, GL_UNSIGNED_INT,
//                       nullptr);
    }

    void combineModel(Model model, ModelMatrix const& model_matrix) {
        uint32_t ssize = materials.size();
        materials.insert(materials.end(), model.materials.begin(),
                         model.materials.end());

        for (auto& mesh : model.meshes) {
            if (mesh.material_index != 0) {
                mesh.material_index = ssize + mesh.material_index;
            }

            for (auto& pos : mesh.positions) {
                pos = model_matrix.getModelMatrix() * glm::vec4(pos, 1.0f);
            }

            for (auto& nor : mesh.normals) {
                nor = model_matrix.getNormalMat33() * nor;
            }

            mesh.uploadToGPU();
        }
        meshes.insert(meshes.end(), model.meshes.begin(), model.meshes.end());
    }
};

}// namespace cg

#endif// INCLUDE_MODEL_B1692832D36D42B987EEEDD2E69B1C48

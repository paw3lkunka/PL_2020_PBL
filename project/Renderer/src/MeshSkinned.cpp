#include "mesh/MeshSkinned.hpp"
#include "mesh/MeshDataStructures.inl"
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

MeshSkinned::MeshSkinned(std::vector<VertexSkinned> vertices, std::vector<unsigned int> indices, Bounds bounds, std::string meshFilePath, std::string meshPath) : Mesh(meshFilePath, meshPath)
{
    this->vertices = vertices;
    this->bounds = bounds;
    this->indices = indices;
    this->bounds = bounds;
    setup();
}

void MeshSkinned::setup()
{
    // ===== Generate buffers =====
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // ===== Bind vao and send data =====
    glBindVertexArray(vao);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexSkinned), vertices.data(), GL_STATIC_DRAW);
    std::cout << "Vertices count: " << this->vertices.size() << '\n';

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    std::cout << "Indices count: " << this->indices.size() << '\n';

    // ===== Attrib pointers =====
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkinned), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkinned), (void*)offsetof(VertexSkinned, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkinned), (void*)offsetof(VertexSkinned, tangent));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSkinned), (void*)offsetof(VertexSkinned, texcoord));

    // ===== Bone attribs =====
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, VertexSkinned::MAX_WEIGHTS, GL_INT, sizeof(VertexSkinned), (void*)offsetof(VertexSkinned, boneIDs));

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, VertexSkinned::MAX_WEIGHTS, GL_FLOAT, GL_FALSE, sizeof(VertexSkinned), (void*)offsetof(VertexSkinned, weights));

    // ===== Reset vao =====
    glBindVertexArray(0);
}

void MeshSkinned::render()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
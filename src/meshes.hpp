#ifndef MESHES_HPP
#define MESHES_HPP

#include <array>
#include <glm/glm.hpp>
#include <vector>

using Vertex = glm::vec3;
using Triangle = std::array<unsigned int, 3>;

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<Triangle> triangles;

  Mesh(const std::vector<Vertex> &vertices_,
       const std::vector<Triangle> &triangles_)
      : vertices(vertices_), triangles(triangles_) {}

  void addVertex(const Vertex &vertex) { vertices.push_back(vertex); }

  void addTriangle(const Triangle &triangle) {
    // TODO: check if triangle is valid
    triangles.push_back(triangle);
  }

  void clear() {
    vertices.clear();
    triangles.clear();
  }
};

Mesh createIcosahedron();
Mesh createIcosphere(unsigned int n);
Mesh subdivideTriangles(const Mesh &mesh);
Mesh projectToUnitSphere(const Mesh &mesh);
std::vector<Vertex> projectToUnitSphere(const std::vector<Vertex> vertices);

#endif // MESHES_HPP
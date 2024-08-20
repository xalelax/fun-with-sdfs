#include "meshes.hpp"
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <vector>

std::vector<Vertex> projectToUnitSphere(const std::vector<Vertex> vertices) {
  std::vector<Vertex> new_vertices;
  for (const auto &v : vertices) {
    new_vertices.push_back(glm::normalize(v));
  }
  return new_vertices;
}

Mesh projectToUnitSphere(const Mesh &mesh) {
  return Mesh(projectToUnitSphere(mesh.vertices), mesh.triangles);
}

Mesh createIcosahedron() {
  float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
  float a = 1.0f;
  float b = 1.0f / phi;

  // add vertices
  auto v0 = Vertex(0, b, -a);
  auto v1 = Vertex(b, a, 0);
  auto v2 = Vertex(-b, a, 0);
  auto v3 = Vertex(0, b, a);
  auto v4 = Vertex(0, -b, a);
  auto v5 = Vertex(-a, 0, b);
  auto v6 = Vertex(0, -b, -a);
  auto v7 = Vertex(a, 0, -b);
  auto v8 = Vertex(a, 0, b);
  auto v9 = Vertex(-a, 0, -b);
  auto v10 = Vertex(b, -a, 0);
  auto v11 = Vertex(-b, -a, 0);

  auto vertices =
      std::vector<Vertex>{v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11};

  vertices = projectToUnitSphere(vertices);

  auto triangles = std::vector<Triangle>{
      Triangle{2, 1, 0},   Triangle{1, 2, 3},   Triangle{5, 4, 3},
      Triangle{4, 8, 3},   Triangle{7, 6, 0},   Triangle{6, 9, 0},
      Triangle{11, 10, 4}, Triangle{10, 11, 6}, Triangle{9, 5, 2},
      Triangle{5, 9, 11},  Triangle{8, 7, 1},   Triangle{7, 8, 10},
      Triangle{2, 5, 3},   Triangle{8, 1, 3},   Triangle{9, 2, 0},
      Triangle{1, 7, 0},   Triangle{11, 9, 6},  Triangle{7, 10, 6},
      Triangle{5, 11, 4},  Triangle{10, 8, 4},
  };

  return Mesh(vertices, triangles);
}

Mesh subdivideTriangles(const Mesh &mesh) {
  std::vector<Vertex> newVertices;
  std::vector<Triangle> newTriangles;

  for (const auto &triangle : mesh.triangles) {
    // find the midpoint of each edge; notation:
    //
    //     1
    //    * *
    //   a   b
    //  *     *
    // 0 * c * 2
    //
    // Notice: each midpoint will be calculated twice, but that's ok
    Vertex a = (mesh.vertices[triangle[0]] + mesh.vertices[triangle[1]]) * 0.5f;
    Vertex b = (mesh.vertices[triangle[1]] + mesh.vertices[triangle[2]]) * 0.5f;
    Vertex c = (mesh.vertices[triangle[0]] + mesh.vertices[triangle[2]]) * 0.5f;

    // add these vertices to the mesh
    unsigned int offset = newVertices.size();
    newVertices.push_back(mesh.vertices[triangle[0]]);
    newVertices.push_back(mesh.vertices[triangle[1]]);
    newVertices.push_back(mesh.vertices[triangle[2]]);
    newVertices.push_back(a); // 3 + offset
    newVertices.push_back(b); // 4 + offset
    newVertices.push_back(c); // 5 + offset

    // replace the old triangle with 4 new triangles; notice: clockwise order
    newTriangles.push_back(Triangle{0 + offset, 3 + offset, 5 + offset});
    newTriangles.push_back(Triangle{3 + offset, 4 + offset, 5 + offset});
    newTriangles.push_back(Triangle{3 + offset, 1 + offset, 4 + offset});
    newTriangles.push_back(Triangle{5 + offset, 4 + offset, 2 + offset});
  }

  return Mesh(newVertices, newTriangles);
}

Mesh createIcosphere(unsigned int n) {
  Mesh mesh = createIcosahedron();

  for (unsigned int i = 0; i < n; ++i) {
    mesh = subdivideTriangles(mesh);
    mesh = projectToUnitSphere(mesh);
  }

  return mesh;
}
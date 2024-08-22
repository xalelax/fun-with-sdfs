#include "sdf.hpp"
#include <glm/ext/vector_float3.hpp>

Mesh morphTowardsBox(Mesh mesh) {
  glm::vec3 b = {1.f, 1.f, 2.f};

  // move each vertex towards the origin by a fraction of the distance to the
  // implicit surface
  for (auto &v : mesh.vertices) {
    float d = sdBox(v, b);

    float factor = d * 0.0003f;
    v *= 1.0f - factor;
  }
  return mesh;
}
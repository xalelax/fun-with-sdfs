#include "meshes.hpp"
#include <glm/glm.hpp>

inline float sdBox(glm::vec3 p, glm::vec3 b) {
  glm::vec3 q = abs(p) - b;
  return glm::length(glm::max(q, 0.0f)) +
         glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
}

Mesh morphTowardsBox(Mesh mesh);
#pragma once

#include "Renderer/IRenderer.hpp"

namespace Rose::Renderer::D3D12 {

  class Mesh : public IRenderer
  {
   public:
    Mesh() = default;
    ~Mesh() = default;
   
    std::shared_ptr<Rose::Framework::Internal::IMesh> CreateMesh(
      std::vector<Rose::Framework::Internal::Vertex> Vertices,
      std::vector<std::uint32_t> Indices
    ) override;
  };

} //  namespace Rose::Renderer::D3D12
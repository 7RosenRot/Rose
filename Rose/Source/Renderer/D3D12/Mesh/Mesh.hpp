#pragma once

#include "Renderer/IRenderer.hpp"

namespace Rose::Renderer::D3D12 {

  class Mesh : public IRenderer
  {
   public:
    Mesh() = default;
    ~Mesh() = default;
  };

} //  namespace Rose::Renderer::D3D12
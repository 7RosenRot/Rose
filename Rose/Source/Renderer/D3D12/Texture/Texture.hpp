#pragma once

#include "Renderer/IRenderer.hpp"

namespace Rose::Renderer::D3D12 {

  class Texture : public IRenderer
  {
   public:
    Texture() = default;
    ~Texture() = default;
  };

} //  namespace Rose::Renderer::D3D12
#pragma once

#include "Renderer/IRenderer.hpp"

namespace Rose::Renderer::D3D12 {

  class Texture : public IRenderer
  {
   public:
    Texture() = default;
    ~Texture() = default;
   
    std::shared_ptr<Rose::Framework::Internal::ITexture> CreateTexture() override;
  };

} //  namespace Rose::Renderer::D3D12
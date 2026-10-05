#pragma once

#include "Framework/AssetManager/Resources/ITexture.hpp"

namespace Rose::Renderer::D3D12
{

  class Texture : public Rose::Framework::Internal::ITexture
  {
   public:
    Texture() = default;
    ~Texture() = default;
  };

} //  namespace Rose::Renderer::D3D12
#pragma once

#include "Framework/AssetManager/Resources/IMesh.hpp"

namespace Rose::Renderer::D3D12
{

  class Mesh : public Rose::Framework::Internal::IMesh
  {
   public:
    Mesh() = default;
    virtual ~Mesh() = default;
  };

} //  namespace Rose::Renderer::D3D12
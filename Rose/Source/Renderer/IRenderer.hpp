#pragma once

#include <memory>
#include <vector>

#include "Core/Math/Math.hpp"
#include "Framework/AssetManager/Resources/IMesh.hpp"
#include "Framework/AssetManager/Resources/ITexture.hpp"

namespace Rose::Renderer {

  class IRenderer {
   public:
    IRenderer() = default;
    virtual ~IRenderer() = default;

    virtual std::shared_ptr<Rose::Framework::Internal::IMesh> CreateMesh(
      std::vector<Rose::Framework::Internal::Vertex> Vertices, std::vector<std::uint32_t> Indices
    ) = 0;
    
    virtual std::shared_ptr<Rose::Framework::Internal::ITexture> CreateTexture(
      std::uint32_t Width, std::uint32_t Height, unsigned char* pData
    ) = 0;
  };

} // namespace Rose::Renderer
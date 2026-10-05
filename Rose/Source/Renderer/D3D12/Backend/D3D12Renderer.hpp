#pragma once

#include "Renderer/IRenderer.hpp"
#include "Renderer/D3D12/Mesh/Mesh.hpp"
#include "Renderer/D3D12/Texture/Texture.hpp"

namespace Rose::Renderer::D3D12
{

  class D3D12Renderer : public IRenderer
  {
   public:
    D3D12Renderer() = default;
    virtual ~D3D12Renderer() override = default;

    virtual std::shared_ptr<Rose::Framework::Internal::IMesh> CreateMesh(
      std::vector<Rose::Framework::Internal::Vertex> Vertices, 
      std::vector<std::uint32_t> Indices
    ) override 
    {
      return std::make_shared<Rose::Renderer::D3D12::Mesh>();
    }
    
    virtual std::shared_ptr<Rose::Framework::Internal::ITexture> CreateTexture(
      std::uint32_t Width, 
      std::uint32_t Height, 
      unsigned char* pData
    ) override 
    {
      return std::make_shared<Rose::Renderer::D3D12::Texture>();
    }
  };

} //  namespace Rose::Renderer::D3D12
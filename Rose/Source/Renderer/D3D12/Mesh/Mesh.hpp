#pragma once

#include "Framework/AssetManager/Resources/IMesh.hpp"

namespace Rose::Renderer::D3D12
{

  class Mesh : public Rose::Framework::Internal::IMesh
  {
   public:
    Mesh(std::size_t VertexCount, std::size_t IndexCount)
     : m_VertexCount(VertexCount), m_IndexCount(IndexCount) {}
    
    virtual ~Mesh() = default;

    virtual std::size_t GetVertexCount() const override
    {
      return m_VertexCount;
    }
    
    virtual std::size_t GetIndexCount() const override
    {
      return m_IndexCount;
    }

   private:
    std::size_t m_VertexCount = 0;
    std::size_t m_IndexCount = 0;
  };

} //  namespace Rose::Renderer::D3D12
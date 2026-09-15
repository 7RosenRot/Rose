#pragma once

#include "IResourceLoader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <cstdint>

#include "Core/Math/Math.hpp"

namespace Rose::Framework::Internal {

  class ObjLoader : public IResourceLoader {
   public:
    ObjLoader() = default;
    ~ObjLoader() = default;

    std::string_view GetPattern() const override
    {
      return ".obj";
    }

    std::shared_ptr<IResource> LoadResource(const std::string& Path);

   private:
    bool ParseObjFile(
      _In_  const std::string& Path,
      _Out_ std::vector<Vertex>& Vertices,
      _Out_ std::vector<std::uint32_t>& Indices
    );

  };

} //  namespace Rose::Framework::Internal
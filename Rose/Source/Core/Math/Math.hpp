#pragma once

namespace Rose::Core::Math
{

  struct FLOAT3
  {
    float x;
    float y;
    float z;

    FLOAT3() = default;

    FLOAT3(const FLOAT3&) = default;
    FLOAT3& operator=(const FLOAT3&) = default;

    FLOAT3(FLOAT3&&) = default;
    FLOAT3& operator=(FLOAT3&&) = default;
  };

  struct FLOAT2
  {
    float x;
    float y;

    FLOAT2() = default;

    FLOAT2(const FLOAT2&) = default;
    FLOAT2& operator=(const FLOAT2&) = default;

    FLOAT2(FLOAT2&&) = default;
    FLOAT2& operator=(FLOAT2&&) = default;
  };

}  // namespace Rose::Core::Math
#ifndef FSLBASE_MATH_VIEWPORT_HPP
#define FSLBASE_MATH_VIEWPORT_HPP
/*
Microsoft Public License (Ms-PL)
MonoGame - Copyright (C) 2009 The MonoGame Team

All rights reserved.

This license governs use of the accompanying software. If you use the software, you accept this license. If you do not
accept the license, do not use the software.

1. Definitions
The terms "reproduce," "reproduction," "derivative works," and "distribution" have the same meaning here as under
U.S. copyright law.

A "contribution" is the original software, or any additions or changes to the software.
A "contributor" is any person that distributes its contribution under this license.
"Licensed patents" are a contributor's patent claims that read directly on its contribution.

2. Grant of Rights
(A) Copyright Grant- Subject to the terms of this license, including the license conditions and limitations in section 3,
each contributor grants you a non-exclusive, worldwide, royalty-free copyright license to reproduce its contribution, prepare derivative works of its
contribution, and distribute its contribution or any derivative works that you create. (B) Patent Grant- Subject to the terms of this license,
including the license conditions and limitations in section 3, each contributor grants you a non-exclusive, worldwide, royalty-free license under its
licensed patents to make, have made, use, sell, offer for sale, import, and/or otherwise dispose of its contribution in the software or derivative
works of the contribution in the software.

3. Conditions and Limitations
(A) No Trademark License- This license does not grant you rights to use any contributors' name, logo, or trademarks.
(B) If you bring a patent claim against any contributor over patents that you claim are infringed by the software,
your patent license from such contributor to the software ends automatically.
(C) If you distribute any portion of the software, you must retain all copyright, patent, trademark, and attribution
notices that are present in the software.
(D) If you distribute any portion of the software in source code form, you may do so only under this license by including
a complete copy of this license with your distribution. If you distribute any portion of the software in compiled or object
code form, you may only do so under a license that complies with this license.
(E) The software is licensed "as-is." You bear the risk of using it. The contributors give no express warranties, guarantees
or conditions. You may have additional consumer rights under your local laws which this license cannot change. To the extent
permitted under your local laws, the contributors exclude the implied warranties of merchantability, fitness for a particular
purpose and non-infringement.
*/

// The functions in this file are a port of an MIT licensed library: MonoGame - Viewport.cs.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Vector3.hpp>

namespace Fsl
{
  struct Matrix;

  struct Viewport
  {
    int32_t X{0};
    int32_t Y{0};
    int32_t Width{0};
    int32_t Height{0};
    float MinDepth{0.0f};
    float MaxDepth{0.0f};


    float GetAspectRatio() const
    {
      if ((Height != 0) && (Width != 0))
      {
        return static_cast<float>(Width) / static_cast<float>(Height);
      }
      return 0.0f;
    }

    Rectangle GetBounds() const
    {
      return Rectangle(X, Y, Width, Height);
    }

    void SetBounds(const Rectangle& value)
    {
      X = value.X();
      Y = value.Y();
      Width = value.Width();
      Height = value.Height();
    }

    constexpr Viewport() = default;

    constexpr Viewport(const int32_t x, const int32_t y, const int32_t width, const int32_t height)
      : X(x)
      , Y(y)
      , Width(width)
      , Height(height)
      , MaxDepth(1.0f)
    {
    }

    constexpr Viewport(const Rectangle& bounds)
      : Viewport(bounds.X(), bounds.Y(), bounds.Width(), bounds.Height())
    {
    }

    Vector3 Project(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world);
    Vector3 Unproject(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world);
  };
}


#endif

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

#include <FslBase/Math/Viewport.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MatrixFields.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <cmath>
#include <limits>

namespace Fsl
{
  using namespace MatrixFields;

  namespace
  {
    bool WithinEpsilon(const float a, const float b)
    {
      float num = a - b;
      return ((-1.401298E-45f <= num) && (num <= std::numeric_limits<float>::epsilon()));
    }
  }


  Vector3 Viewport::Project(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world)
  {
    Matrix matrix = Matrix::Multiply(Matrix::Multiply(world, view), projection);
    Vector3 vector = Vector3::Transform(source, matrix);

    const auto* pMatrix = matrix.DirectAccess();
    float a = (((source.X * pMatrix[_M14]) + (source.Y * pMatrix[_M24])) + (source.Z * pMatrix[_M34])) + pMatrix[_M44];
    if (!WithinEpsilon(a, 1.0f))
    {
      vector.X = vector.X / a;
      vector.Y = vector.Y / a;
      vector.Z = vector.Z / a;
    }
    vector.X = (((vector.X + 1.0f) * 0.5f) * Width) + X;
    vector.Y = (((-vector.Y + 1.0f) * 0.5f) * Height) + Y;
    vector.Z = (vector.Z * (MaxDepth - MinDepth)) + MinDepth;
    return vector;
  }

  Vector3 Viewport::Unproject(const Vector3& source, const Matrix& projection, const Matrix& view, const Matrix& world)
  {
    Matrix matrix = Matrix::Invert(Matrix::Multiply(Matrix::Multiply(world, view), projection));

    const Vector3 source1((((source.X - X) / static_cast<float>(Width)) * 2.0f) - 1.0f,
                          -((((source.Y - Y) / static_cast<float>(Height)) * 2.0f) - 1.0f), (source.Z - MinDepth) / (MaxDepth - MinDepth));


    Vector3 vector = Vector3::Transform(source1, matrix);

    const auto* pMatrix = matrix.DirectAccess();
    const float a = (((source1.X * pMatrix[_M14]) + (source1.Y * pMatrix[_M24])) + (source1.Z * pMatrix[_M34])) + pMatrix[_M44];

    if (!WithinEpsilon(a, 1.0f))
    {
      vector.X = vector.X / a;
      vector.Y = vector.Y / a;
      vector.Z = vector.Z / a;
    }
    return vector;
  }
}

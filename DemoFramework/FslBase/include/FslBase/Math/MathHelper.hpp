#ifndef FSLBASE_MATH_MATHHELPER_HPP
#define FSLBASE_MATH_MATHHELPER_HPP
/*
MIT License
Copyright (c) 2006 The Mono.Xna Team

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Some of the functions in this file are a port of an MIT licensed library: MonoGame - MathHelper.cs.

#include <algorithm>
#include <cmath>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/RectangleSizeRestrictionFlag.hpp>

namespace Fsl
{
  namespace MathHelper
  {
    constexpr float PI = 3.1415926535897932384626433832795028841971693f;
    //! PI / 180.0f
    constexpr float TO_RADS = 0.017453292519943295769236907684886f;
    //! 1.0f / (PI / 180.0f)
    constexpr float TO_DEGREES = 57.295779513082320876798154814105f;

    //! @brief Represents the log base ten of e(0.4342945).
    constexpr float Log10E = 0.4342945f;

    //! @brief Represents the log base two of e(1.442695).
    constexpr float Log2E = 1.442695f;

    /// Represents the value of pi divided by two(1.57079637).
    constexpr float PiOver2 = static_cast<float>(PI / 2.0);

    /// Represents the value of pi divided by four(0.7853982).
    constexpr float PiOver4 = static_cast<float>(PI / 4.0);

    /// Represents the value of pi times two(6.28318548).
    constexpr float TwoPi = static_cast<float>(PI * 2.0);

    constexpr float RADS360 = (360.0f * TO_RADS);
    constexpr float RADS315 = (315.0f * TO_RADS);
    constexpr float RADS270 = (270.0f * TO_RADS);
    constexpr float RADS225 = (225.0f * TO_RADS);
    constexpr float RADS180 = (180.0f * TO_RADS);
    constexpr float RADS135 = (135.0f * TO_RADS);
    constexpr float RADS90 = (90.0f * TO_RADS);
    constexpr float RADS45 = (45.0f * TO_RADS);
    constexpr float RADS0 = (0);

    //! @brief Returns the Cartesian coordinate for one axis of a point that is defined by a given triangle and two normalized barycentric (areal)
    //! coordinates.
    //! @param value1 The coordinate on one axis of vertex 1 of the defining triangle.
    //! @param value2 The coordinate on the same axis of vertex 2 of the defining triangle.
    //! @param value3 The coordinate on the same axis of vertex 3 of the defining triangle.
    //! @param amount1 The normalized barycentric (areal) coordinate b2, equal to the weighting factor for vertex 2, the coordinate of which is
    //! specified in value2.
    //! @param amount2 The normalized barycentric (areal) coordinate b3, equal to the weighting factor for vertex 3, the coordinate of which is
    //! specified in value3.
    //! @return Cartesian coordinate of the specified point with respect to the axis being used.
    constexpr inline float Barycentric(const float value1, const float value2, const float value3, const float amount1, const float amount2)
    {
      return value1 + (value2 - value1) * amount1 + (value3 - value1) * amount2;
    }


    //! @brief Performs a Catmull-Rom interpolation using the specified positions.
    //! @param value1 The first position in the interpolation.
    //! @param value2 The second position in the interpolation.
    //! @param value3 The third position in the interpolation.
    //! @param value4 The fourth position in the interpolation.
    //! @param amount Weighting factor.
    //! @return A position that is the result of the Catmull-Rom interpolation.
    inline float CatmullRom(const float value1, const float value2, const float value3, const float value4, const float amount)
    {
      // Using formula from http://www.mvps.org/directx/articles/catmull/
      // Internally using doubles not to lose precision
      const double amountSquared = static_cast<double>(amount) * amount;
      const double amountCubed = amountSquared * amount;
      return static_cast<float>(0.5 *
                                (2.0 * value2 + (value3 - value1) * amount + (2.0 * value1 - 5.0 * value2 + 4.0 * value3 - value4) * amountSquared +
                                 (3.0 * value2 - value1 - 3.0 * value3 + value4) * amountCubed));
    }


    //! @brief Restricts a value to be within a specified range.
    inline float Clamp(const float value, const float min, const float max)
    {
      return std::min(std::max(value, min), max);
    }


    //! @brief Calculates the absolute value of the difference of two values.
    //! @param value1 Source value.
    //! @param value2 Source value.
    //! @return Distance between the two values.
    inline float Distance(const float value1, const float value2)
    {
      return std::abs(value1 - value2);
    }


    //! @brief Calc the shortest distance between two angles in radians.
    float DistBetweenAngles(const float from, const float to);


    //! @brief Performs a Hermite spline interpolation.
    //! @param value1 Source position.
    //! @param tangent1 Source tangent.
    //! @param value2 Source position.
    //! @param tangent2 Source tangent.
    //! @param amount Weighting factor.
    //! @return The result of the Hermite spline interpolation.
    inline float Hermite(const float value1, const float tangent1, const float value2, const float tangent2, const float amount)
    {
      // All transformed to double not to lose precision
      // Otherwise, for high numbers of param:amount the result is NaN instead of Infinity
      const double v1 = value1, v2 = value2, t1 = tangent1, t2 = tangent2, s = amount;
      const double sCubed = s * s * s;
      const double sSquared = s * s;

      double result;
      if (amount == 0.0f)
      {
        result = value1;
      }
      else if (amount == 1.0f)
      {
        result = value2;
      }
      else
      {
        result = (((2 * v1) - (2 * v2) + t2 + t1) * sCubed) + (((3 * v2) - (3 * v1) - (2 * t1) - t2) * sSquared) + (t1 * s) + v1;
      }
      return static_cast<float>(result);
    }


    //! @brief Determines if value is powered by two.
    //! @param value A value.
    //! @return true if value is powered by two; otherwise false.
    constexpr inline static bool IsPowerOfTwo(const int value)
    {
      return (value > 0) && ((value & (value - 1)) == 0);
    }


    //! @brief Linearly interpolates between two values.
    constexpr inline float Lerp(const float value1, const float value2, const float amount)
    {
      return value1 + ((value2 - value1) * amount);
    }


    //! @brief Linearly interpolates between two values.
    //!  This method is a less efficient, more precise version of MathHelper::Lerp.
    //!  See remarks for more info.
    //! @param value1">Source value.
    //! @param value2">Destination value.
    //! @param amount">Value between 0 and 1 indicating the weight of value2.
    //! @return Interpolated value.
    //! @note   This method performs the linear interpolation based on the following formula:
    //!             ((1 - amount) * value1) + (value2 * amount)
    //!         Passing amount a value of 0 will cause value1 to be returned, a value of 1 will cause value2 to be returned.
    //!         This method does not have the floating point precision issue that <see cref="MathHelper.Lerp"/> has.
    //!         i.e. If there is a big gap between value1 and value2 in magnitude (e.g. value1=10000000000000000, value2=1),
    //!         right at the edge of the interpolation range (amount=1), <see cref="MathHelper.Lerp"/> will return 0 (whereas it should return 1).
    //!         This also holds for value1=10^17, value2=10; value1=10^18,value2=10^2... so on.
    //!         For an in depth explanation of the issue, see below references:
    //!         Relevant Wikipedia Article: https://en.wikipedia.org/wiki/Linear_interpolation#Programming_language_support
    //!         Relevant StackOverflow Answer: http://stackoverflow.com/questions/4353525/floating-point-linear-interpolation#answer-23716956
    constexpr inline float LerpPrecise(const float value1, const float value2, const float amount)
    {
      return ((1 - amount) * value1) + (value2 * amount);
    }

    //! @brief A number that indicates the sign of value, as shown in the following table.
    //! @Return value Meaning
    //!         -1 = value is less than zero.
    //!          0 = value is equal to zero.
    //!          1 = value is greater than zero.
    constexpr inline int Sign(const float value)
    {
      // Reference: 4.7 [conv.integral] / 4: If the source type is bool... true is converted to one.
      return static_cast<int>(0.0f < value) - static_cast<int>(value < 0.0f);
    }


    //! @brief Interpolates between two values using a cubic equation.
    //! @param value1 Source value.
    //! @param value2 Source value.
    //! @param amount Weighting value.
    //! @return Interpolated value.
    inline float SmoothStep(const float value1, const float value2, const float amount)
    {
      // It is expected that 0 < amount < 1
      // If amount < 0, return value1
      // If amount > 1, return value2
      const float result = Clamp(amount, 0.0f, 1.0f);
      return Hermite(value1, 0.0f, value2, 0.0f, result);
    }


    //! @brief Converts radians to degrees.
    //! @param radians The angle in radians.
    //! @return The angle in degrees.
    constexpr inline float ToDegrees(const float radians)
    {
      // We use doubles to not loose precision
      return static_cast<float>(radians * 57.295779513082320876798154814105);
    }


    //! @brief Converts degrees to radians.
    //! @param degrees The angle in degrees.
    //! @return The angle in radians.
    constexpr inline float ToRadians(const float degrees)
    {
      // We use doubles to not loose precision
      return static_cast<float>(degrees * 0.017453292519943295769236907684886);
    }


    //! @brief Find the nearest power of two value that is greater or equal the input value (>=)
    //! @param value must be >= 0;
    int ToPowerOfTwo(const int value);


    //! @brief Reduces a given angle to a value between PI and -PI.
    //! @param angle The angle to reduce, in radians.
    //! @return The new angle, in radians.
    float WrapAngle(const float angle);


    //! @brief Find the optimal rectangle size for packing a given amount of uniform sized units under the supplied constraints
    //! @param unitSize the size of the unit (x > 0 && y > 0)
    //! @param unitCount the total number of units > 0
    Point2 CalcOptimalSize(const Point2& unitSize, const int32_t unitCount, const RectangleSizeRestrictionFlag::Enum restrictionFlags);
  };
}

#endif

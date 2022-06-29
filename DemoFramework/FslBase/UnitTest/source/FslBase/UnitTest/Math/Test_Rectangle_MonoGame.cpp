/*
MIT License
Copyright (C) 2006 The Mono.Xna Team

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

// The functions in this file are a port of an MIT licensed library: MonoGame - RectangleTest.cs.

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Rectangle_MonoGame = TestFixtureFslBase;
}


TEST(TestMath_Rectangle_MonoGame, ConstructorsAndProperties)
{
  const Rectangle rectangle(10, 20, 64, 64);

  // Constructor
  EXPECT_EQ(Rectangle(10, 20, 64, 64), rectangle);

  // Left property
  EXPECT_EQ(10, rectangle.Left());

  // Right property
  EXPECT_EQ(64 + 10, rectangle.Right());

  // Top property
  EXPECT_EQ(20, rectangle.Top());

  // Bottom property
  EXPECT_EQ(64 + 20, rectangle.Bottom());

  // Location property
  EXPECT_EQ(Point2(10, 20), rectangle.TopLeft());

  // Center property

  EXPECT_EQ(Point2(10 + 32, 20 + 32), rectangle.GetCenter());

  // IsEmpty property
  EXPECT_FALSE(rectangle.IsEmpty());
  EXPECT_TRUE(Rectangle().IsEmpty());

  // Empty - static property
  EXPECT_EQ(Rectangle(), Rectangle::Empty());
}

TEST(TestMath_Rectangle_MonoGame, ContainsPoint)
{
  const Rectangle rectangle(0, 0, 64, 64);

  const Point2 p1(-1, -1);
  const Point2 p2(0, 0);
  const Point2 p3(32, 32);
  const Point2 p4(63, 63);
  const Point2 p5(64, 64);

  EXPECT_FALSE(rectangle.Contains(p1));
  EXPECT_TRUE(rectangle.Contains(p2));
  EXPECT_TRUE(rectangle.Contains(p3));
  EXPECT_TRUE(rectangle.Contains(p4));
  EXPECT_FALSE(rectangle.Contains(p5));
}


// TEST(TestMath_Rectangle_MonoGame, ContainsVector2)
//{
//  const Rectangle rectangle(0, 0, 64, 64);
//  const Vector2 p1(-1, -1);
//  const Vector2 p2(0, 0);
//  const Vector2 p3(32, 32);
//  const Vector2 p4(63, 63);
//  const Vector2 p5(64, 64);
//
//  EXPECT_FALSE(rectangle.Contains(p1));
//  EXPECT_TRUE(rectangle.Contains(p2));
//  EXPECT_TRUE(rectangle.Contains(p3));
//  EXPECT_TRUE(rectangle.Contains(p4));
//  EXPECT_FALSE(rectangle.Contains(p5));
//}


TEST(TestMath_Rectangle_MonoGame, ContainsInts)
{
  Rectangle rectangle(0, 0, 64, 64);

  const int x1 = -1;
  const int y1 = -1;
  const int x2 = 0;
  const int y2 = 0;
  const int x3 = 32;
  const int y3 = 32;
  const int x4 = 63;
  const int y4 = 63;
  const int x5 = 64;
  const int y5 = 64;

  EXPECT_FALSE(rectangle.Contains(x1, y1));
  EXPECT_TRUE(rectangle.Contains(x2, y2));
  EXPECT_TRUE(rectangle.Contains(x3, y3));
  EXPECT_TRUE(rectangle.Contains(x4, y4));
  EXPECT_FALSE(rectangle.Contains(x5, y5));
}


TEST(TestMath_Rectangle_MonoGame, ContainsFloats)
{
  const Rectangle rectangle(0, 0, 64, 64);
  const float x1 = -1;
  const float y1 = -1;
  const float x2 = 0;
  const float y2 = 0;
  const float x3 = 32;
  const float y3 = 32;
  const float x4 = 63;
  const float y4 = 63;
  const float x5 = 64;
  const float y5 = 64;

  EXPECT_FALSE(rectangle.Contains(x1, y1));
  EXPECT_TRUE(rectangle.Contains(x2, y2));
  EXPECT_TRUE(rectangle.Contains(x3, y3));
  EXPECT_TRUE(rectangle.Contains(x4, y4));
  EXPECT_FALSE(rectangle.Contains(x5, y5));
}


TEST(TestMath_Rectangle_MonoGame, ContainsRectangle)
{
  Rectangle rectangle(0, 0, 64, 64);
  Rectangle rect1(-1, -1, 32, 32);
  Rectangle rect2(0, 0, 32, 32);
  Rectangle rect3(0, 0, 64, 64);
  Rectangle rect4(1, 1, 64, 64);

  EXPECT_FALSE(rectangle.Contains(rect1));
  EXPECT_TRUE(rectangle.Contains(rect2));
  EXPECT_TRUE(rectangle.Contains(rect3));
  EXPECT_FALSE(rectangle.Contains(rect4));
}


TEST(TestMath_Rectangle_MonoGame, Inflate)
{
  Rectangle rectangle(0, 0, 64, 64);
  rectangle.Inflate(10, -10);

  EXPECT_EQ(Rectangle(-10, 10, 84, 44), rectangle);
}


TEST(TestMath_Rectangle_MonoGame, Intersect)
{
  const Rectangle first(0, 0, 64, 64);
  const Rectangle second(-32, -32, 64, 64);
  const Rectangle expected(0, 0, 32, 32);

  // First overload testing(forward and backward)

  Rectangle result = Rectangle::Intersect(first, second);

  EXPECT_EQ(expected, result);

  result = Rectangle::Intersect(second, first);

  EXPECT_EQ(expected, result);

  // Second overload testing(forward and backward)
  // Assert.AreEqual(expected, Rectangle.Intersect(first, second));
  // Assert.AreEqual(expected, Rectangle.Intersect(second, first));
}


TEST(TestMath_Rectangle_MonoGame, Union)
{
  const Rectangle first(-64, -64, 64, 64);
  const Rectangle second(0, 0, 64, 64);
  const Rectangle expected(-64, -64, 128, 128);

  // First overload testing(forward and backward)

  Rectangle result = Rectangle::Union(first, second);

  EXPECT_EQ(expected, result);

  result = Rectangle::Union(second, first);

  EXPECT_EQ(expected, result);

  // Second overload testing(forward and backward)
  // Assert.AreEqual(expected, Rectangle.Union(first, second));
  // Assert.AreEqual(expected, Rectangle.Union(second, first));
}

/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Simplified and adapted for use in the NXP demo framework 2024.

#include "LeastSquaresVelocityTrackerStrategy.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Log/Math/Dp/FmtDpPoint2F.hpp>
#include <FslBase/Log/Time/FmtMillisecondTickCount32.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

// #define LOCAL_DEBUG_GET_VELOCITY

namespace Fsl::UI
{
  namespace
  {
    namespace LocalConfig
    {
      // Sample horizon.
      // We don't use too much history by default since we want to react to quick
      // changes in direction.
      constexpr TimeSpan Horizon = TimeSpan::FromMilliseconds(100);

      constexpr uint32_t MinSampleCount = 3;

      constexpr uint32_t MinDegree = 2;
      constexpr uint32_t MaxDegree = 4;

      constexpr uint32_t MaxN = (MaxDegree + 1);
      constexpr uint32_t MaxM = AccumulatingVelocityTrackerStrategy::HistorySize;
      constexpr uint32_t MaxMatrixElements = MaxN * MaxM;
    }

    constexpr float VectorDot(const ReadOnlySpan<float> a, const ReadOnlySpan<float> b) noexcept
    {
      assert(a.size() == b.size());
      float r = 0;
      for (std::size_t i = 0; i < a.size(); ++i)
      {
        r += a[i] * b[i];
      }
      return r;
    }

    float VectorNorm(const ReadOnlySpan<float> a) noexcept
    {
      float r = 0;
      for (std::size_t i = 0; i < a.size(); ++i)
      {
        const float t = a[i];
        r += t * t;
      }
      return std::sqrt(r);
    }
  }

  LeastSquaresVelocityTrackerStrategy::LeastSquaresVelocityTrackerStrategy(const uint32_t degree, const Weighting weighting)
    : AccumulatingVelocityTrackerStrategy(LocalConfig::Horizon, true)
    , Degree(std::clamp(degree, LocalConfig::MinDegree, LocalConfig::MaxDegree))
    , DesiredWeighting(weighting)
  {
  }

  bool LeastSquaresVelocityTrackerStrategy::TryGetVelocity(DpValueF& rVelocityDpf) const noexcept
  {
#ifdef LOCAL_DEBUG_GET_VELOCITY
    {
      FSLLOG3_INFO("GetVelocity: movements {}", m_movements.size());
      if (m_movements.size() > 0)
      {
        // Assume there is no timestamps that are too old
        assert((m_movements.back().EventTime - m_movements.front().EventTime) <= LocalConfig::Horizon);
        auto lastTimestamp = m_movements.front().EventTime;
        for (std::size_t i = 0; i < m_movements.size(); ++i)
        {
          assert((m_movements[i].EventTime - lastTimestamp).Ticks() >= 0);
          lastTimestamp = m_movements[i].EventTime;
          FSLLOG3_INFO("- Time: {} Position: {}", m_movements[i].EventTime, m_movements[i].PositionDpf);
        }
      }
    }
#endif

    if (m_movements.size() <= 1u)
    {
      rVelocityDpf = {};
      return false;    // no data
    }
    if (m_movements.size() < LocalConfig::MinSampleCount)
    {
      // Not enough data, just do a linear estimate
      const auto& firstRecord = m_movements.front();
      const auto& lastRecord = m_movements.back();
      const TimeSpan duration = lastRecord.EventTime - firstRecord.EventTime;
      const DpValueF movementDpf = lastRecord.PositionDpf - firstRecord.PositionDpf;
      const double totalSeconds = duration.TotalSeconds();
      if (totalSeconds <= 0.0000000001f)
      {
        rVelocityDpf = {};
        return false;    // no data
      }
      // Convert to dp per second
      rVelocityDpf = DpValueF(static_cast<float>(movementDpf.Value / totalSeconds));
      return true;
    }

    uint32_t degree = Degree;
    if (degree > (m_movements.size() - 1))
    {
      degree = UncheckedNumericCast<uint32_t>(m_movements.size() - 1);
    }
    if (degree <= 0)
    {
      rVelocityDpf = {};
      return false;
    }
    if (degree == 2 && DesiredWeighting == Weighting::Disabled)
    {
      // Optimize unweighted, quadratic polynomial fit
      return TrySolveUnweightedLeastSquaresDeg2(m_movements, rVelocityDpf);
    }
    // Iterate over movement samples in reverse time order and collect samples.
    MovementRecord newestMovement = m_movements.back();
    std::size_t historyIndex = 0;
    for (int32_t i = UncheckedNumericCast<int32_t>(m_movements.size()) - 1; i >= 0; --i)
    {
      const MovementRecord& movement = m_movements[i];
      TimeSpan age = newestMovement.EventTime - movement.EventTime;
      const float w = ChooseWeight(i);
      const auto time = static_cast<float>(-age.TotalSeconds());
      m_scratchpad[historyIndex] = Record(movement.PositionDpf, w, time);
      ++historyIndex;
    }
    // General case for an Nth degree polynomial fit
    return TrySolveLeastSquares(SpanUtil::UncheckedAsReadOnlySpan(m_scratchpad, 0, historyIndex), degree + 1, rVelocityDpf);
  }

  float LeastSquaresVelocityTrackerStrategy::ChooseWeight(const std::size_t index) const noexcept
  {
    const std::size_t size = m_movements.size();
    if (size <= 0)
    {
      return 1.0f;
    }
    switch (DesiredWeighting)
    {
    case LeastSquaresVelocityTrackerStrategy::Weighting::Delta:
      {
        // Weight points based on how much time elapsed between them and the next
        // point so that points that "cover" a shorter time span are weighed less.
        //   delta  0ms: 0.5
        //   delta 10ms: 1.0
        if (index == size - 1)
        {
          return 1.0f;
        }
        double deltaMillis = (m_movements[index + 1].EventTime - m_movements[index].EventTime).TotalMilliseconds();
        if (deltaMillis < 0)
        {
          return 0.5f;
        }
        if (deltaMillis < 10)
        {
          return static_cast<float>(0.5 + deltaMillis * 0.05);
        }
        return 1.0f;
      }
    case LeastSquaresVelocityTrackerStrategy::Weighting::Central:
      {
        // Weight points based on their age, weighing very recent and very old points less.
        //   age  0ms: 0.5
        //   age 10ms: 1.0
        //   age 50ms: 1.0
        //   age 60ms: 0.5
        double ageMillis = (m_movements[size - 1].EventTime - m_movements[index].EventTime).TotalMilliseconds();
        if (ageMillis < 0)
        {
          return 0.5f;
        }
        if (ageMillis < 10)
        {
          return static_cast<float>(0.5 + ageMillis * 0.05);
        }
        if (ageMillis < 50)
        {
          return 1.0f;
        }
        if (ageMillis < 60)
        {
          return static_cast<float>(0.5 + (60 - ageMillis) * 0.05);
        }
        return 0.5f;
      }
    case LeastSquaresVelocityTrackerStrategy::Weighting::Recent:
      {
        // Weight points based on their age, weighing older points less.
        //   age   0ms: 1.0
        //   age  50ms: 1.0
        //   age 100ms: 0.5
        double ageMillis = (m_movements[size - 1].EventTime - m_movements[index].EventTime).TotalMilliseconds();
        if (ageMillis < 50)
        {
          return 1.0f;
        }
        if (ageMillis < 100)
        {
          return static_cast<float>(0.5 + (100 - ageMillis) * 0.01);
        }
        return 0.5f;
      }
    case LeastSquaresVelocityTrackerStrategy::Weighting::Disabled:
    default:
      return 1.0f;
    }
  }


  bool LeastSquaresVelocityTrackerStrategy::TrySolveUnweightedLeastSquaresDeg2(const CircularFixedSizeBuffer<MovementRecord>& movements,
                                                                               DpValueF& rResultDpf) noexcept
  {
    // Solving y = a*x^2 + b*x + c, where
    //      - "x" is age (i.e. duration since latest movement) of the movements
    //      - "y" is positions of the movements.
    float sxi = 0;
    float sxiyi = 0;
    float syi = 0;
    float sxi2 = 0;
    float sxi3 = 0;
    float sxi2yi = 0;
    float sxi4 = 0;

    const auto countF = static_cast<float>(movements.size());
    const MovementRecord newestMovement = movements.back();
    for (std::size_t i = 0; i < movements.size(); ++i)
    {
      const MovementRecord& movement = movements[i];
      TimeSpan age = newestMovement.EventTime - movement.EventTime;
      auto xi = static_cast<float>(-age.TotalSeconds());
      float yi = movement.PositionDpf.Value;
      float xi2 = xi * xi;
      float xi3 = xi2 * xi;
      float xi4 = xi3 * xi;
      float xiyi = xi * yi;
      float xi2yi = xi2 * yi;
      sxi += xi;
      sxi2 += xi2;
      sxiyi += xiyi;
      sxi2yi += xi2yi;
      syi += yi;
      sxi3 += xi3;
      sxi4 += xi4;
    }

    float sxx = sxi2 - sxi * sxi / countF;
    float sxy = sxiyi - sxi * syi / countF;
    float sxx2 = sxi3 - sxi * sxi2 / countF;
    float sx2y = sxi2yi - sxi2 * syi / countF;
    float sx2x2 = sxi4 - sxi2 * sxi2 / countF;
    float denominator = sxx * sx2x2 - sxx2 * sxx2;
    if (denominator == 0)
    {
      FSLLOG3_WARNING("division by 0 when computing velocity, Sxx={}, Sx2x2={}, Sxx2={}", sxx, sx2x2, sxx2);
      rResultDpf = {};
      return false;
    }
    rResultDpf = DpValueF(((sxy * sx2x2) - (sx2y * sxx2)) / denominator);
    return true;
  }

  /**
   * Solves a linear least squares problem to obtain a N degree polynomial that fits
   * the specified input data as nearly as possible.
   *
   * Returns true if a solution is found, false otherwise.
   *
   * The input consists of two vectors of data points X and Y with indices 0..m-1
   * along with a weight vector W of the same size.
   *
   * The output is a vector B with indices 0..n that describes a polynomial
   * that fits the data, such the sum of W[i] * W[i] * abs(Y[i] - (B[0] + B[1] X[i]
   * + B[2] X[i]^2 ... B[n] X[i]^n)) for all i between 0 and m-1 is minimized.
   *
   * Accordingly, the weight vector W should be initialized by the caller with the
   * reciprocal square root of the variance of the error in each input data point.
   * In other words, an ideal choice for W would be W[i] = 1 / var(Y[i]) = 1 / stddev(Y[i]).
   * The weights express the relative importance of each data point.  If the weights are
   * all 1, then the data points are considered to be of equal importance when fitting
   * the polynomial.  It is a good idea to choose weights that diminish the importance
   * of data points that may have higher than usual error margins.
   *
   * Errors among data points are assumed to be independent.  W is represented here
   * as a vector although in the literature it is typically taken to be a diagonal matrix.
   *
   * That is to say, the function that generated the input data can be approximated
   * by y(x) ~= B[0] + B[1] x + B[2] x^2 + ... + B[n] x^n.
   *
   * The coefficient of determination (R^2) is also returned to describe the goodness
   * of fit of the model for the given data.  It is a value between 0 and 1, where 1
   * indicates perfect correspondence.
   *
   * This function first expands the X vector to a m by n matrix A such that
   * A[i][0] = 1, A[i][1] = X[i], A[i][2] = X[i]^2, ..., A[i][n] = X[i]^n, then
   * multiplies it by w[i]./
   *
   * Then it calculates the QR decomposition of A yielding an m by m orthonormal matrix Q
   * and an m by n upper triangular matrix R.  Because R is upper triangular (lower
   * part is all zeroes), we can simplify the decomposition into an m by n matrix
   * Q1 and a n by n matrix R1 such that A = Q1 R1.
   *
   * Finally we solve the system of linear equations given by R1 B = (Qtranspose W Y)
   * to find B.
   *
   * For efficiency, we lay out A and Q column-wise in memory because we frequently
   * operate on the column vectors.  Conversely, we lay out R row-wise.
   *
   * http://en.wikipedia.org/wiki/Numerical_methods_for_linear_least_squares
   * http://en.wikipedia.org/wiki/Gram-Schmidt
   */
  // source: x=time, y=positions, w=weight
  bool LeastSquaresVelocityTrackerStrategy::TrySolveLeastSquares(const ReadOnlySpan<Record> span, const uint32_t n, DpValueF& rResultDpf) noexcept
  {
    const std::size_t m = span.size();
    assert(m <= LocalConfig::MaxM);
    assert(n <= LocalConfig::MaxN);

    // Expand the X vector to a matrix A, pre-multiplied by the weights.
    std::array<float, LocalConfig::MaxMatrixElements> tmpA{};    // column-major order
    Span<float> spanA = SpanUtil::UncheckedAsSpan(tmpA, 0, n * m);
    for (uint32_t h = 0; h < m; ++h)
    {
      spanA[h] = span[h].W;
      for (uint32_t i = 1; i < n; ++i)
      {
        spanA[(i * m) + h] = spanA[((i - 1) * m) + h] * span[h].Time;
      }
    }

    // Apply the Gram-Schmidt process to A to obtain its QR decomposition.
    std::array<float, LocalConfig::MaxMatrixElements> tmpQ{};           // orthonormal basis, column-major order
    std::array<float, LocalConfig::MaxN * LocalConfig::MaxN> tmpR{};    // upper triangular matrix, row-major order
    Span<float> spanQ = SpanUtil::UncheckedAsSpan(tmpQ, 0, n * m);
    Span<float> spanR = SpanUtil::UncheckedAsSpan(tmpR, 0, n * n);
    for (uint32_t j = 0; j < n; ++j)
    {
      for (uint32_t h = 0; h < m; ++h)
      {
        spanQ[(j * m) + h] = spanA[(j * m) + h];
      }
      for (uint32_t i = 0; i < j; ++i)
      {
        const float dot = VectorDot(spanQ.unchecked_subspan(j * m, m), spanQ.unchecked_subspan(i * m, m));
        for (uint32_t h = 0; h < m; ++h)
        {
          spanQ[(j * m) + h] -= dot * spanQ[(i * m) + h];
        }
      }
      const float norm = VectorNorm(spanQ.unchecked_subspan(j * m, m));
      if (norm < 0.000001f)
      {
        // vectors are linearly dependent or zero so no solution
        rResultDpf = {};
        return false;
      }
      const float invNorm = 1.0f / norm;
      for (uint32_t h = 0; h < m; ++h)
      {
        spanQ[(j * m) + h] *= invNorm;
      }
      for (uint32_t i = 0; i < n; ++i)
      {
        spanR[(j * n) + i] = i < j ? 0 : VectorDot(spanQ.unchecked_subspan(j * m, m), spanA.unchecked_subspan(i * m, m));
      }
    }

    // Solve R B = Qt W Y to find B.  This is easy because R is upper triangular.
    // We just work from bottom-right to top-left calculating B's coefficients.
    std::array<float, LocalConfig::MaxM> tmpWY{};
    Span<float> spanWY = SpanUtil::UncheckedAsSpan(tmpWY, 0, m);
    for (uint32_t h = 0; h < m; ++h)
    {
      spanWY[h] = span[h].Position.Value * span[h].W;
    }

    std::array<float, LocalConfig::MaxN> outB{};
    for (uint32_t i = n; i != 0;)
    {
      --i;
      outB[i] = VectorDot(spanQ.unchecked_subspan(i * m, m), spanWY);
      for (uint32_t j = n - 1; j > i; --j)
      {
        outB[i] -= spanR[(i * n) + j] * outB[j];
      }
      outB[i] /= spanR[(i * n) + i];
    }

    rResultDpf = DpValueF(outB[1]);
    return true;
  }
}

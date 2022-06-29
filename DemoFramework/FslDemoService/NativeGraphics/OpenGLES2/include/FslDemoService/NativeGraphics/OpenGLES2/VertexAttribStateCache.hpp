#ifndef FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_VERTEXATTRIBSTATECACHE_HPP
#define FSLDEMOSERVICE_NATIVEGRAPHICS_OPENGLES2_VERTEXATTRIBSTATECACHE_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribConfig.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexAttribState.hpp>
#include <FslDemoService/NativeGraphics/OpenGLES2/VertexElementAttribLinks.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <GLES2/gl2.h>
#include <array>
#include <cstdint>

namespace Fsl::GLES2
{
  template <typename TVertexAttribStateFunctor>
  class VertexAttribStateCache
  {
    struct VertexAttribStateEntry
    {
      GLuint AttribIndex{0};
      VertexAttribState OldState;
      VertexAttribState CurrentState;

      VertexAttribStateEntry() noexcept = default;
      inline VertexAttribStateEntry(const GLuint attribIndex, const VertexAttribState& oldState, const VertexAttribState& currentState) noexcept
        : AttribIndex(attribIndex)
        , OldState(oldState)
        , CurrentState(currentState)
      {
      }
    };

    //! Vertex attribs is cached on demand
    //! The entries are always stored sorted by AttribIndex (low to high)
    std::array<VertexAttribStateEntry, VertexAttribConfig::MaxAttribCount> m_vertexAttribs{};

    //! The number of cached entries in vertex attribs
    uint16_t m_count{0};

  public:
    VertexAttribStateCache(const VertexAttribStateCache&) = delete;
    VertexAttribStateCache& operator=(const VertexAttribStateCache&) = delete;

    VertexAttribStateCache& operator=(VertexAttribStateCache&& other) noexcept
    {
      if (this != &other)
      {
        if (m_count > 0)
        {
          Reset();
        }
        m_vertexAttribs = other.m_vertexAttribs;
        m_count = other.m_count;
        other.m_vertexAttribs = {};
        other.m_count = {};
      }
      return *this;
    }

    VertexAttribStateCache(VertexAttribStateCache&& other) noexcept
      : m_vertexAttribs(other.m_vertexAttribs)
      , m_count(other.m_count)
    {
      other.m_vertexAttribs = {};
      other.m_count = 0;
    }


    VertexAttribStateCache() = default;

    ~VertexAttribStateCache() noexcept
    {
      Reset();
    }

    void ChangeAttribs(const VertexElementAttribLinks& attribs)
    {
      auto vertexStride = attribs.VertexStride();
      auto span = attribs.AsSpan();

      // We expect the max entries in VertexElementAttribLinks will be able to fit inside our cache
      assert(span.size() <= m_vertexAttribs.size());

      if (m_count <= 0)
      {
        // No existing materials so we just insert
        assert(m_count == 0u);
        {
          const auto count = UncheckedNumericCast<uint16_t>(span.size());
          for (uint16_t i = 0; i < count; ++i)
          {
            const auto& entry = span[i];
            AddAttrib(entry.AttribIndex, ToVertexAttribState(entry, vertexStride));
          }
        }
        assert(span.size() == m_count);
      }
      else
      {
        // When changing attribs we use a two pass algorithm.
        // 1. Prune -> Figure out which indices are not used anymore and restore them to their old state
        // 2. Update

        // Since both VertexElementAttribLinks and m_vertexAttribs are sorted list (attribIndex as key, low to high) we can take advantage of that
        // when determining what attribs were changed

        {    // 1. Prune
          assert(m_count <= m_vertexAttribs.size());
          const auto currentCount = UncheckedNumericCast<int32_t>(m_count);
          int32_t lastFindIndex = UncheckedNumericCast<int32_t>(span.size()) - 1;
          for (int32_t i = currentCount - 1; i >= 0; --i)
          {
            const auto findAttribIndex = m_vertexAttribs[i].AttribIndex;
            while (lastFindIndex >= 0 && span[lastFindIndex].AttribIndex > findAttribIndex)
            {
              --lastFindIndex;
            }
            if (lastFindIndex < 0 || span[lastFindIndex].AttribIndex != findAttribIndex)
            {
              RemoveAttribAt(UncheckedNumericCast<uint16_t>(i));
            }
          }
        }

        assert(span.size() >= m_count);
        {    // 2. Update to match, this means that the m_vertexAttribs[i] entries will end up a 1 to 1 order match of the span
          const auto count = UncheckedNumericCast<uint16_t>(span.size());
          for (uint16_t i = 0; i < count; ++i)
          {
            const auto& entry = span[i];
            if (i < m_count && entry.AttribIndex == m_vertexAttribs[i].AttribIndex)
            {
              UpdateAttribAt(i, ToVertexAttribState(entry, vertexStride));
            }
            else
            {
              assert(i >= m_count || entry.AttribIndex < m_vertexAttribs[i].AttribIndex);
              InsertAttribAt(i, entry.AttribIndex, ToVertexAttribState(entry, vertexStride));
            }
          }
        }
        assert(span.size() == m_count);
      }
    }

  private:
    static inline VertexAttribState ToVertexAttribState(const GLVertexElementAttribConfig& entry, const GLint vertexStride) noexcept
    {
      return {true, {entry.Size, UncheckedNumericCast<GLint>(entry.Type), entry.Normalized != GL_FALSE, vertexStride, entry.Pointer}};
    }


    static inline void SetAttribIfChanged(const GLuint attribIndex, const VertexAttribState& currentState, const VertexAttribState& newState) noexcept
    {
      // When a buffer is new buffer is bound its not enough to check that the state was not changed
      // if (currentState.Basic != newState.Basic)
      {
        TVertexAttribStateFunctor::SetBasicAttrib(attribIndex, newState.Basic);
      }
      if (currentState.Enabled != newState.Enabled)
      {
        TVertexAttribStateFunctor::SetAttribEnabled(attribIndex, newState.Enabled);
      }
    }


    void RemoveAttribAt(const uint16_t cacheIndex) noexcept
    {
      assert(cacheIndex < m_count);
      // Restore the attrib settings if it was modified
      {
        const auto& entry = m_vertexAttribs[cacheIndex];
        // On remove we want to restore the old state
        SetAttribIfChanged(entry.AttribIndex, entry.CurrentState, entry.OldState);
      }

      // Remove the entry from the array
      assert(m_count > 0);
      const uint32_t lastIndex = m_count - 1;
      for (std::size_t i = cacheIndex; i < lastIndex; ++i)
      {
        m_vertexAttribs[i] = m_vertexAttribs[i + 1];
      }
      m_vertexAttribs[lastIndex] = {};
      m_count = lastIndex;
    }

    void UpdateAttribAt(const uint16_t cacheIndex, const VertexAttribState vertexAttribState) noexcept
    {
      assert(cacheIndex < m_count);
      auto& rEntry = m_vertexAttribs[cacheIndex];
      {
        // Update the current state to the newly supplied one
        SetAttribIfChanged(rEntry.AttribIndex, rEntry.CurrentState, vertexAttribState);
        rEntry.CurrentState = vertexAttribState;
      }
    }

    void InsertAttribAt(const uint16_t cacheIndex, const GLuint attribIndex, const VertexAttribState vertexAttribState) noexcept
    {
      assert(m_count < m_vertexAttribs.size());
      assert(cacheIndex <= m_count);
      // Make room for the attribute
      if (cacheIndex < m_count)
      {
        for (std::size_t i = m_count; i > cacheIndex; --i)
        {
          m_vertexAttribs[i] = m_vertexAttribs[i - 1];
        }
      }
      m_vertexAttribs[cacheIndex] = VertexAttribStateEntry(attribIndex, TVertexAttribStateFunctor::GetAttrib(attribIndex), vertexAttribState);
      // Since this is a new entry the current state is actually in 'old', and the setting we want is in 'current'
      SetAttribIfChanged(attribIndex, m_vertexAttribs[cacheIndex].OldState, m_vertexAttribs[cacheIndex].CurrentState);

      ++m_count;
      assert(m_count <= m_vertexAttribs.size());
    }

    void AddAttrib(const GLuint attribIndex, const VertexAttribState vertexAttribState) noexcept
    {
      assert(m_count < m_vertexAttribs.size());
      m_vertexAttribs[m_count] = VertexAttribStateEntry(attribIndex, TVertexAttribStateFunctor::GetAttrib(attribIndex), vertexAttribState);
      // Since this is a new entry the current state is actually in 'old', and the setting we want is in 'current'
      SetAttribIfChanged(attribIndex, m_vertexAttribs[m_count].OldState, m_vertexAttribs[m_count].CurrentState);
      ++m_count;
      assert(m_count <= m_vertexAttribs.size());
    }


    void Reset() noexcept
    {
      // Restore changed vertex-attribs
      for (uint16_t i = 0; i < m_count; ++i)
      {
        auto& rEntry = m_vertexAttribs[i];
        // Restore any changes from the current state to match the old state
        SetAttribIfChanged(rEntry.AttribIndex, rEntry.CurrentState, rEntry.OldState);
        rEntry = {};
      }
      m_count = 0;
    }
  };
}

#endif

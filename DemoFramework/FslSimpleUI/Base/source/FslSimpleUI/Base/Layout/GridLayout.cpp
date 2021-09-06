/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslSimpleUI/Base/Layout/GridLayout.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl
{
  namespace UI
  {
    namespace LocalConfig
    {
      constexpr const uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();
      constexpr const uint32_t MaxLoopCount = 4;
    }

    GridLayout::GridLayout(const std::shared_ptr<BaseWindowContext>& context)
      : ComplexLayout<GridWindowCollectionRecord>(context)
    {
      // Set up a "dummy" entry so that we can handle measuring without columns and rows easily.
      m_definitionsX.emplace_back(GridColumnDefinition(GridUnitType::Auto));
      m_definitionsY.emplace_back(GridRowDefinition(GridUnitType::Auto));
    }


    void GridLayout::AddColumnDefinition(const GridColumnDefinition& definition)
    {
      if (!m_definitionCache.HasEntriesX)
      {
        assert(m_definitionsX.size() == 1u);
        m_definitionsX.clear();
        m_definitionCache.HasEntriesX = true;
      }

      m_definitionsX.emplace_back(definition);
    }

    void GridLayout::AddRowDefinition(const GridRowDefinition& definition)
    {
      if (!m_definitionCache.HasEntriesY)
      {
        assert(m_definitionsY.size() == 1u);
        m_definitionsY.clear();
        m_definitionCache.HasEntriesY = true;
      }

      m_definitionsY.emplace_back(definition);
    }


    void GridLayout::SetColumn(const std::shared_ptr<BaseWindow>& window, const uint32_t index)
    {
      if (index >= m_definitionsX.size())
      {
        throw std::invalid_argument("index out of bounds");
      }

      auto itrFind = std::find_if(begin(), end(), [window](const GridWindowCollectionRecord& record) { return record.Window == window; });
      if (itrFind == end())
      {
        throw std::invalid_argument("window not a child of this layout");
      }

      if (itrFind->IndexX != index)
      {
        itrFind->IndexX = index;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void GridLayout::SetRow(const std::shared_ptr<BaseWindow>& window, const uint32_t index)
    {
      if (index >= m_definitionsY.size())
      {
        throw std::invalid_argument("index out of bounds");
      }
      auto itrFind = std::find_if(begin(), end(), [window](const GridWindowCollectionRecord& record) { return record.Window == window; });
      if (itrFind == end())
      {
        throw std::invalid_argument("window not a child of this layout");
      }

      if (itrFind->IndexY != index)
      {
        itrFind->IndexY = index;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void GridLayout::Set(const std::shared_ptr<BaseWindow>& window, const uint32_t columnIndex, const uint32_t rowIndex)
    {
      if (columnIndex >= m_definitionsX.size())
      {
        throw std::invalid_argument(fmt::format("columnIndex {} out of bounds", columnIndex));
      }
      if (rowIndex >= m_definitionsY.size())
      {
        throw std::invalid_argument(fmt::format("rowIndex {} out of bounds", rowIndex));
      }
      auto itrFind = std::find_if(begin(), end(), [window](const GridWindowCollectionRecord& record) { return record.Window == window; });
      if (itrFind == end())
      {
        throw std::invalid_argument("window not a child of this layout");
      }

      if (itrFind->IndexX != columnIndex || itrFind->IndexY != rowIndex)
      {
        itrFind->IndexX = columnIndex;
        itrFind->IndexY = rowIndex;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    void GridLayout::SetLimitToAvailableSpace(const bool enabled)
    {
      if (m_limitToAvailableSpace != enabled)
      {
        m_limitToAvailableSpace = enabled;
        PropertyUpdated(PropertyType::Layout);
      }
    }


    // void GridLayout::ClearColumnDefinitions()
    //{
    //  m_columnDefinitions.clear();
    //  m_definitionCache.HasEntriesX = false;
    //  m_definitionsX.push_back(GridRowColumnDefinitionEx(GridColumnDefinition(GridUnitType::Auto)));

    //  // FIX: clear any control 'set' to this column
    //  throw NotImplementedException("ClearColumnDefinitions");
    //}


    // void GridLayout::ClearRowDefinitions()
    //{
    //  m_columnDefinitions.clear();
    //  m_definitionCache.HasEntriesY = false;
    //  m_definitionsY.push_back(GridRowColumnDefinitionEx(GridRowDefinition(GridUnitType::Auto)));

    //  // FIX: clear any control 'set' to this row
    //  throw NotImplementedException("ClearRowDefinitions");
    //}


    PxSize2D GridLayout::MeasureOverride(const PxAvailableSize& availableSizePx)
    {
      const SpriteUnitConverter& unitConverter = GetContext()->UnitConverter;

      const bool isInfX = availableSizePx.IsInfinityWidth();
      const bool isInfY = availableSizePx.IsInfinityHeight();

      // Update the definition cache to ensure the 'internal definition values' match our current available size etc
      m_definitionCache.HasStarX = UpdateDefinitionCache(unitConverter, m_definitionsX, isInfX);
      m_definitionCache.HasStarY = UpdateDefinitionCache(unitConverter, m_definitionsY, isInfY);

      // Classify each window cell based on which group it belongs to
      ClassifyWindowCell();
      // Measure the simplest group
      MeasureCellGroup0(unitConverter, m_cellInfo.FirstIndexCellGroup0);

      if (!m_cellInfo.Group2HasAutoMembers)
      {
        BasicMeasure(unitConverter, availableSizePx);
      }
      else
      {
        if (m_cellInfo.FirstIndexCellGroup1 >= m_cellRecords.size())
        {
          SemiComplexMeasure(unitConverter, availableSizePx);
        }
        else
        {
          ComplexMeasure(unitConverter, availableSizePx);
        }
      }

      MeasureCellGroup3(unitConverter, m_cellInfo.FirstIndexCellGroup3, availableSizePx);

      auto resolvedSize = ResolveMeasureSize();
      return !m_limitToAvailableSpace
               ? resolvedSize
               : PxSize2D(std::min(availableSizePx.Width(), resolvedSize.Width()), std::min(availableSizePx.Height(), resolvedSize.Height()));
    }


    PxSize2D GridLayout::ArrangeOverride(const PxSize2D& finalSizePx)
    {
      PxSize2D sizePx = FinalizeSizes(finalSizePx);

      for (uint32_t index = 0; index < m_cellRecords.size(); ++index)
      {
        const auto& cell = m_cellRecords[index];
        assert(cell.IndexX <= m_definitionsX.size());
        assert(cell.IndexY <= m_definitionsY.size());

        assert(PxAvailableSizeUtil::IsNormalValue(m_definitionsX[cell.IndexX].TempValue));
        assert(PxAvailableSizeUtil::IsNormalValue(m_definitionsY[cell.IndexY].TempValue));
        assert(PxAvailableSizeUtil::IsNormalValue(m_definitionsX[cell.IndexX].ArrangeMinimumSizePx));
        assert(PxAvailableSizeUtil::IsNormalValue(m_definitionsY[cell.IndexY].ArrangeMinimumSizePx));

        PxRectangle rectPx(m_definitionsX[cell.IndexX].TempValue, m_definitionsY[cell.IndexY].TempValue,
                           m_definitionsX[cell.IndexX].ArrangeMinimumSizePx, m_definitionsY[cell.IndexY].ArrangeMinimumSizePx);

        ChildAt(index)->Arrange(rectPx);
      }
      return !m_limitToAvailableSpace ? sizePx
                                      : PxSize2D(std::min(finalSizePx.Width(), sizePx.Width()), std::min(finalSizePx.Height(), sizePx.Height()));
    }


    void GridLayout::BasicMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx)
    {
      assert(!m_cellInfo.Group2HasAutoMembers);

      // No "star X auto" members, so we can do a simple resolve of the "y-stars"
      if (m_definitionCache.HasStarY)
      {
        assert(availableSizePx.IsNormalHeight());
        ResolveStars(m_definitionsY, availableSizePx.Height());
      }
      MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSizePx, false, false);
      if (m_definitionCache.HasStarX)
      {
        assert(availableSizePx.IsNormalWidth());
        ResolveStars(m_definitionsX, availableSizePx.Width());
      }
      MeasureCellGroup2(unitConverter, m_cellInfo.FirstIndexCellGroup2, availableSizePx);
    }


    void GridLayout::SemiComplexMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx)
    {
      assert(m_cellInfo.Group2HasAutoMembers);
      assert(m_cellInfo.FirstIndexCellGroup1 >= m_cellRecords.size());

      // There are no elements in group1 elements so we can do a simple resolve
      if (m_definitionCache.HasStarX)
      {
        assert(availableSizePx.IsNormalWidth());
        ResolveStars(m_definitionsX, availableSizePx.Width());
      }
      MeasureCellGroup2(unitConverter, m_cellInfo.FirstIndexCellGroup2, availableSizePx);
      if (m_definitionCache.HasStarY)
      {
        assert(availableSizePx.IsNormalHeight());
        ResolveStars(m_definitionsY, availableSizePx.Height());
      }
    }


    void GridLayout::ComplexMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx)
    {
      assert(m_cellInfo.Group2HasAutoMembers);
      assert(m_cellInfo.FirstIndexCellGroup1 < m_cellRecords.size());

      StoreMinSizeX(m_cellInfo.FirstIndexCellGroup1);
      StoreMinSizeY(m_cellInfo.FirstIndexCellGroup2);
      bool hasXChanges = false;
      MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSizePx, true, false);
      uint32_t retryCount = 0;
      do
      {
        if (hasXChanges)
        {
          // restore min sizes for g2
          RestoreMinSize(m_definitionsY);
        }

        if (m_definitionCache.HasStarX)
        {
          assert(availableSizePx.IsNormalWidth());
          ResolveStars(m_definitionsX, availableSizePx.Width());
        }
        MeasureCellGroup2(unitConverter, m_cellInfo.FirstIndexCellGroup2, availableSizePx);

        // restore min sizes for g1
        RestoreMinSize(m_definitionsX);

        if (m_definitionCache.HasStarY)
        {
          assert(availableSizePx.IsNormalHeight());
          ResolveStars(m_definitionsY, availableSizePx.Height());
        }
        hasXChanges = MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSizePx, false, retryCount == LocalConfig::MaxLoopCount);

        ++retryCount;
      } while (hasXChanges && retryCount <= LocalConfig::MaxLoopCount);
    }


    PxSize2D GridLayout::ResolveMeasureSize()
    {
      int32_t finalWidth = 0;
      for (const auto& entry : m_definitionsX)
      {
        finalWidth += entry.MinimumSizePx;
      }
      int32_t finalHeight = 0;
      for (const auto& entry : m_definitionsY)
      {
        finalHeight += entry.MinimumSizePx;
      }

      return {finalWidth, finalHeight};
    }


    bool GridLayout::UpdateDefinitionCache(const SpriteUnitConverter& unitConverter, std::deque<GridRowColumnDefinitionEx>& rDefinitions,
                                           const bool treatStarAsAuto)
    {
      bool hasStar = false;
      for (GridRowColumnDefinitionEx& rDef : rDefinitions)
      {
        int32_t userMinSizePx = 0;
        int32_t userSizePx = 0;
        switch (rDef.Unit)
        {
        case GridUnitType::Fixed:
          rDef.MeasureUnitType = InternalGridUnitType::Fixed;
          userSizePx = unitConverter.DpToPxInt32(rDef.Size);
          userMinSizePx = userSizePx;
          break;
        case GridUnitType::FixedPx:
          rDef.MeasureUnitType = InternalGridUnitType::Fixed;
          userSizePx = unitConverter.PxfToPxInt32(rDef.Size);
          userMinSizePx = userSizePx;
          break;
        case GridUnitType::Auto:
          rDef.MeasureUnitType = InternalGridUnitType::Auto;
          userSizePx = PxAvailableSizeUtil::InfiniteSpacePx;
          break;
        case GridUnitType::Star:
          rDef.MeasureUnitType = !treatStarAsAuto ? InternalGridUnitType::Star : InternalGridUnitType::AutoStar;
          userSizePx = PxAvailableSizeUtil::InfiniteSpacePx;
          hasStar = !treatStarAsAuto;
          break;
        default:
          FSLLOG3_WARNING("Unsupported unit type: {}", static_cast<uint32_t>(rDef.Unit));
          break;
        }
        rDef.MeasureSizePx = std::max(userSizePx, userMinSizePx);
        rDef.MinimumSizePx = userMinSizePx;
        rDef.TempValue = 0;
      }
      return hasStar;
    }


    void GridLayout::ClassifyWindowCell()
    {
      if (size() >= m_cellRecords.size())
      {
        m_cellRecords.resize(size());
      }

      m_cellInfo.FirstIndexCellGroup0 = LocalConfig::InvalidIndex;
      m_cellInfo.FirstIndexCellGroup1 = LocalConfig::InvalidIndex;
      m_cellInfo.FirstIndexCellGroup2 = LocalConfig::InvalidIndex;
      m_cellInfo.FirstIndexCellGroup3 = LocalConfig::InvalidIndex;
      bool group2HasAutoMembers = false;

      auto itr = begin();
      const auto itrEnd = end();
      if (itr != itrEnd)
      {
        uint32_t index = 0;
        while (itr != itrEnd)
        {
          WindowCellRecord cell{};
          cell.IndexX = itr->IndexX;
          cell.IndexY = itr->IndexY;

          assert(cell.IndexX <= m_definitionsX.size());
          assert(cell.IndexY <= m_definitionsY.size());

          cell.UnitTypeFlagsX = m_definitionsX[cell.IndexX].Unit;
          cell.UnitTypeFlagsY = m_definitionsY[cell.IndexY].Unit;

          //       | Fixed | Auto | Star |
          // Fixed | 0     | 0    | 2    |
          // Auto  | 0     | 0    | 2    |
          // Star  | 3     | 1    | 3    |

          if (cell.UnitTypeFlagsY != GridUnitType::Star)
          {
            if (cell.UnitTypeFlagsX != GridUnitType::Star)
            {
              cell.NextIndex = m_cellInfo.FirstIndexCellGroup0;
              m_cellInfo.FirstIndexCellGroup0 = index;
            }
            else
            {
              cell.NextIndex = m_cellInfo.FirstIndexCellGroup2;
              m_cellInfo.FirstIndexCellGroup2 = index;
              group2HasAutoMembers |= (cell.UnitTypeFlagsY == GridUnitType::Auto);
            }
          }
          else
          {
            if (cell.UnitTypeFlagsX == GridUnitType::Auto)
            {
              cell.NextIndex = m_cellInfo.FirstIndexCellGroup1;
              m_cellInfo.FirstIndexCellGroup1 = index;
            }
            else
            {
              cell.NextIndex = m_cellInfo.FirstIndexCellGroup3;
              m_cellInfo.FirstIndexCellGroup3 = index;
            }
          }

          m_cellRecords[index] = cell;
          ++itr;
          ++index;
        }
      }
      m_cellInfo.Group2HasAutoMembers = group2HasAutoMembers;
    }


    // As cell group zero only contains Fixed and auto we just measure them directly
    //       | Fixed | Auto | Star |
    // Fixed | 0     | 0    | 2    |
    // Auto  | 0     | 0    | 2    |
    // Star  | 3     | 1    | 3    |

    void GridLayout::MeasureCellGroup0(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex)
    {
      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto || m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Fixed ||
               m_cellRecords[index].UnitTypeFlagsX == GridUnitType::FixedPx);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto || m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Fixed ||
               m_cellRecords[index].UnitTypeFlagsY == GridUnitType::FixedPx);

        auto window = ChildAt(index);
        assert(window);

        const uint32_t indexX = m_cellRecords[index].IndexX;
        const uint32_t indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        const PxAvailableSize avail(
          m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto ? PxAvailableSizeUtil::InfiniteSpacePx : m_definitionsX[indexX].MeasureSizePx,
          m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto ? PxAvailableSizeUtil::InfiniteSpacePx : m_definitionsY[indexY].MeasureSizePx);

        // Finally measure the cell
        window->Measure(avail);

        assert(m_definitionsX[indexX].MeasureUnitType != InternalGridUnitType::Fixed ||
               (unitConverter.DpToPxInt32(m_definitionsX[indexX].Size) == m_definitionsX[indexX].MinimumSizePx ||
                unitConverter.PxfToPxInt32(m_definitionsX[indexX].Size) == m_definitionsX[indexX].MinimumSizePx));
        assert(m_definitionsY[indexY].MeasureUnitType != InternalGridUnitType::Fixed ||
               (unitConverter.DpToPxInt32(m_definitionsY[indexY].Size) == m_definitionsY[indexY].MinimumSizePx ||
                unitConverter.PxfToPxInt32(m_definitionsY[indexY].Size) == m_definitionsY[indexY].MinimumSizePx));

        // Apply the measured window dimensions to the minimum cell size
        PxSize2D desiredSizePx = window->DesiredSizePx();
        if (m_definitionsX[indexX].MeasureUnitType != InternalGridUnitType::Fixed)
        {
          m_definitionsX[indexX].ApplyMeasureMinSize(desiredSizePx.Width());
        }
        if (m_definitionsY[indexY].MeasureUnitType != InternalGridUnitType::Fixed)
        {
          m_definitionsY[indexY].ApplyMeasureMinSize(desiredSizePx.Height());
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    //! Group1 cells are all of the type "auto X star"
    //       | Fixed | Auto | Star |
    // Fixed | 0     | 0    | 2    |
    // Auto  | 0     | 0    | 2    |
    // Star  | 3     | 1    | 3    |

    bool GridLayout::MeasureCellGroup1(const uint32_t groupStartIndex, const PxAvailableSize& /*availableSizePx*/, const bool useInfinityY,
                                       const bool ignoreDesiredX)
    {
      bool widthModified = false;
      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());

      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Star);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        const PxAvailableSize avail(PxAvailableSizeUtil::InfiniteSpacePx,
                                    !useInfinityY ? m_definitionsY[indexY].MeasureSizePx : PxAvailableSizeUtil::InfiniteSpacePx);

        int32_t oldWidthPx = window->DesiredSizePx().Width();
        // Finally measure the cell
        window->Measure(avail);

        widthModified |= (oldWidthPx != window->DesiredSizePx().Width());

        // Apply the measured window dimensions to the minimum cell size
        PxSize2D desiredSizePx = window->DesiredSizePx();
        if (!ignoreDesiredX)
        {
          m_definitionsX[indexX].ApplyMeasureMinSize(desiredSizePx.Width());
        }
        if (!useInfinityY)
        {
          m_definitionsY[indexY].ApplyMeasureMinSize(desiredSizePx.Height());
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
      return widthModified;
    }

    //! Group2 cells are all of the type "star X fixed", "star x auto"
    //       | Fixed | Auto | Star |
    // Fixed | 0     | 0    | 2    |
    // Auto  | 0     | 0    | 2    |
    // Star  | 3     | 1    | 3    |
    void GridLayout::MeasureCellGroup2(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex,
                                       const PxAvailableSize& /*availableSizePx*/)
    {
      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Star);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Fixed || m_cellRecords[index].UnitTypeFlagsY == GridUnitType::FixedPx ||
               m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        const PxAvailableSize avail(m_definitionsX[indexX].MeasureSizePx, m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto
                                                                            ? PxAvailableSizeUtil::InfiniteSpacePx
                                                                            : m_definitionsY[indexY].MeasureSizePx);

        // Finally measure the cell
        window->Measure(avail);

        assert(m_definitionsY[indexY].MeasureUnitType != InternalGridUnitType::Fixed ||
               (unitConverter.DpToPxInt32(m_definitionsY[indexY].Size) == m_definitionsY[indexY].MinimumSizePx ||
                unitConverter.PxfToPxInt32(m_definitionsY[indexY].Size) == m_definitionsY[indexY].MinimumSizePx));

        // Apply the measured window dimensions to the minimum cell size
        PxSize2D desiredSizePx = window->DesiredSizePx();
        m_definitionsX[indexX].ApplyMeasureMinSize(desiredSizePx.Width());
        if (m_definitionsY[indexY].MeasureUnitType != InternalGridUnitType::Fixed)
        {
          m_definitionsY[indexY].ApplyMeasureMinSize(desiredSizePx.Height());
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }

    //! Group3  cells are all of the type "star X star" or "fixed X star"
    //       | Fixed | Auto | Star |
    // Fixed | 0     | 0    | 2    |
    // Auto  | 0     | 0    | 2    |
    // Star  | 3     | 1    | 3    |
    void GridLayout::MeasureCellGroup3(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex,
                                       const PxAvailableSize& /*availableSizePx*/)
    {
      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Fixed || m_cellRecords[index].UnitTypeFlagsX == GridUnitType::FixedPx ||
               m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Star);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Star);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        const PxAvailableSize avail(m_definitionsX[indexX].MeasureSizePx, m_definitionsY[indexY].MeasureSizePx);

        // Finally measure the cell
        window->Measure(avail);

        assert(m_definitionsX[indexX].MeasureUnitType != InternalGridUnitType::Fixed ||
               (unitConverter.DpToPxInt32(m_definitionsX[indexX].Size == m_definitionsX[indexX].MinimumSizePx) ||
                unitConverter.PxfToPxInt32(m_definitionsX[indexX].Size == m_definitionsX[indexX].MinimumSizePx)));

        // Apply the measured window dimensions to the minimum cell size
        PxSize2D desiredSizePx = window->DesiredSizePx();
        if (m_definitionsX[indexX].MeasureUnitType != InternalGridUnitType::Fixed)
        {
          m_definitionsX[indexX].ApplyMeasureMinSize(desiredSizePx.Width());
        }
        m_definitionsY[indexY].ApplyMeasureMinSize(desiredSizePx.Height());

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    void GridLayout::ResolveStars(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const int32_t totalAvailableSpacePx)
    {
      assert(!PxAvailableSizeUtil::IsConsideredInfiniteSpace(totalAvailableSpacePx));

      // Figure out the total star
      float totalStar = 0.0f;
      int32_t spaceLeftPx = totalAvailableSpacePx;
      uint32_t starCount = 0;
      for (const auto& entry : rDefinitions)
      {
        if (entry.MeasureUnitType == InternalGridUnitType::Star)
        {
          assert(entry.Size >= 0.0f);
          assert(!std::isinf(entry.Size));
          // We rely on reasonable stars being used (so overflow will not be a issue)
          totalStar += entry.Size;
          ++starCount;
        }
        else
        {
          spaceLeftPx -= entry.MinimumSizePx;
        }
      }
      if (starCount > 0)
      {
        // This method should not be called if there are no stars
        // We expect the total to be in a valid range
        assert(totalStar > 0.0f);
        assert(!std::isinf(totalStar));
        spaceLeftPx = std::max(spaceLeftPx, 0);
        for (auto& rEntry : rDefinitions)
        {
          if (rEntry.MeasureUnitType == InternalGridUnitType::Star)
          {
            auto staredSpacePxf = static_cast<float>(spaceLeftPx) * (rEntry.Size / totalStar);
            rEntry.MeasureSizePx = static_cast<int32_t>(std::round(staredSpacePxf));
            rEntry.ApplyMeasureMinSize(rEntry.MeasureSizePx);
          }
        }
      }
    }

    PxSize2D GridLayout::FinalizeSizes(const PxSize2D& finalSizePx)
    {
      ResetMinimumSize(m_definitionsX);
      ResetMinimumSize(m_definitionsY);
      FinalizeStars(m_definitionsY, finalSizePx.Height());
      FinalizeStars(m_definitionsX, finalSizePx.Width());

      PxSize2D sizePx;
      {
        for (auto& rEntry : m_definitionsX)
        {
          rEntry.TempValue = sizePx.Width();
          sizePx.AddWidth(rEntry.ArrangeMinimumSizePx);
        }
        for (auto& rEntry : m_definitionsY)
        {
          rEntry.TempValue = sizePx.Height();
          sizePx.AddHeight(rEntry.ArrangeMinimumSizePx);
        }
      }
      return sizePx;
    }


    void GridLayout::ResetMinimumSize(std::deque<GridRowColumnDefinitionEx>& rDefinitions)
    {
      for (auto& rEntry : rDefinitions)
      {
        rEntry.ClearArrangeMinSize();
      }
    }


    void GridLayout::FinalizeStars(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const int32_t totalAvailableSpacePx)
    {
      assert(!PxAvailableSizeUtil::IsConsideredInfiniteSpace(totalAvailableSpacePx));

      // Figure out the total star
      float totalStar = 0.0f;
      int32_t spaceLeftPx = totalAvailableSpacePx;
      uint32_t starCount = 0;
      int32_t minimumSize = 0;
      for (const auto& entry : rDefinitions)
      {
        if ((uint32_t(entry.MeasureUnitType) & uint32_t(InternalGridUnitType::Star)) == uint32_t(InternalGridUnitType::Star))
        {
          assert(entry.Size >= 0.0f);
          assert(!std::isinf(entry.Size));
          // We rely on reasonable stars being used (so overflow will not be a issue)
          totalStar += entry.Size;
          ++starCount;
          minimumSize += entry.MinimumSizePx;
        }
        else
        {
          spaceLeftPx -= entry.MinimumSizePx;
        }
      }
      if (starCount > 0 && spaceLeftPx >= minimumSize)
      {
        // This method should not be called if there are no stars
        // We expect the total to be in a valid range
        assert(totalStar > 0.0f);
        assert(!std::isinf(totalStar));
        spaceLeftPx = std::max(spaceLeftPx, 0);

        int32_t sizePx = 0;
        for (auto& rEntry : rDefinitions)
        {
          if ((uint32_t(rEntry.MeasureUnitType) & uint32_t(InternalGridUnitType::Star)) == uint32_t(InternalGridUnitType::Star))
          {
            auto staredSpacePxf = static_cast<float>(spaceLeftPx) * (rEntry.Size / totalStar);
            rEntry.MeasureSizePx = static_cast<int32_t>(std::round(staredSpacePxf));
            sizePx += std::max(rEntry.MeasureSizePx, rEntry.MinimumSizePx);
          }
        }
        if (sizePx > spaceLeftPx)
        {
          // Due to full pixel rounding the required final size grew larger than the available pixels, so we need to reduce the size a bit.
          // The following algorithm is pretty dumb but it will produce a result that fit on screen if at all possible
          bool entriesChanged = true;
          while (sizePx > spaceLeftPx && entriesChanged)
          {
            entriesChanged = false;
            for (auto itrFix = rDefinitions.rbegin(); itrFix != rDefinitions.rend() && sizePx > spaceLeftPx; ++itrFix)
            {
              if ((uint32_t(itrFix->MeasureUnitType) & uint32_t(InternalGridUnitType::Star)) == uint32_t(InternalGridUnitType::Star) &&
                  itrFix->MeasureSizePx > itrFix->MinimumSizePx)
              {
                --sizePx;
                --itrFix->MeasureSizePx;
                entriesChanged = true;
              }
            }
          }
        }
        if (sizePx <= spaceLeftPx)
        {
          for (auto& rEntry : rDefinitions)
          {
            if ((uint32_t(rEntry.MeasureUnitType) & uint32_t(InternalGridUnitType::Star)) == uint32_t(InternalGridUnitType::Star))
            {
              rEntry.SetArrangeMinSize(rEntry.MeasureSizePx);
            }
          }
        }
      }
    }


    void GridLayout::StoreMinSizeX(const uint32_t groupStartIndex)
    {
      for (auto& rDef : m_definitionsX)
      {
        rDef.TempValue = -42;
      }

      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());

        const auto indexX = m_cellRecords[index].IndexX;
        assert(indexX < m_definitionsX.size());

        // Store the min size
        m_definitionsX[indexX].TempValue = m_definitionsX[indexX].MinimumSizePx;

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    void GridLayout::StoreMinSizeY(const uint32_t groupStartIndex)
    {
      for (auto& rDef : m_definitionsY)
      {
        rDef.TempValue = -42;
      }

      uint32_t index = groupStartIndex;
      assert(index == LocalConfig::InvalidIndex || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());

        const auto indexY = m_cellRecords[index].IndexY;
        assert(indexY < m_definitionsY.size());

        // Store the min size
        m_definitionsY[indexY].TempValue = m_definitionsY[indexY].MinimumSizePx;

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    void GridLayout::RestoreMinSize(std::deque<GridRowColumnDefinitionEx>& rDefinitions)
    {
      for (auto& rDef : rDefinitions)
      {
        if (rDef.TempValue >= 0.0f)
        {
          rDef.MinimumSizePx = rDef.TempValue;
        }
      }
    }
  }
}

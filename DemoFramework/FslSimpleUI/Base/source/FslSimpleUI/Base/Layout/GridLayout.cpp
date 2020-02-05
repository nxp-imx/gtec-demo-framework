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
#include <FslBase/Math/EqualHelper.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslSimpleUI/Base/LayoutHelper.hpp>
#include <FslSimpleUI/Base/PropertyTypeFlags.hpp>
#include <cassert>
#include <cmath>
#include <limits>


// Workaround for issues with std::isinf and std::isnan on qnx
using namespace std;

namespace Fsl
{
  namespace UI
  {
    namespace
    {
      const uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
      const uint32_t MAX_LOOP_COUNT = 4;
    }

    GridLayout::GridLayout(const std::shared_ptr<BaseWindowContext>& context)
      : ComplexLayout<GridWindowCollectionRecord>(context)
    {
      // Set up a "dummy" entry so that we can handle measuring without columns and rows easily.
      m_definitionsX.emplace_back(GridColumnDefinition(GridUnitType::Auto));
      m_definitionsY.emplace_back(GridRowDefinition(GridUnitType::Auto));
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
        throw std::invalid_argument("columnIndex out of bounds");
      }
      if (rowIndex >= m_definitionsY.size())
      {
        throw std::invalid_argument("rowIndex out of bounds");
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

    void GridLayout::AddColumnDefinition(const GridColumnDefinition& definition)
    {
      if (!m_definitionCache.HasEntriesX)
      {
        assert(m_definitionsX.size() == 1u);
        m_definitionsX.clear();
        m_definitionCache.HasEntriesX = true;
      }

      return m_definitionsX.emplace_back(definition);
    }

    void GridLayout::AddRowDefinition(const GridRowDefinition& definition)
    {
      if (!m_definitionCache.HasEntriesY)
      {
        assert(m_definitionsY.size() == 1u);
        m_definitionsY.clear();
        m_definitionCache.HasEntriesY = true;
      }

      return m_definitionsY.emplace_back(definition);
    }


    Vector2 GridLayout::ArrangeOverride(const Vector2& finalSize)
    {
      Vector2 size;
      {
        for (auto& rEntry : m_definitionsX)
        {
          rEntry.TempValue = size.X;
          size.X += rEntry.MinimumSize;
        }
        for (auto& rEntry : m_definitionsY)
        {
          rEntry.TempValue = size.Y;
          size.Y += rEntry.MinimumSize;
        }
      }


      for (uint32_t index = 0; index < m_cellRecords.size(); ++index)
      {
        const auto& cell = m_cellRecords[index];
        assert(cell.IndexX <= m_definitionsX.size());
        assert(cell.IndexY <= m_definitionsY.size());
        Rect rect(m_definitionsX[cell.IndexX].TempValue, m_definitionsY[cell.IndexY].TempValue, m_definitionsX[cell.IndexX].MinimumSize,
                  m_definitionsY[cell.IndexY].MinimumSize);

        ChildAt(index)->Arrange(rect);
      }
      return size;
    }


    Vector2 GridLayout::MeasureOverride(const Vector2& availableSize)
    {
      bool isInfX = availableSize.X >= LayoutHelper::ConsideredInfiniteSpace;
      bool isInfY = availableSize.Y >= LayoutHelper::ConsideredInfiniteSpace;

      // Update the definition cache to ensure the 'internal definition values' match our current available size etc
      m_definitionCache.HasWeightX = UpdateDefinitionCache(m_definitionsX, isInfX);
      m_definitionCache.HasWeightY = UpdateDefinitionCache(m_definitionsY, isInfY);

      // Classify each window cell based on which group it belongs to
      ClassifyWindowCell();
      // Measure the simplest group
      MeasureCellGroup0(m_cellInfo.FirstIndexCellGroup0, availableSize);

      if (!m_cellInfo.Group2HasAutoMembers)
      {
        BasicMeasure(availableSize);
      }
      else
      {
        if (m_cellInfo.FirstIndexCellGroup1 >= m_cellRecords.size())
        {
          SemiComplexMeasure(availableSize);
        }
        else
        {
          ComplexMeasure(availableSize);
        }
      }

      MeasureCellGroup3(m_cellInfo.FirstIndexCellGroup3, availableSize);

      return ResolveMeasureSize();
    }


    void GridLayout::BasicMeasure(const Vector2& availableSize)
    {
      assert(!m_cellInfo.Group2HasAutoMembers);

      // No "weight X auto" members, so we can do a simple resolve of the "y-weights"
      if (m_definitionCache.HasWeightY)
      {
        ResolveWeights(m_definitionsY, availableSize.Y);
      }
      MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSize, false, false);
      if (m_definitionCache.HasWeightX)
      {
        ResolveWeights(m_definitionsX, availableSize.X);
      }
      MeasureCellGroup2(m_cellInfo.FirstIndexCellGroup2, availableSize);
    }


    void GridLayout::SemiComplexMeasure(const Vector2& availableSize)
    {
      assert(m_cellInfo.Group2HasAutoMembers);
      assert(m_cellInfo.FirstIndexCellGroup1 >= m_cellRecords.size());

      // There are no elements in group1 elements so we can do a simple resolve
      if (m_definitionCache.HasWeightX)
      {
        ResolveWeights(m_definitionsX, availableSize.X);
      }
      MeasureCellGroup2(m_cellInfo.FirstIndexCellGroup2, availableSize);
      if (m_definitionCache.HasWeightY)
      {
        ResolveWeights(m_definitionsY, availableSize.Y);
      }
    }


    void GridLayout::ComplexMeasure(const Vector2& availableSize)
    {
      assert(m_cellInfo.Group2HasAutoMembers);
      assert(m_cellInfo.FirstIndexCellGroup1 < m_cellRecords.size());

      StoreMinSizeX(m_cellInfo.FirstIndexCellGroup1);
      StoreMinSizeY(m_cellInfo.FirstIndexCellGroup2);
      bool hasXChanges = false;
      MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSize, true, false);
      uint32_t retryCount = 0;
      do
      {
        // restore min sizes for g2
        RestoreMinSize(m_definitionsX);

        if (m_definitionCache.HasWeightX)
        {
          ResolveWeights(m_definitionsX, availableSize.X);
        }
        MeasureCellGroup2(m_cellInfo.FirstIndexCellGroup2, availableSize);

        // restore min sizes for g1
        RestoreMinSize(m_definitionsY);

        if (m_definitionCache.HasWeightY)
        {
          ResolveWeights(m_definitionsY, availableSize.Y);
        }
        hasXChanges = MeasureCellGroup1(m_cellInfo.FirstIndexCellGroup1, availableSize, false, retryCount == MAX_LOOP_COUNT);

        ++retryCount;
      } while (hasXChanges && retryCount <= MAX_LOOP_COUNT);
    }


    Vector2 GridLayout::ResolveMeasureSize()
    {
      float finalWidth = 0.0f;
      for (const auto& entry : m_definitionsX)
      {
        finalWidth += entry.MinimumSize;
      }
      float finalHeight = 0.0f;
      for (const auto& entry : m_definitionsY)
      {
        finalHeight += entry.MinimumSize;
      }

      return Vector2(finalWidth, finalHeight);
    }


    bool GridLayout::UpdateDefinitionCache(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const bool treatWeightAsAuto)
    {
      bool hasWeight = false;
      for (GridRowColumnDefinitionEx& rDef : rDefinitions)
      {
        float userMinSize = 0.0f;
        float userSize = 0.0f;
        switch (rDef.Unit)
        {
        case GridUnitType::Fixed:
          rDef.MeasureUnitType = GridUnitType::Fixed;
          userSize = rDef.Size;
          userMinSize = userSize;
          break;
        case GridUnitType::Auto:
          rDef.MeasureUnitType = GridUnitType::Auto;
          userSize = LayoutHelper::InfiniteSpace;
          break;
        case GridUnitType::Weight:
          rDef.MeasureUnitType = !treatWeightAsAuto ? GridUnitType::Weight : GridUnitType::Auto;
          userSize = LayoutHelper::InfiniteSpace;
          hasWeight = true;
          break;
        default:
          FSLLOG3_WARNING("Unsupported unit type: {}", static_cast<uint32_t>(rDef.Unit));
          break;
        }
        rDef.MeasureSize = std::max(userSize, userMinSize);
        rDef.MinimumSize = userMinSize;
      }
      return hasWeight;
    }


    void GridLayout::ClassifyWindowCell()
    {
      if (size() >= m_cellRecords.size())
      {
        m_cellRecords.resize(size());
      }

      m_cellInfo.FirstIndexCellGroup0 = INVALID_INDEX;
      m_cellInfo.FirstIndexCellGroup1 = INVALID_INDEX;
      m_cellInfo.FirstIndexCellGroup2 = INVALID_INDEX;
      m_cellInfo.FirstIndexCellGroup3 = INVALID_INDEX;
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

          //        | Fixed | Auto | Weight |
          // Fixed  | 0     | 0    | 2      |
          // Auto   | 0     | 0    | 2      |
          // Weight | 3     | 1    | 3      |

          if (cell.UnitTypeFlagsY != GridUnitType::Weight)
          {
            if (cell.UnitTypeFlagsX != GridUnitType::Weight)
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
    //        | Fixed | Auto | Weight |
    // Fixed  | 0     | 0    | 2      |
    // Auto   | 0     | 0    | 2      |
    // Weight | 3     | 1    | 3      |

    void GridLayout::MeasureCellGroup0(const uint32_t groupStartIndex, const Vector2& availableSize)
    {
      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto || m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Fixed);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto || m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Fixed);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        auto availX = m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto ? LayoutHelper::InfiniteSpace : m_definitionsX[indexX].MeasureSize;
        auto availY = m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto ? LayoutHelper::InfiniteSpace : m_definitionsY[indexY].MeasureSize;

        // Finally measure the cell
        window->Measure(Vector2(availX, availY));

        assert(m_definitionsX[indexX].MeasureUnitType != GridUnitType::Fixed || m_definitionsX[indexX].Size == m_definitionsX[indexX].MinimumSize);
        assert(m_definitionsY[indexY].MeasureUnitType != GridUnitType::Fixed || m_definitionsY[indexY].Size == m_definitionsY[indexY].MinimumSize);

        // Apply the measured window dimensions to the minimum cell size
        auto desiredSize = window->DesiredSize();
        if (m_definitionsX[indexX].MeasureUnitType != GridUnitType::Fixed)
        {
          m_definitionsX[indexX].ApplyMinSize(desiredSize.X);
        }
        if (m_definitionsY[indexY].MeasureUnitType != GridUnitType::Fixed)
        {
          m_definitionsY[indexY].ApplyMinSize(desiredSize.Y);
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    //! Group1 cells are all of the type "auto X weight"
    //        | Fixed | Auto | Weight |
    // Fixed  | 0     | 0    | 2      |
    // Auto   | 0     | 0    | 2      |
    // Weight | 3     | 1    | 3      |

    bool GridLayout::MeasureCellGroup1(const uint32_t groupStartIndex, const Vector2& availableSize, const bool useInfinityY,
                                       const bool ignoreDesiredX)
    {
      bool widthModified = false;
      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());

      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Auto);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Weight);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        auto availX = LayoutHelper::InfiniteSpace;
        auto availY = !useInfinityY ? m_definitionsY[indexY].MeasureSize : LayoutHelper::InfiniteSpace;

        float oldWidth = window->DesiredSize().X;
        // Finally measure the cell
        window->Measure(Vector2(availX, availY));

        widthModified |= EqualHelper::IsAlmostEqual(oldWidth, window->DesiredSize().X);

        // Apply the measured window dimensions to the minimum cell size
        auto desiredSize = window->DesiredSize();
        if (!ignoreDesiredX)
        {
          m_definitionsX[indexX].ApplyMinSize(desiredSize.X);
        }
        if (!useInfinityY)
        {
          m_definitionsY[indexY].ApplyMinSize(desiredSize.Y);
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
      return widthModified;
    }

    //! Group2 cells are all of the type "weight X fixed", "weight x auto"
    //        | Fixed | Auto | Weight |
    // Fixed  | 0     | 0    | 2      |
    // Auto   | 0     | 0    | 2      |
    // Weight | 3     | 1    | 3      |
    void GridLayout::MeasureCellGroup2(const uint32_t groupStartIndex, const Vector2& availableSize)
    {
      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Weight);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Fixed || m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        auto availX = m_definitionsX[indexX].MeasureSize;
        auto availY = m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Auto ? LayoutHelper::InfiniteSpace : m_definitionsY[indexY].MeasureSize;

        // Finally measure the cell
        window->Measure(Vector2(availX, availY));

        assert(m_definitionsY[indexY].MeasureUnitType != GridUnitType::Fixed || m_definitionsY[indexY].Size == m_definitionsY[indexY].MinimumSize);

        // Apply the measured window dimensions to the minimum cell size
        auto desiredSize = window->DesiredSize();
        m_definitionsX[indexX].ApplyMinSize(desiredSize.X);
        if (m_definitionsY[indexY].MeasureUnitType != GridUnitType::Fixed)
        {
          m_definitionsY[indexY].ApplyMinSize(desiredSize.Y);
        }

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }

    //! Group3  cells are all of the type "weight X weight" or "fixed X weight"
    //        | Fixed | Auto | Weight |
    // Fixed  | 0     | 0    | 2      |
    // Auto   | 0     | 0    | 2      |
    // Weight | 3     | 1    | 3      |
    void GridLayout::MeasureCellGroup3(const uint32_t groupStartIndex, const Vector2& availableSize)
    {
      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());
        assert(m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Fixed || m_cellRecords[index].UnitTypeFlagsX == GridUnitType::Weight);
        assert(m_cellRecords[index].UnitTypeFlagsY == GridUnitType::Weight);

        auto window = ChildAt(index);
        assert(window);

        const auto indexX = m_cellRecords[index].IndexX;
        const auto indexY = m_cellRecords[index].IndexY;

        assert(indexX < m_definitionsX.size());
        assert(indexY < m_definitionsY.size());

        auto availX = m_definitionsX[indexX].MeasureSize;
        auto availY = m_definitionsY[indexY].MeasureSize;

        // Finally measure the cell
        window->Measure(Vector2(availX, availY));

        assert(m_definitionsX[indexX].MeasureUnitType != GridUnitType::Fixed || m_definitionsX[indexX].Size == m_definitionsX[indexX].MinimumSize);

        // Apply the measured window dimensions to the minimum cell size
        auto desiredSize = window->DesiredSize();
        if (m_definitionsX[indexX].MeasureUnitType != GridUnitType::Fixed)
        {
          m_definitionsX[indexX].ApplyMinSize(desiredSize.X);
        }
        m_definitionsY[indexY].ApplyMinSize(desiredSize.Y);

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    void GridLayout::ResolveWeights(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const float totalAvailableSpace)
    {
      // Figure out the total weight
      float totalWeight = 0.0f;
      float spaceLeft = totalAvailableSpace;
      for (const auto& entry : rDefinitions)
      {
        if (entry.MeasureUnitType == GridUnitType::Weight)
        {
          assert(entry.Size >= 0.0f);
          assert(entry.Size < LayoutHelper::ConsideredInfiniteSpace);
          // We rely on reasonable weights being used (so overflow will not be a issue)
          totalWeight += entry.Size;
        }
        else
        {
          spaceLeft -= entry.MinimumSize;
        }
      }
      // This method should not be called if there are no weights
      // We expect the total to be in a valid range
      assert(totalWeight > 0.0f);
      assert(totalWeight < LayoutHelper::ConsideredInfiniteSpace);
      spaceLeft = std::max(spaceLeft, 0.0f);
      for (auto& rEntry : rDefinitions)
      {
        if (rEntry.MeasureUnitType == GridUnitType::Weight)
        {
          auto weightedSpace = spaceLeft * (rEntry.Size / totalWeight);
          rEntry.MeasureSize = weightedSpace;
          rEntry.ApplyMinSize(weightedSpace);
        }
      }
    }


    void GridLayout::StoreMinSizeX(const uint32_t groupStartIndex)
    {
      for (auto& rDef : m_definitionsX)
      {
        rDef.TempValue = -42.0f;
      }

      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());

        const auto indexX = m_cellRecords[index].IndexX;
        assert(indexX < m_definitionsX.size());

        // Store the min size
        m_definitionsX[indexX].TempValue = m_definitionsX[indexX].MinimumSize;

        // Validate our assumptions
        assert(index != m_cellRecords[index].NextIndex);
        index = m_cellRecords[index].NextIndex;
      }
    }


    void GridLayout::StoreMinSizeY(const uint32_t groupStartIndex)
    {
      for (auto& rDef : m_definitionsY)
      {
        rDef.TempValue = -42.0f;
      }

      uint32_t index = groupStartIndex;
      assert(index == INVALID_INDEX || index < m_cellRecords.size());
      assert(size() <= m_cellRecords.size());
      while (index < m_cellRecords.size())
      {
        assert(index < m_cellRecords.size());

        const auto indexY = m_cellRecords[index].IndexY;
        assert(indexY < m_definitionsY.size());

        // Store the min size
        m_definitionsY[indexY].TempValue = m_definitionsY[indexY].MinimumSize;

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
          rDef.MinimumSize = rDef.TempValue;
        }
      }
    }
  }
}

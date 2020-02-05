#ifndef FSLSIMPLEUI_BASE_LAYOUT_GRIDLAYOUT_HPP
#define FSLSIMPLEUI_BASE_LAYOUT_GRIDLAYOUT_HPP
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

#include <FslSimpleUI/Base/Layout/ComplexLayout.hpp>
#include <FslSimpleUI/Base/Layout/GridRowColumnDefinition.hpp>

namespace Fsl
{
  namespace UI
  {
    struct GridWindowCollectionRecord : public GenericWindowCollectionRecordBase
    {
      uint32_t IndexX{0};
      uint32_t IndexY{0};

      GridWindowCollectionRecord(const std::shared_ptr<BaseWindow>& window)
        : GenericWindowCollectionRecordBase(window)
      {
      }
    };

    class GridLayout : public ComplexLayout<GridWindowCollectionRecord>
    {
      struct WindowCellRecord
      {
        uint32_t IndexX{0};
        uint32_t IndexY{0};
        uint32_t NextIndex{0};
        GridUnitType UnitTypeFlagsX{GridUnitType::Fixed};
        GridUnitType UnitTypeFlagsY{GridUnitType::Fixed};
      };

      struct DefinitionCache
      {
        bool HasWeightX{false};
        bool HasWeightY{false};
        bool HasEntriesX{false};
        bool HasEntriesY{false};
      };

      struct CellInfo
      {
        uint32_t FirstIndexCellGroup0{0};
        uint32_t FirstIndexCellGroup1{0};
        uint32_t FirstIndexCellGroup2{0};
        uint32_t FirstIndexCellGroup3{0};
        bool Group2HasAutoMembers{false};
      };

      struct GridRowColumnDefinitionEx : public GridRowColumnDefinitionBase
      {
        GridUnitType MeasureUnitType{GridUnitType::Fixed};
        float MeasureSize{0.0f};
        //! The minimum size of the final cell
        float MinimumSize{0.0f};
        //! Used for temporary storing the MinimumSize during measure and also as a cell offset during arrange
        float TempValue{0.0f};

        explicit constexpr GridRowColumnDefinitionEx(const GridColumnDefinition& ex)
          : GridRowColumnDefinitionBase(ex)
        {
        }
        explicit constexpr GridRowColumnDefinitionEx(const GridRowDefinition& ex)
          : GridRowColumnDefinitionBase(ex)
        {
        }

        //! @brief Applies the newMinSize if its larger than the existing min size
        inline void ApplyMinSize(const float newMinSize)
        {
          MinimumSize = std::max(MinimumSize, newMinSize);
        }
      };


      std::deque<GridRowColumnDefinitionEx> m_definitionsX;
      std::deque<GridRowColumnDefinitionEx> m_definitionsY;
      std::deque<WindowCellRecord> m_cellRecords;
      DefinitionCache m_definitionCache;
      CellInfo m_cellInfo;

    public:
      GridLayout(const std::shared_ptr<BaseWindowContext>& context);


      //! @brief Assign window to a specific column
      //! @exception std::invalid_argument
      void SetColumn(const std::shared_ptr<BaseWindow>& window, const uint32_t index);

      //! @brief Assign window to a specific row
      void SetRow(const std::shared_ptr<BaseWindow>& window, const uint32_t index);

      //! @brief Assign window to a specific column and row
      void Set(const std::shared_ptr<BaseWindow>& window, const uint32_t columnIndex, const uint32_t rowIndex);

      std::size_t GetColumnDefinitionCount() const
      {
        return m_definitionsX.size();
      }

      // void ClearColumnDefinitions();

      void AddColumnDefinition(const GridColumnDefinition& definition);

      std::size_t GetRowDefinitionCount() const
      {
        return m_definitionsY.size();
      }

      void AddRowDefinition(const GridRowDefinition& definition);

      // void ClearRowDefinitions();

    protected:
      Vector2 ArrangeOverride(const Vector2& finalSize) override;
      Vector2 MeasureOverride(const Vector2& availableSize) override;

    private:
      void BasicMeasure(const Vector2& availableSize);
      void SemiComplexMeasure(const Vector2& availableSize);
      void ComplexMeasure(const Vector2& availableSize);

      Vector2 ResolveMeasureSize();

      bool UpdateDefinitionCache(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const bool treatWeightAsAuto);
      //! @brief Iterate through all windows and classify which layout group it belongs to.
      void ClassifyWindowCell();
      void MeasureCellGroup0(const uint32_t groupStartIndex, const Vector2& availableSize);
      //! @return true if one of the controls had its width modified
      bool MeasureCellGroup1(const uint32_t groupStartIndex, const Vector2& availableSize, const bool useInfinityY, const bool ignoreDesiredX);
      void MeasureCellGroup2(const uint32_t groupStartIndex, const Vector2& availableSize);
      void MeasureCellGroup3(const uint32_t groupStartIndex, const Vector2& availableSize);
      void ResolveWeights(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const float totalAvailableSpace);
      void StoreMinSizeX(const uint32_t groupStartIndex);
      void StoreMinSizeY(const uint32_t groupStartIndex);
      static void RestoreMinSize(std::deque<GridRowColumnDefinitionEx>& rDefinitions);
    };
  }
}

#endif

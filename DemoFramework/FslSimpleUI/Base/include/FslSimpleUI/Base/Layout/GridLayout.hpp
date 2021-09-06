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

      explicit GridWindowCollectionRecord(const std::shared_ptr<BaseWindow>& window)
        : GenericWindowCollectionRecordBase(window)
      {
      }
    };

    class GridLayout : public ComplexLayout<GridWindowCollectionRecord>
    {
      enum class InternalGridUnitType
      {
        //! Fixed size
        Fixed = 0,
        //! Auto value, any specified size is ignored
        Auto = 1,
        //! Star
        Star = 2,
        //! Auto during measure, weight druing final
        AutoStar = 3
      };

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
        bool HasStarX{false};
        bool HasStarY{false};
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
        InternalGridUnitType MeasureUnitType{InternalGridUnitType::Fixed};
        int32_t MeasureSizePx{};
        //! The minimum size of the final cell
        int32_t MinimumSizePx{};
        //! The minimum arrange size of the final cell
        int32_t ArrangeMinimumSizePx{};
        //! Used for temporary storing the MinimumSize during measure and also as a cell offset during arrange
        int32_t TempValue{};

        explicit constexpr GridRowColumnDefinitionEx(const GridColumnDefinition& ex)
          : GridRowColumnDefinitionBase(ex)
        {
        }
        explicit constexpr GridRowColumnDefinitionEx(const GridRowDefinition& ex)
          : GridRowColumnDefinitionBase(ex)
        {
        }

        //! @brief Applies the newMinSize if its larger than the existing min size
        inline void ApplyMeasureMinSize(const int32_t newMinSizePx)
        {
          MinimumSizePx = std::max(MinimumSizePx, newMinSizePx);
        }

        inline void ClearArrangeMinSize()
        {
          ArrangeMinimumSizePx = MinimumSizePx;
        }

        inline void SetArrangeMinSize(const int32_t newMinSizePx)
        {
          // it is correct that we use MinimumSizePx as the one we validate against
          ArrangeMinimumSizePx = std::max(MinimumSizePx, newMinSizePx);
        }

        inline int32_t CalcMeasureMinSize(const int32_t newMinSizePx)
        {
          return std::max(MinimumSizePx, newMinSizePx);
        }
      };

      std::deque<GridRowColumnDefinitionEx> m_definitionsX;
      std::deque<GridRowColumnDefinitionEx> m_definitionsY;
      std::deque<WindowCellRecord> m_cellRecords;
      DefinitionCache m_definitionCache;
      CellInfo m_cellInfo;
      bool m_limitToAvailableSpace{false};

    public:
      explicit GridLayout(const std::shared_ptr<BaseWindowContext>& context);


      //! @brief Assign window to a specific column
      //! @exception std::invalid_argument
      void SetColumn(const std::shared_ptr<BaseWindow>& window, const uint32_t index);

      //! @brief Assign window to a specific row
      void SetRow(const std::shared_ptr<BaseWindow>& window, const uint32_t index);

      //! @brief Assign window to a specific column and row
      void Set(const std::shared_ptr<BaseWindow>& window, const uint32_t columnIndex, const uint32_t rowIndex);

      void SetLimitToAvailableSpace(const bool enabled);

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

      using ComplexLayout<GridWindowCollectionRecord>::AddChild;

      void AddChild(const std::shared_ptr<BaseWindow>& window, const uint32_t columnIndex, const uint32_t rowIndex)
      {
        AddChild(window);
        Set(window, columnIndex, rowIndex);
      }

      // void ClearRowDefinitions();


    protected:
      PxSize2D ArrangeOverride(const PxSize2D& finalSizePx) override;
      PxSize2D MeasureOverride(const PxAvailableSize& availableSizePx) override;

    private:
      void BasicMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx);
      void SemiComplexMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx);
      void ComplexMeasure(const SpriteUnitConverter& unitConverter, const PxAvailableSize& availableSizePx);

      PxSize2D ResolveMeasureSize();

      static bool UpdateDefinitionCache(const SpriteUnitConverter& unitConverter, std::deque<GridRowColumnDefinitionEx>& rDefinitions,
                                        const bool treatStarAsAuto);
      //! @brief Iterate through all windows and classify which layout group it belongs to.
      void ClassifyWindowCell();
      void MeasureCellGroup0(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex);
      //! @return true if one of the controls had its width modified
      bool MeasureCellGroup1(const uint32_t groupStartIndex, const PxAvailableSize& availableSizePx, const bool useInfinityY,
                             const bool ignoreDesiredX);
      void MeasureCellGroup2(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex, const PxAvailableSize& availableSizePx);
      void MeasureCellGroup3(const SpriteUnitConverter& unitConverter, const uint32_t groupStartIndex, const PxAvailableSize& availableSizePx);
      static void ResolveStars(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const int32_t totalAvailableSpacePx);
      PxSize2D FinalizeSizes(const PxSize2D& finalSizePx);
      static void ResetMinimumSize(std::deque<GridRowColumnDefinitionEx>& rDefinitions);
      static void FinalizeStars(std::deque<GridRowColumnDefinitionEx>& rDefinitions, const int32_t totalAvailableSpacePx);
      void StoreMinSizeX(const uint32_t groupStartIndex);
      void StoreMinSizeY(const uint32_t groupStartIndex);
      static void RestoreMinSize(std::deque<GridRowColumnDefinitionEx>& rDefinitions);
    };
  }
}

#endif

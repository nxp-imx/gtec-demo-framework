#ifndef FSLNATIVEWINDOW_NATIVEWINDOWEVENTQUEUE_HPP
#define FSLNATIVEWINDOW_NATIVEWINDOWEVENTQUEUE_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Collections/Concurrent/ConcurrentQueue_fwd.hpp>
#include <FslNativeWindow/INativeWindowEventQueue.hpp>

namespace Fsl
{

  //! @brief Simple queue interface for positing events
  //! Beware this object is thread safe.
  class NativeWindowEventQueue : public INativeWindowEventQueue
  {
    ConcurrentQueue<NativeWindowEvent> m_queue;
  public:
    NativeWindowEventQueue();
    ~NativeWindowEventQueue();

    //! @brief Clear all events from the queue
    void Clear();

    //! @brief Tries to remove and return the element at the beginning of the queue.
    //! @return true on success, false if unsuccessful. When false rValue will be set to T().
    bool TryDequeue(NativeWindowEvent& rEvent);

    // From INativeWindowEventQueue
    virtual void PostEvent(const NativeWindowEvent& event);
  };

}

#endif

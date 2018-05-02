/****************************************************************************************************************************************************
* Copyright (c) 2015 Freescale Semiconductor, Inc.
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
package com.freescale.demoplatform;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import com.freescale.contentsync.OneWaySync;

public class DemoHelper {

  private OneWaySync m_oneWaySync;
  private ImageService m_imageService;

  public DemoHelper() {
    m_oneWaySync = new OneWaySync();    
    m_imageService = new ImageService();    
  }


	public String SyncNow(Context context) {
    return m_oneWaySync.SyncNow(context);
  }


	public boolean isExternalStorageWritable() {
    return m_oneWaySync.isExternalStorageWritable();
  }
  
  
	public boolean Sync(Context context, String toPath) {
    return m_oneWaySync.Sync(context, toPath);
  }  
  
  public Bitmap TryOpen(Context context, String path) {
    return m_imageService.TryOpen(context, path);
  }


  public int GetWidth(Bitmap bitmap) { 
    return m_imageService.GetWidth(bitmap);
  }


  public int GetHeight(Bitmap bitmap) { 
    return m_imageService.GetHeight(bitmap);
  }


  public void GetPixels(Bitmap bitmap, int[] pixels) {
    m_imageService.GetPixels(bitmap, pixels);
  }


  public void Close(Bitmap bitmap) {
     m_imageService.Close(bitmap);
  }    

  // From API 24 we can use Display.HdrCapabilities for more advanced capability checks
  // https://developer.android.com/reference/android/view/Display.HdrCapabilities.html
  public boolean IsDisplayHDRCompatible() throws IOException {
    try {
      java.lang.Process p = Runtime.getRuntime().exec("getprop sys.hwc.hdr.supported");
      BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
      String valueStr = in.readLine();
      final int value = Integer.parseInt(valueStr);
      // If the value is 1 the connected sink is HDR-compatible.
      if (value == 1)
        return true;
    } 
    catch (IOException e) 
    {
      e.printStackTrace();
    }
    return false;    
  }
}

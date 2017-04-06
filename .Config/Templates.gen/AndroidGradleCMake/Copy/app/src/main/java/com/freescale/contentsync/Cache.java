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
package com.freescale.contentsync;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class Cache {

	private static final String HEADER = "# FslContentSync cache file";
	private static final String HEADER_FORMAT = "# Format: 1";
	
	public ArrayList<CacheEntry> Files = new ArrayList<CacheEntry>();
	public ArrayList<CacheEntry> Dirs = new ArrayList<CacheEntry>();

	public Cache()
	{
	}
	
	public Cache(InputStream stream) throws UnsupportedEncodingException,
			IOException {
		String content = FileUtil.LoadUTF8(stream);
		Prepare(content);
	}

	public Cache(String filename) throws UnsupportedEncodingException,
			IOException {
		try
		{
			String content = FileUtil.LoadUTF8(filename);
			Prepare(content);
		}
		catch(FileNotFoundException ex)
		{
			
		}
	}

	private boolean ValidateHeader(String[] lines){
		if(lines.length < 2 )
			return false;
		if( ! lines[0].equals(HEADER) ) {
			return false;
		}
		if( ! lines[1].equals(HEADER_FORMAT) ) {
			return false;
		}
		return true;
	}
	
	private void Prepare(String content) throws UnsupportedEncodingException,
			IOException {
		String[] lines = content.split("\n");

		if( ! ValidateHeader(lines) )
			return;
		
		for (int i = 0; i < lines.length; i++) {
			String line = lines[i];
			if (!line.startsWith("#")) {
				String[] parts = line.split("\\\\");
				if (parts.length != 4)
					throw new IOException("Cache entry is invalid");

				long length = Long.parseLong(parts[1]);
				CacheEntry entry = new CacheEntry(parts[0], length, parts[2],
						parts[3]);

				if (length >= 0)
					Files.add(entry);
				else
					Dirs.add(entry);
			}
		}
	}

}

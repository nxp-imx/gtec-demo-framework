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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;

public class FileUtil {

	public static String LoadUTF8(InputStream stream)
			throws UnsupportedEncodingException, IOException {

		BufferedReader in = null;
		try {
			in = new BufferedReader(new InputStreamReader(stream, "UTF8"));

			String str;
			StringBuilder builder = new StringBuilder();

			while ((str = in.readLine()) != null) {
				builder.append(str);
				builder.append("\n");
			}
			return builder.toString();

		} finally {
			if (in != null) {
				in.close();
			}
		}
	}

	public static String LoadUTF8(String filename)
			throws UnsupportedEncodingException, IOException {

		BufferedReader in = null;
		try {
			File fileDir = new File(filename);
			in = new BufferedReader(new InputStreamReader(new FileInputStream(
					fileDir), "UTF8"));

			String str;
			StringBuilder builder = new StringBuilder();

			while ((str = in.readLine()) != null) {
				builder.append(str);
				builder.append("\n");
			}
			return builder.toString();

		} finally {
			if (in != null) {
				in.close();
			}
		}
	}

	public static String GetFileSha256Hash(String filename)
			throws NoSuchAlgorithmException, IOException {
		MessageDigest md = MessageDigest.getInstance("SHA-256");

		FileInputStream fis = null;
		try {
			fis = new FileInputStream(filename);

			byte[] dataBytes = new byte[1024];

			int nread = 0;
			while ((nread = fis.read(dataBytes)) != -1) {
				md.update(dataBytes, 0, nread);
			}
			;
		} finally {
			if (fis != null)
				fis.close();
		}
		byte[] mdbytes = md.digest();

		// convert the byte to hex format method 1
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < mdbytes.length; i++) {
			sb.append(Integer.toString((mdbytes[i] & 0xff) + 0x100, 16)
					.substring(1));
		}
		return sb.toString();
	}


	public static void Walk(String path, ArrayList<String> files,
			ArrayList<String> dirs) {
		Walk(new File(path), files, dirs);
	}

	public static void Walk(File root, ArrayList<String> files,
			ArrayList<String> dirs) {

		File[] list = root.listFiles();

		if (list != null) {
			for (File f : list) {
				if (f.isDirectory()) {
					dirs.add(f.getAbsoluteFile().toString());
					Walk(f, files, dirs);
				} else {
					files.add(f.getAbsoluteFile().toString());
				}
			}

		}
	}

}
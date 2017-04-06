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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

public class OneWaySync {

	static final String TAG = "com.freescale.contentsync";
	static final String CONTENT_PATH = "fsl-content";
	static final String CACHE_FILENAME = "_ContentSyncCache.fsl";

	public String SyncNow(Context context) {

		String toPath;
		if (isExternalStorageWritable()) {
			toPath = context.getExternalFilesDir(null).toString();
			String to = CombinePath(toPath, CONTENT_PATH);
			if (!Sync(context, to))
				toPath = null;
		} else {
			Log.e(TAG,
					"External storage is not writable, using internal storage");

			toPath = context.getFilesDir().toString();
			String to = CombinePath(toPath, CONTENT_PATH);
			if (!Sync(context, to))
				toPath = null;
		}
		return toPath;
	}

	public boolean isExternalStorageWritable() {
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			return true;
		}
		return false;
	}

	public boolean Sync(Context context, String toPath) {
		InputStream instream = null;
		try {
			String fromPath = CONTENT_PATH;
			AssetManager assetManager = context.getAssets();

			instream = assetManager.open(CombinePath(fromPath, CACHE_FILENAME));
			Cache cacheFrom = new Cache(instream);

			String toCacheFilename = CombinePath(toPath, CACHE_FILENAME);
			Cache cacheTo = new Cache(toCacheFilename);
			DoSync(assetManager, cacheFrom, fromPath, cacheTo, toPath);
			return true;
		} catch (Exception ex) {
			PrintError(ex.getMessage());
			return false;
		} finally {
			try {
				if (instream != null)
					instream.close();
			} catch (IOException ex) {
				PrintError(ex.getMessage());
				return false;
			}
		}
	}

	private void DoSync(AssetManager assetManager, Cache cacheFrom,
			String from, Cache cacheTo, String to) throws IOException {
        
		{ // Ensure that the to folder exist
			File file = new File(to);
			file.mkdirs();
		}
    
		UpdateCache(to, cacheTo);

		ArrayList<CacheEntry> removeDirs = BuildRemoveList(cacheFrom.Dirs,
				cacheTo.Dirs);
		ArrayList<CacheEntry> removeFiles = BuildRemoveList(cacheFrom.Files,
				cacheTo.Files);
		ArrayList<CacheEntry> createDirs = BuildCreateList(cacheFrom.Dirs,
				cacheTo.Dirs);
		ArrayList<CacheEntry> copyFiles = BuildCopyList(cacheFrom.Files,
				cacheTo.Files);

		for (int i = 0; i < removeFiles.size(); ++i) {
			CacheEntry entry = removeFiles.get(i);
			Print("Removing: " + entry.Name);
			File file = new File(to, entry.Name);
			file.delete();
		}

		for (int i = 0; i < removeDirs.size(); ++i) {
			CacheEntry entry = removeDirs.get(i);
			Print("Removing dir: " + entry.Name);
			File file = new File(to, entry.Name);
			file.delete();
		}

		for (int i = 0; i < createDirs.size(); ++i) {
			CacheEntry entry = createDirs.get(i);
			Print("Creating dir: " + entry.Name);
			File file = new File(to, entry.Name);
			file.mkdirs();
		}

		for (int i = 0; i < copyFiles.size(); ++i) {
			CacheEntry entry = copyFiles.get(i);
			Print("Extracting file: " + entry.Name);
			String fromName = CombinePath(from, entry.Name);
			String toName = CombinePath(to, entry.Name);
			CopyAsset(assetManager, fromName, new File(toName));
		}

		{ // Write the cache file
			String fromName = CombinePath(from, CACHE_FILENAME);
			String toName = CombinePath(to, CACHE_FILENAME);
			CopyAsset(assetManager, fromName, new File(toName));
		}

		if (removeDirs.size() == 0 && removeFiles.size() == 0
				&& createDirs.size() == 0 && copyFiles.size() == 0) {
			Print("Everything is up to date.");
		}
		Print("Sync completed.");
	}

	public void CopyAsset(AssetManager assetManager, String srcFile, File dst)
			throws IOException {
		
		// todo: Copy modification time correctly 
		
		InputStream in = null;
		BufferedOutputStream out = null;
		try {
			in = assetManager.open(srcFile);
			out = new BufferedOutputStream(new FileOutputStream(dst));
			// Transfer bytes from in to out
			byte[] buf = new byte[1024];
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}
		} finally {
			if (in != null)
				in.close();
			if (out != null)
				out.close();
		}

	}

	private static void Print(String str) {
		Log.i(TAG, str);
	}

	private static void PrintError(String str) {
		Log.e(TAG, str);
	}

	private static ArrayList<CacheEntry> BuildCopyList(
			ArrayList<CacheEntry> from, ArrayList<CacheEntry> to) {
		ArrayList<CacheEntry> res = new ArrayList<CacheEntry>();
		for (int i = 0; i < from.size(); ++i) {
			CacheEntry entryFrom = from.get(i);
			CacheEntry entryTo = Find(to, entryFrom.Name);
			if (entryTo == null || !entryFrom.IsValueEqual(entryTo))
				res.add(entryFrom);
		}
		return res;
	}

	private static ArrayList<CacheEntry> BuildCreateList(
			ArrayList<CacheEntry> from, ArrayList<CacheEntry> to) {
		ArrayList<CacheEntry> res = new ArrayList<CacheEntry>();
		for (int i = 0; i < from.size(); ++i) {
			CacheEntry entry = from.get(i);
			if (!Contains(to, entry.Name))
				res.add(entry);
		}
		return res;
	}

	private static ArrayList<CacheEntry> BuildRemoveList(
			ArrayList<CacheEntry> from, ArrayList<CacheEntry> to) {
		ArrayList<CacheEntry> res = new ArrayList<CacheEntry>();
		for (int i = 0; i < to.size(); ++i) {
			CacheEntry entry = to.get(i);
			if (!Contains(from, entry.Name))
				res.add(entry);
		}
		return res;
	}

	private static boolean Contains(ArrayList<CacheEntry> list, String name) {
		for (int i = 0; i < list.size(); ++i) {
			CacheEntry entry = list.get(i);
			if (entry.Name.equals(name))
				return true;
		}
		return false;
	}

	private static CacheEntry Find(ArrayList<CacheEntry> list, String name) {
		for (int i = 0; i < list.size(); ++i) {
			CacheEntry entry = list.get(i);
			if (entry.Name.equals(name))
				return entry;
		}
		return null;
	}

	private static void UpdateCache(String path, Cache cache) {
		ArrayList<String> files = new ArrayList<String>();
		ArrayList<String> dirs = new ArrayList<String>();
		FileUtil.Walk(path, files, dirs);
		StripPath(path, dirs);
		StripPath(path, files);

		// Lets remove the invalid directories
		ArrayList<CacheEntry> remove = new ArrayList<CacheEntry>();
		for (int i = 0; i < cache.Dirs.size(); ++i) {
			CacheEntry entry = cache.Dirs.get(i);
			if (!ContainsString(dirs, entry.Name))
				remove.add(entry);
		}
		cache.Dirs.removeAll(remove);

		// Lets remove the invalid files
		remove.clear();
		for (int i = 0; i < cache.Files.size(); ++i) {
			CacheEntry entry = cache.Files.get(i);
			if (!ContainsString(files, entry.Name))
				remove.add(entry);
		}

		cache.Files.removeAll(remove);

		// Lets add the found directories
		for (int i = 0; i < dirs.size(); ++i) {
			String entry = dirs.get(i);
			if (!Contains(cache.Dirs, entry)) {
				cache.Dirs.add(new CacheEntry(entry, -1, "0", "0"));
			}
		}
		// Lets add the found files (to ensure they get removed or overwritten)
		for (int i = 0; i < files.size(); ++i) {
			String entry = files.get(i);
			if (!Contains(cache.Files, entry)) {
				// todo: extract the modification time correctly 
				//File file = new File(path, entry);
				// cache.Files.add(new CacheEntry(entry, file.length(), "0", "0"));
				cache.Files.add(new CacheEntry(entry, 0, "0", "0"));
			}
		}

	}

	private static boolean ContainsString(ArrayList<String> list, String name) {
		for (int i = 0; i < list.size(); ++i) {
			String entry = list.get(i);
			if (entry.equals(name))
				return true;
		}
		return false;
	}

	private static void StripPath(String path, ArrayList<String> list) {
		int len = PathStripEndingSlash(path).length() + 1;
		for (int i = 0; i < list.size(); ++i) {
			String entry = list.get(i);
			entry = ConvertToSlash(entry.substring(len));
			list.set(i, entry);
		}
	}

	private static String ConvertToSlash(String path) {
		return path.replaceAll("\\\\", "/");
	}

	private static String PathStripEndingSlash(String path) {
		if (path.endsWith("/") || path.endsWith("\\"))
			return path.substring(0, path.length() - 1);
		else
			return path;
	}

	private static boolean IsPathRooted(String path) {
		// A fairly simple check for rooted paths
		return path.length() != 0
				&& (path.startsWith("/") || path.startsWith("\\") || path
						.indexOf(":") >= 0);
	}

	private static String CombinePath(String path1, String path2) {
		if (IsPathRooted(path2))
			return path2;

		if (path1.length() == 0)
			return path2;
		else if (path2.length() == 0)
			return path1;
		else if (!path1.endsWith("/") || !path1.endsWith("\\"))
			return path1 + '/' + path2;
		else
			return path1 + path2;
	}

}

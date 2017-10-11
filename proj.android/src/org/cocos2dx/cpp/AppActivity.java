/****************************************************************************
 Copyright (c) 2015 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.cpp;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.os.Bundle;
import android.util.Log;


import org.cocos2dx.lib.Cocos2dxActivity;

public class AppActivity extends Cocos2dxActivity {

    private static void copy(InputStream srcStream, String destName) throws IOException {
        new File(destName).createNewFile();
        OutputStream destStream = new FileOutputStream(destName);

        byte[] buffer = new byte[1024];
        int length;

        while ((length = srcStream.read(buffer)) > 0) {
            destStream.write(buffer, 0, length);
        }

        srcStream.close();
        destStream.close();
    }

    private static boolean deleteRecursively(File dir) {
        if (dir.isDirectory()) {
            String[] children = dir.list();
            for (int i = 0; i < children.length; i++) {
                boolean success = deleteRecursively(new File(dir, children[i]));
                if (!success) {
                    return false;
                }
            }
        }

        // 目录此时为空，可以删除
        return dir.delete();
    }


    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /* 1. 删除手机内存储的 gamedata 目录
         *    在开发时期，为方便起见，Android 应用每次启动时都重置
         */

        File toDir = new File(this.getFilesDir().getAbsolutePath() + "/gamedata");
        deleteRecursively(toDir);

        Log.d("AppActivity", ">> path 1: " + this.getFilesDir().getAbsolutePath() + "/gamedata");

        /* 2. 在手机内创建新的 gamedata 目录 */

        toDir.mkdir();

        /* 3. 拷贝 assets */

        String assetsPrefix = "gamedata/";
        String[] filesToCopy = {"awards.json",
                "characters.json",
                "conversations.json",
                "enemies.json",
                "events.json",
                "items.json",
                "locations.json",
                "saves.json",
                "spell_cards.json"};

        try {
            InputStream is;
            for (String f : filesToCopy) {
                is = this.getAssets().open(assetsPrefix + f);
                copy(is, getFilesDir() + "/gamedata/" + f);
                Log.d("AppActivity", ">> path2: " + getFilesDir() + "/gamedata/" + f);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

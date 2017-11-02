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

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        boolean overrideSaves = true;

        String[] filesToCopy = {
                "gamedata/awards.json",
                "gamedata/characters.json",
                "gamedata/conversations.json",
                "gamedata/enemies.json",
                "gamedata/events.json",
                "gamedata/items.json",
                "gamedata/locations.json",
                "gamedata/saves.json",
                "gamedata/spell_cards.json",
                "gamedata/update.json",
                "LuaScripts/conversations/init.lua",
                "LuaScripts/conversations/c1.lua"
        };

        for (String path : filesToCopy) {
            Log.d("AppActivity", ">> path: " + path);
            File file = new File(this.getFilesDir().getAbsolutePath() + "/" + path);
            Log.d("AppActivity", ">> copying file " + file.toString());

            if (overrideSaves && file.getName().equals("saves.json") && file.exists()) {
                continue;
            }

            try {
                if (file.exists())
                    file.delete();

                new File(file.getParent()).mkdirs();
                copy(this.getAssets().open(path), file.toString());
                Log.d("AppActivity", ">> copied " + path);
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }
}

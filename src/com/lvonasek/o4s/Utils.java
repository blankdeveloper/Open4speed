package com.lvonasek.o4s;

import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Common utils for easier coding. Code of this class was inspired by another projects
 * @author Lubos Vonasek
 */
public class Utils {

    /**
     * Decompress files from zip archive
     * @param fin is file input stream with opened zip file
     * @param location is place to decompress files
     * @throws IOException exception from ZipInputStream, ZipEntry or  if there is a writing problem
     */
    public static void unzip(InputStream fin, String location) throws IOException {
        // open zip file
        ZipInputStream zin = new ZipInputStream(fin);
        ZipEntry ze;

        // parse all files
        while ((ze = zin.getNextEntry()) != null) {
            if (!ze.isDirectory()) {

                // create directory and file
                if (ze.getName().contains("/")) {
                    int lastIndex = ze.getName().lastIndexOf('/');
                    new File(location + ze.getName().substring(0, lastIndex)).mkdirs();
                }

                // open file output strem for decompression
                Log.i("Decompress", location + ze.getName());
                FileOutputStream fout = new FileOutputStream(location + ze.getName());

                // unpack data
                byte[] buffer = new byte[4096];
                int bytes_read;
                while ((bytes_read = zin.read(buffer)) != -1) {
                    fout.write(buffer, 0, bytes_read);
                }

                fout.close();
            }
            zin.closeEntry();
        }
        zin.close();
    }
}

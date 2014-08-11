package components;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

import javax.imageio.ImageIO;

import geometry.Triangle;

// storage for materials
public class MTLLoader {

  public ArrayList<Triangle> faces;
  public ArrayList<Integer> pointers;
  public String parameters;
  public static boolean top;
  
  private MTLLoader() {}
  
  public MTLLoader(String t) {
    faces = new ArrayList<Triangle>();
    pointers = new ArrayList<Integer>();
    parameters = t;
  }

  public MTLLoader clone(boolean half) {
    MTLLoader mtl = new MTLLoader();
    mtl.faces = new ArrayList<Triangle>();
    mtl.pointers = new ArrayList<Integer>();
    if (half) {
      for (int i = 0; i < this.faces.size() / 2; i++) {
        mtl.faces.add(this.faces.get(i));
      }
    } else {
      for (int i = this.faces.size() / 2; i < this.faces.size(); i++) {
        mtl.faces.add(this.faces.get(i));
      }
    }
    mtl.parameters = "" + this.parameters;
    return mtl;
  }

  // get material parameters from library
  public static String getMaterial(String lib, String mtl) throws IOException {

    top = false;
    // open material library file
    FileInputStream fis;
    try {
      fis = new FileInputStream(lib);
    } catch (FileNotFoundException e) {
      System.err.println(lib + " doesn't exists.");
      return "(null)";
    }
    String phong;
    try (BufferedReader sc = new BufferedReader(new InputStreamReader(fis))) {
      phong = "";
      while (sc.ready()) {
        // read one line of library
        String data = sc.readLine();

        // read parameters of material
        if (data.equals("newmtl " + mtl)) {
          // set default variables
          float ar = 0, ag = 0, ab = 0;
          float dr = 0, dg = 0, db = 0;
          float sr = 0, sg = 0, sb = 0;
          float alpha = 1;

          // parse material data
          while (sc.ready()) {
            // read one line of material data
            data = sc.readLine();

            // set ambient parameter
            if (data.startsWith("Ka ")) {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              ar = ObjLoader.stringToFloat(lsc.nextToken());
              ag = ObjLoader.stringToFloat(lsc.nextToken());
              ab = ObjLoader.stringToFloat(lsc.nextToken());
            }

            // set diffuse parameter
            if (data.startsWith("Kd ")) {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              dr = ObjLoader.stringToFloat(lsc.nextToken());
              dg = ObjLoader.stringToFloat(lsc.nextToken());
              db = ObjLoader.stringToFloat(lsc.nextToken());
            }

            // set specular parameter
            if (data.startsWith("Ks ")) {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              sr = ObjLoader.stringToFloat(lsc.nextToken());
              sg = ObjLoader.stringToFloat(lsc.nextToken());
              sb = ObjLoader.stringToFloat(lsc.nextToken());
            }

            // set alpha parameter
            if (data.startsWith("d ")) {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              alpha = ObjLoader.stringToFloat(lsc.nextToken());
            }

            // compress parameters
            phong = ar + " " + ag + " " + ab + " ";
            phong += dr + " " + dg + " " + db + " ";
            phong += sr + " " + sg + " " + sb + " ";
            phong += alpha;

            // if transparent place it to the top
            if (alpha < 1) {
              top = true;
            }

            // parse material texture
            if (data.startsWith("map_Kd ")) {

              // create name of processed texture
              int fileStart = data.lastIndexOf('/') + 1;
              if (!data.contains("/")) {
                fileStart = 7;
              }
              String textureName = data.substring(fileStart, data.length());
              String fileName = data.substring(fileStart, data.length());
              // cut extension
              StringTokenizer lsc = new StringTokenizer(textureName);
              String cut = lsc.nextToken();
              if (cut.contains(".")) {
                cut = cut.substring(0, cut.indexOf('.'));
              }
              textureName = Common.path + "/" + cut + ".png";

              // check if texture is not already processed
              if (!ObjLoader.exists(textureName)) {
                // get image of texture
                BufferedImage img = null;
                BufferedImage resizedImage;
                try {
                  if (ObjLoader.exists(data.substring(7))) {
                    img = ImageIO.read(new File(data.substring(7)));
                  } else if (ObjLoader.exists(fileName)) {
                    img = ImageIO.read(new File(fileName));
                  } 
                  
                  if (img == null) {
                    System.err.println("Unable to find/open " + fileName);
                    return "(null)";
                  }
                  // get type of texture
                  int type = img.getType();
                  // scale texture
                  int w = 2;
                  int h = 2;
                  while (w < img.getWidth()) {
                    w *= 2;
                  }
                  while (h < img.getHeight()) {
                    h *= 2;
                  }
                  // resize texture
                  resizedImage = new BufferedImage(w, h, type);
                  Graphics2D g = resizedImage.createGraphics();
                  g.drawImage(img, 0, 0, w, h, null);
                  g.dispose();
                } catch (IOException e) {
                  System.err.println("Error parsing " + fileName);
                  fis.close();
                  sc.close();
                  return "(null)";
                }
                // save resized texture
                try {
                  ImageIO.write(resizedImage, "png", new File(textureName));
                  if (!ObjLoader.exists(textureName)) {
                    System.err.println("Unsupported texture format " + fileName);
                    fis.close();
                    sc.close();
                    return "(null)";
                  }
                } catch (IOException e) {
                  System.err.println("Unable to write " + textureName);
                  fis.close();
                  sc.close();
                  return "(null)";
                }
              }
              fis.close();
              sc.close();
              return textureName + " " + phong;
            }

            // end of the material
            if (data.startsWith("newmtl ")) {
              break;
            }
          }
        }
      }
      fis.close();
    }
    return "* " + phong;
  }
}

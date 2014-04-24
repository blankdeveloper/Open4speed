package components;

import geometry.Edge;
import geometry.Triangle;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

public class Writer {

  public static void write(String[] args) {

    System.out.print("Writing output file...");
    ArrayList<Triangle> faces;

    // count faces
    int facesCount = 0;
    for (int j = 1; j < Common.materials.size(); j++) {
      // check if current material has faces
      if (Common.materials.get(j).faces.size() > 0) {
        facesCount++;
      }
    }
    // count edges
    int edgesCount = 0;
    for (int j = 0; j < Common.graph.size(); j++) {
      // check if current graph has edges
      if (Common.graph.get(j).edges.size() > 0) {
        edgesCount++;
      }
    }

    try (FileOutputStream fos = new FileOutputStream(args[1])) {
      fos.write((Common.cellWidth + "\n").getBytes());
      fos.write((Common.cellHeight + "\n").getBytes());

      // save extreme values
      String extremes = Common.minx + " " + Common.miny + " "
              + Common.minz + " ";
      extremes += Common.maxx + " " + Common.maxy + " "
              + Common.maxz + "\n";
      fos.write((extremes).getBytes());

      // save faces count
      fos.write((facesCount + "\n").getBytes());

      // save all faces
      for (int j = 1; j < Common.materials.size(); j++) {
        // get faces pointer for current material
        faces = Common.materials.get(j).faces;
        // find local center
        Common.minx = 99999;
        Common.miny = 99999;
        Common.minz = 99999;
        Common.maxx = -99999;
        Common.maxy = -99999;
        Common.maxz = -99999;
        for (int i = 0; i < faces.size(); i++) {
          if (Common.minx > faces.get(i).a.v.x) {
            Common.minx = faces.get(i).a.v.x;
          }
          if (Common.miny > faces.get(i).a.v.y) {
            Common.miny = faces.get(i).a.v.y;
          }
          if (Common.minz > faces.get(i).a.v.z) {
            Common.minz = faces.get(i).a.v.z;
          }

          if (Common.maxx < faces.get(i).a.v.x) {
            Common.maxx = faces.get(i).a.v.x;
          }
          if (Common.maxy < faces.get(i).a.v.y) {
            Common.maxy = faces.get(i).a.v.y;
          }
          if (Common.maxz < faces.get(i).a.v.z) {
            Common.maxz = faces.get(i).a.v.z;
          }

          if (Common.minx > faces.get(i).b.v.x) {
            Common.minx = faces.get(i).b.v.x;
          }
          if (Common.miny > faces.get(i).b.v.y) {
            Common.miny = faces.get(i).b.v.y;
          }
          if (Common.minz > faces.get(i).b.v.z) {
            Common.minz = faces.get(i).b.v.z;
          }

          if (Common.maxx < faces.get(i).b.v.x) {
            Common.maxx = faces.get(i).b.v.x;
          }
          if (Common.maxy < faces.get(i).b.v.y) {
            Common.maxy = faces.get(i).b.v.y;
          }
          if (Common.maxz < faces.get(i).b.v.z) {
            Common.maxz = faces.get(i).b.v.z;
          }

          if (Common.minx > faces.get(i).c.v.x) {
            Common.minx = faces.get(i).c.v.x;
          }
          if (Common.miny > faces.get(i).c.v.y) {
            Common.miny = faces.get(i).c.v.y;
          }
          if (Common.minz > faces.get(i).c.v.z) {
            Common.minz = faces.get(i).c.v.z;
          }

          if (Common.maxx < faces.get(i).c.v.x) {
            Common.maxx = faces.get(i).c.v.x;
          }
          if (Common.maxy < faces.get(i).c.v.y) {
            Common.maxy = faces.get(i).c.v.y;
          }
          if (Common.maxz < faces.get(i).c.v.z) {
            Common.maxz = faces.get(i).c.v.z;
          }
        }

        // check if current material has faces
        if (faces.size() > 0) {
          // save material parameters
          fos.write((Common.minx + " " + Common.miny
                  + " " + Common.minz + " " + Common.maxx
                  + " " + Common.maxy + " " + Common.maxz
                  + " " + Common.materials.get(j).parameters + "\n")
                  .getBytes());
          // save faces count
          for (int l = 0; l < Common.materials.get(j).pointers
                  .size(); l++) {
            fos.write((Common.materials.get(j).pointers
                    .get(l) + "\n").getBytes());
          }
          fos.write((faces.size() + "\n").getBytes());
          // save face parameters
          for (int i = 0; i < faces.size(); i++) {
            faces.get(i).a.v.x -= Common.minx;
            faces.get(i).a.v.y -= Common.miny;
            faces.get(i).a.v.z -= Common.minz;
            faces.get(i).b.v.x -= Common.minx;
            faces.get(i).b.v.y -= Common.miny;
            faces.get(i).b.v.z -= Common.minz;
            faces.get(i).c.v.x -= Common.minx;
            faces.get(i).c.v.y -= Common.miny;
            faces.get(i).c.v.z -= Common.minz;
            fos.write(faces.get(i).value());
            faces.get(i).a.v.x += Common.minx;
            faces.get(i).a.v.y += Common.miny;
            faces.get(i).a.v.z += Common.minz;
            faces.get(i).b.v.x += Common.minx;
            faces.get(i).b.v.y += Common.miny;
            faces.get(i).b.v.z += Common.minz;
            faces.get(i).c.v.x += Common.minx;
            faces.get(i).c.v.y += Common.miny;
            faces.get(i).c.v.z += Common.minz;
          }
        }
      }

      // save edges count
      fos.write((edgesCount + "\n").getBytes());

      // save all edges
      for (int j = 0; j < Common.graph.size(); j++) {
        // get edges pointer for current material
        ArrayList<Edge> edges = Common.graph.get(j).edges;
        // check if current material has edges
        if (edges.size() > 0) {
          // save edges count
          fos.write((edges.size() + "\n").getBytes());
          // save face parameters
          for (int i = 0; i < edges.size(); i++) {
            fos.write(edges.get(i).value());
          }
        }
      }
    } catch (IOException e) {
      System.err.println("Unable to save file");
      return;
    }
    System.out.println("OK");

    // show information
    if (Common.materials.get(0).faces.size() > 0) {
      System.out.println(Common.materials.get(0).faces.size()
              + " polygons weren't parsed.");
    }
    if (Common.missingNormal > 0) {
      System.out.println(Common.missingNormal
              + " polygons haven't normal.");
    }
    if (Common.missingCoord > 0) {
      System.out.println(Common.missingCoord
              + " vertices haven't texture coords.");
    }
    System.out.println(Unwrapper.lm.size() + " lightmaps generated");
    
    if (facesCount > 0) {
      System.out.println("Converted " + facesCount + " triangle objects");
      if (edgesCount > 0) {
        System.out.println("Converted " + edgesCount + " edge tracks");
      }
      int time = (int) ((System.currentTimeMillis() - Common.timestamp) / 1000);
      System.out.println("Done in " + time + " seconds.");
    } else {
      System.out.println("Nothing converted");
    }
  }
}

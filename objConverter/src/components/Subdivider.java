package components;

import geometry.Triangle;

import java.util.ArrayList;

public class Subdivider {

  public static void subdivide() {

    System.out.print("Subdividing into cells...");
    ArrayList<Triangle> faces = null;
    // subdivide
    Common.cellWidth = (int) ((Common.maxx - Common.minx) / Common.cellSize + 1);
    Common.cellHeight = (int) ((Common.maxz - Common.minz) / Common.cellSize + 1);
    if (Common.cellWidth * Common.cellHeight <= 1) {
      Common.cellWidth = 1;
      Common.cellHeight = 1;
    } else {
      Common.cellHeight++;
    }
    for (int j = 1; j < Common.materials.size(); j++) {
      faces = Common.materials.get(j).faces;
      for (int i = 0; i < faces.size(); i++) {
        int a = faces.get(i).a.getCell();
        int b = faces.get(i).b.getCell();
        int c = faces.get(i).c.getCell();
        faces.get(i).cell = a;
        if ((a != b) || (a != c) || (b != c)) {
          faces.get(i).cell = Common.cellWidth
                  * Common.cellHeight - 1;
        }
      }

      // sort
      ArrayList<Triangle> triangles = new ArrayList<>();
      for (int l = 0; l <= Common.cellWidth
              * Common.cellHeight; l++) {
        for (int i = 0; i < faces.size(); i++) {
          if (faces.get(i).cell == l) {
            triangles.add(faces.get(i));
            faces.remove(i);
            i--;
          }
        }
      }
      Common.materials.get(j).faces = triangles;
      faces = triangles;

      // find pointers
      int index = 0;
      for (int i = 1; i < Common.cellWidth
              * Common.cellHeight; i++) {
        if (index < faces.size()) {
          while (faces.get(index).cell < i) {
            if (index == faces.size() - 1) {
              break;
            }
            index++;
          }
        }
        Common.materials.get(j).pointers.add(index);
      }
    }
    System.out.println("OK");

  }
}

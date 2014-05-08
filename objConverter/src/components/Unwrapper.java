package components;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import lightmap.MTLPair;
import lightmap.KDNode;
import geometry.Edge;
import geometry.Point2D;
import geometry.Triangle;

public class Unwrapper {

  public static boolean LIGHTMAP_OPTIMALIZE   = true;
  public static int     LIGHTMAP_MAX_TRIANGLE = 32;
  public static double  LIGHTMAP_PRECISION    = 8.5;
  static int res;
  static KDNode root;

  static ArrayList<MTLPair> lm = new ArrayList<>();
  static ArrayList<MTLLoader> skipped = new ArrayList<>();

  public static void unwrap() throws Exception {
    System.out.println("Generating lightmap coordinates...");
    int last = Integer.MAX_VALUE;
    do {
      action();
      System.out.println("Actual lightmap count: " + lm.size());
      if (last != lm.size()) {
        last = lm.size();
        for (int i = 0; i < lm.size(); i++) {
          if (lm.get(i).render != null) {
            lm.get(i).render.show(false);
          }
        }
        if (LIGHTMAP_OPTIMALIZE) {
          lm = new ArrayList<>();
          skipped = new ArrayList<>();
        }
      } else {
        break;
      }
    } while (LIGHTMAP_OPTIMALIZE);

    for (int i = 0; i < lm.size(); i++) {
      for (int j = 0; j < lm.get(i).mtls.size(); j++) {
        lm.get(i).mtls.get(j).parameters += "lm " + i;
      }
    }

    System.out.println("OK");
  }

  private static void action() throws Exception {

    join(Common.materials, 1);
    int lastSkipped = Integer.MAX_VALUE;
    while (!skipped.isEmpty()) {
      if (lastSkipped > skipped.size()) {
        lastSkipped = skipped.size();
      } else {
        for (int i = 0; i < Common.materials.size(); i++) {
          if (skipped.get(0).faces.size() == Common.materials.get(i).faces.size()) {
            boolean same = true;
            for (int j = 0; j < skipped.get(0).faces.size(); j++) {
              if (Common.materials.get(i).faces.get(j) != skipped.get(0).faces.get(j)) {
                same = false;
              }
            }
            if (same) {
              MTLLoader first = Common.materials.get(i).clone(true);
              MTLLoader second = Common.materials.get(i).clone(false);
              Common.materials.remove(i);
              skipped.remove(0);
              Common.materials.add(first);
              Common.materials.add(second);
              skipped.add(first);
              skipped.add(second);
              lastSkipped = Integer.MAX_VALUE;
              break;
            }
          }
        }
      }
      ArrayList<MTLLoader> faces = skipped;
      skipped = new ArrayList<>();
      join(faces, 0);
    }
  }

  private static boolean create(ArrayList<Triangle> faces) throws Exception {
    res = 2048;
    boolean ok;
    do {
      ok = true;

      // count scale
      double scale = 0.0;
      for (int i = 0; i < faces.size(); i++) {
        Triangle t = faces.get(i);
        double wt = 0;
        double wv = 0;
        wt += Edge.dist(t.a, t.b);
        wt += Edge.dist(t.a, t.c);
        wt += Edge.dist(t.b, t.c);
        wv += Point2D.dist(t.a, t.b);
        wv += Point2D.dist(t.a, t.c);
        wv += Point2D.dist(t.b, t.c);
        scale += wt / wv;
      }
      scale /= (double) faces.size();
      scale /= res / LIGHTMAP_PRECISION;

      // create nodes for triangles
      int size = 0;
      for (int i = 0; i < faces.size(); i++) {
        Triangle t = faces.get(i);
        double a = Edge.dist(t.b, t.c);
        double b = Edge.dist(t.a, t.c);
        double c = Edge.dist(t.a, t.b);
        if ((a >= b) && (a >= c)) {
          t.node = new KDNode((int) (b * scale + 1), (int) (c * scale + 1), true);
        }
        if ((b >= a) && (b >= c)) {
          t.node = new KDNode((int) (a * scale + 1), (int) (c * scale + 1), true);
        }
        if ((c >= a) && (c >= b)) {
          t.node = new KDNode((int) (a * scale + 1), (int) (b * scale + 1), true);
        }
        t.node.landscape();
        if (t.node.width > LIGHTMAP_MAX_TRIANGLE) {
          t.node.width = LIGHTMAP_MAX_TRIANGLE;
        }
        t.node.landscape();
        if (t.node.width > LIGHTMAP_MAX_TRIANGLE) {
          t.node.width = LIGHTMAP_MAX_TRIANGLE;
        }
        size += t.node.width * t.node.height / 2;
      }

      // do not build tree if instance is too heavy
      if (size > 256 * 256)
        return false;
      
      // sort triangles
      Collections.sort(faces, new Comparator<Triangle>() {
        @Override
        public int compare(Triangle b, Triangle a) {
          return a.node.width - b.node.width;
        }
      });

      // add geometry as it has texture coords
      root = new KDNode(256, 256, false);
      root.reset();
      for (int i = 0; i < faces.size(); i++) {
        Triangle t = faces.get(i);
        if (!root.addHalfNode(t)) {
          if (!root.addHalfNodeForce(t)) {
            if (!root.addNode(t)) {
              res *= 2;
              if (res >= 4096) {
                return false;
              }
              ok = false;
              break;
            }
          }
        }
      }
      root.countLM(0, 0);

    } while (!ok);
    return true;
  }

  public static void join(ArrayList<MTLLoader> materials, int start)
          throws Exception {

    System.out.print("Solving " + (materials.size() - start) + " objects");
    MTLPair p = new MTLPair();
    for (int j = start; j < materials.size(); j++) {
      ArrayList<Triangle> faces = materials.get(j).faces;
      if (!materials.get(j).parameters.contains("#")) {
        p.add(materials.get(j), faces);

        // create lightmap
        if (create(p.getFaces())) {
          p.apply(res);
          System.out.print(",");
        } else {
          skipped.add(p.mat);
          System.out.print(".");
        }
      }
    }
    System.out.println();
    lm.add(p);

    // show texture coordinates
    create(p.faces);
    //p.render = new Renderer().render(skipped.size() + " / " + p.res, root);
  }
}

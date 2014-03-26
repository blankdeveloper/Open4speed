package lightmap;

import geometry.Triangle;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Polygon;
import java.util.ArrayList;
import java.util.Random;

/**
 * Class representing nodes of kD tree
 */
public class KDNode {

  private static ArrayList<KDNode> secondHalf;
  private Triangle t1, t2;
  // random number generator
  static Random rnd = new Random();
  int freeSpace;
  public KDNode next1, next2;
  // node dimension
  public int width;
  public int height;

  // information if node is a list
  public boolean isList;
  // color of node
  Color color;
  Color color2;

  /**
   * create node
   *
   * @param width is node width
   * @param height is node height
   * @param isList is true if node is a list
   */
  public KDNode(int width, int height, boolean isList) {
    this.width = width;
    this.height = height;
    this.isList = isList;
    if (isList) {
      color = new Color(rnd.nextInt(254) + 1, rnd.nextInt(254) + 1,
              rnd.nextInt(254) + 1);
    } else {
      color = Color.BLACK;
    }
  }

  public boolean addHalfNode(Triangle t) {
    for (int i = 0; i < secondHalf.size(); i++) {
      KDNode m = secondHalf.get(i);
      if ((m.width == t.node.width) && (m.height == t.node.height)) {
        m.color2 = new Color(rnd.nextInt(254) + 1, rnd.nextInt(254) + 1,
                rnd.nextInt(254) + 1);
        secondHalf.remove(i);
        m.t2 = t;
        t.node = m;
        return true;
      }
    }
    return false;
  }

  public boolean addHalfNodeForce(Triangle t) {

    for (int i = 0; i < secondHalf.size(); i++) {
      KDNode m = secondHalf.get(i);
      if ((m.width >= t.node.width) && (m.height >= t.node.height)) {
        m.color2 = new Color(rnd.nextInt(254) + 1, rnd.nextInt(254) + 1,
                rnd.nextInt(254) + 1);
        secondHalf.remove(i);
        m.t2 = t;
        t.node = m;
        return true;
      }
    }
    return false;
  }

  public void reset() {
    secondHalf = new ArrayList<>();
  }

  /**
   * Insert node standard way
   *
   * @param t is a triangle to add
   * @return true if triangle was inserted
   * @throws Exception on incorrect data
   */
  public boolean addNode(Triangle t) throws Exception {
    // do not solve if it is a list
    if (isList) {
      return false;
    }

    // node is empty -> create left subnode
    if (next1 == null) {
      // check space
      if ((t.node.width <= width) && (t.node.height <= height)) {

        // fits exactly
        if ((t.node.width == width) || (t.node.height == height)) {
          next1 = t.node;
          secondHalf.add(t.node);
          next1.t1 = t;
          return true;
        } // space division
        else {
          next1 = new KDNode(t.node.width, height, false);
          if (next1.addNode(t)) {
            return true;
          }
        }
      }
    } // try to insert into left subnode
    else if (next1.addNode(t)) {
      return true;
    } // create right subnode
    else if (next2 == null) {

      // horizontal space division
      if ((t.node.width <= width) && (t.node.height <= height - next1.height)) {
        next2 = new KDNode(width, height - next1.height, false);
        if (next2.addNode(t)) {
          return true;
        }
      }

      // vertical space division
      if ((t.node.width <= width - next1.width) && (t.node.height <= height)) {
        next2 = new KDNode(width - next1.width, height, false);
        if (next2.addNode(t)) {
          return true;
        }
      }
    } // try to insert into right subnode
    else if (next2.addNode(t)) {
      return true;
    }
    return false;
  }

  public void countLM(int x, int y) throws Exception {
    if (t1 != null) {
      t1.lightmapX = x;
      t1.lightmapY = y;
      t1.lightmapU = t1.node.width;
      t1.lightmapV = t1.node.height;
    }
    if (t2 != null) {
      t2.lightmapU = -t2.node.width;
      t2.lightmapV = -t2.node.height;
      t2.lightmapX = x - t2.lightmapU;
      t2.lightmapY = y - t2.lightmapV;
    }

    if (next1 != null) {
      next1.countLM(x, y);
    }
    if (next2 != null) {
      if (next1 == null) {
        throw new Exception("Chybne poradi uzlu");
      }
      if (next1.width == width) {
        next2.countLM(x, y + next1.height);
      } else if (next2.height == height) {
        next2.countLM(x + next1.width, y);
      } else {
        throw new Exception("Chybna konzistence kD stromu");
      }
    }
  }

  /**
   * Render curret node and subnodes into window
   *
   * @param g is an instance of Graphics of JPanel
   * @param x is top left corner X
   * @param y is top left corner Y
   * @param s is scale factor
   * @throws Exception on incorrect data
   */
  public void draw(Graphics g, int x, int y, int s) throws Exception {
    if (t1 != null) {
      g.setColor(color);
      Polygon p = new Polygon();
      p.addPoint(t1.lightmapX * s, t1.lightmapY * s);
      p.addPoint(t1.lightmapX * s + t1.lightmapU * s, t1.lightmapY * s);
      p.addPoint(t1.lightmapX * s, t1.lightmapY * s + t1.lightmapV * s);
      g.fillPolygon(p);
    }

    if (t2 != null) {
      g.setColor(color2);
      Polygon p = new Polygon();
      p.addPoint(t2.lightmapX * s, t2.lightmapY * s);
      p.addPoint(t2.lightmapX * s + t2.lightmapU * s, t2.lightmapY * s);
      p.addPoint(t2.lightmapX * s, t2.lightmapY * s + t2.lightmapV * s);
      g.fillPolygon(p);

    }

    if (next1 != null) {
      next1.draw(g, x, y, s);
    }
    if (next2 != null) {
      if (next1 == null) {
        throw new Exception("Chybne poradi uzlu");
      }
      if (next1.width == width) {
        next2.draw(g, x, y + next1.height, s);
      } else if (next2.height == height) {
        next2.draw(g, x + next1.width, y, s);
      } else {
        throw new Exception("Chybna konzistence kD stromu");
      }
    }
  }

  int getFreeSpace() {
    if (next1 == null) {
      if (isList) {
        freeSpace = 0;
      } else {
        freeSpace = size();
      }
    } else if (next2 == null) {
      freeSpace = size() - next1.size() + next1.getFreeSpace();
    } else {
      freeSpace = next1.getFreeSpace() + next2.getFreeSpace();
    }
    return freeSpace;
  }

  /**
   * Set node landscaped
   */
  public void landscape() {
    if (width < height) {
      rotate();
    }
  }

  /**
   * Rotate node
   */
  void rotate() {
    int temp = width;
    width = height;
    height = temp;
  }

  /**
   * Return area of node
   *
   * @return area of node
   */
  int size() {
    return width * height;
  }
}

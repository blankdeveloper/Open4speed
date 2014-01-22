package lightmap;

import geometry.Triangle;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Polygon;
import java.util.ArrayList;
import java.util.Random;

/**
 * Trida uzel je trida reprezentujici uzly kD stromu a operace nad nimi
 *
 * @author Lubos Vonasek
 *
 */
public class Node {

  private static ArrayList<Node> secondHalf;
  private Triangle t1, t2;
  // generator nahodnych cisel
  static Random rnd = new Random();
  int freeSpace;
  // potomci uzlu
  public Node next1, next2;
  // rozmery uzlu
  public int width;
  public int height;

  // informace zda je uzel list
  public boolean isList;
  // barva uzlu
  Color color;
  Color color2;

  /**
   * Vytvori novy uzel
   *
   * @param width je sirka listu
   * @param height je vyska listu
   * @param isList je informace zda se jedna o list
   */
  public Node(int width, int height, boolean isList) {
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
      Node m = secondHalf.get(i);
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
      Node m = secondHalf.get(i);
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
   * Vlozi uzel standartne
   *
   * @param t
   * @return true pokud byl vlozen
   * @throws Exception pri nekorektnich datech
   */
  public boolean addNode(Triangle t) throws Exception {
    // jedna se o list -> neresit
    if (isList) {
      return false;
    }

    // uzel je prazdny -> vytvoreni leve vetve
    if (next1 == null) {
      // kontrola volneho mista
      if ((t.node.width <= width) && (t.node.height <= height)) {

        // padne presne
        if ((t.node.width == width) || (t.node.height == height)) {
          next1 = t.node;
          secondHalf.add(t.node);
          next1.t1 = t;
          return true;
        } // dalsi deleni roviny
        else {
          next1 = new Node(t.node.width, height, false);
          if (next1.addNode(t)) {
            return true;
          }
        }
      }
    } // zkusit vlozit do leve vetve
    else if (next1.addNode(t)) {
      return true;
    } // vytvoreni prave vetve
    else if (next2 == null) {

      // deleni roviny horizontalne
      if ((t.node.width <= width) && (t.node.height <= height - next1.height)) {
        next2 = new Node(width, height - next1.height, false);
        if (next2.addNode(t)) {
          return true;
        }
      }

      // deleni roviny vertikalne
      if ((t.node.width <= width - next1.width) && (t.node.height <= height)) {
        next2 = new Node(width - next1.width, height, false);
        if (next2.addNode(t)) {
          return true;
        }
      }
    } // zkusit vlozit do prave vetve
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
   * Vykresli aktualni uzel i s poduzly do okna
   *
   * @param g je instance Graphics z JPanelu
   * @param x je levy horni roh
   * @param y je levy horni roh
   * @param s
   * @throws Exception pri nekorektnich datech
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
   * Nastavi uzel na sirku
   */
  public void landscape() {
    if (width < height) {
      rotate();
    }
  }

  /**
   * Otoci uzel
   */
  void rotate() {
    int temp = width;
    width = height;
    height = temp;
  }

  /**
   * Vrati velikost uzlu
   *
   * @return velikost uzlu(obsah)
   */
  int size() {
    return width * height;
  }
}

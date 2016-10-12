import geometry.Edge;
import geometry.Graph;
import geometry.Model;
import geometry.Point2D;
import geometry.Point3D;
import geometry.Triangle;
import geometry.Vertex;

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

public class ObjLoader
{
  private String               prefix             = " ";
  private String               currentMaterialLib = "";
  private String               path               = "";
  private boolean              top                = false;

  // variables to count extreme coordinates of model
  private float                minx               = Float.MAX_VALUE;
  private float                miny               = Float.MAX_VALUE;
  private float                minz               = Float.MAX_VALUE;
  private float                maxx               = Float.MIN_VALUE;
  private float                maxy               = Float.MIN_VALUE;
  private float                maxz               = Float.MIN_VALUE;

  // create storage in memory for OBJ model
  private ArrayList<Point3D>   vertices           = new ArrayList<>();
  private ArrayList<Point3D>   normals            = new ArrayList<>();
  private ArrayList<Point2D>   texCoords          = new ArrayList<>();
  private ArrayList<Model>     models             = new ArrayList<>();
  private ArrayList<Graph>     graph              = new ArrayList<>();

  public ObjLoader(String path)
  {
    this.path = path;
  }

  // get material parameters from library
  private String getMaterial(String lib, String mtl, String path) throws IOException
  {
    top = false;
    // open material library file
    FileInputStream fis;
    try
    {
      fis = new FileInputStream(lib);
    } catch (FileNotFoundException e)
    {
      System.err.println(lib + " doesn't exists.");
      return "(null)";
    }
    String phong;
    try (BufferedReader sc = new BufferedReader(new InputStreamReader(fis)))
    {
      phong = "";
      while (sc.ready())
      {
        // read one line of library
        String data = sc.readLine();

        // read parameters of material
        if (data.equals("newmtl " + mtl))
        {
          // set default variables
          float ar = 0, ag = 0, ab = 0;
          float dr = 0, dg = 0, db = 0;
          float sr = 0, sg = 0, sb = 0;
          float alpha = 1;

          // parse material data
          while (sc.ready())
          {
            // read one line of material data
            data = sc.readLine();

            // set ambient parameter
            if (data.startsWith("Ka "))
            {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              ar = stringToFloat(lsc.nextToken());
              ag = stringToFloat(lsc.nextToken());
              ab = stringToFloat(lsc.nextToken());
            }

            // set diffuse parameter
            if (data.startsWith("Kd "))
            {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              dr = stringToFloat(lsc.nextToken());
              dg = stringToFloat(lsc.nextToken());
              db = stringToFloat(lsc.nextToken());
            }

            // set specular parameter
            if (data.startsWith("Ks "))
            {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              sr = stringToFloat(lsc.nextToken());
              sg = stringToFloat(lsc.nextToken());
              sb = stringToFloat(lsc.nextToken());
            }

            // set alpha parameter
            if (data.startsWith("d "))
            {
              StringTokenizer lsc = new StringTokenizer(data);
              lsc.nextToken();
              alpha = stringToFloat(lsc.nextToken());
            }

            // compress parameters
            phong = ar + " " + ag + " " + ab + " ";
            phong += dr + " " + dg + " " + db + " ";
            phong += sr + " " + sg + " " + sb + " ";
            phong += alpha;

            // if transparent place it to the top
            if (alpha < 1)
            {
              top = true;
            }

            // parse material texture
            if (data.startsWith("map_Kd "))
            {

              // create name of processed texture
              int fileStart = data.lastIndexOf('/') + 1;
              if (!data.contains("/"))
              {
                fileStart = 7;
              }
              String textureName = data.substring(fileStart, data.length());
              String fileName = data.substring(fileStart, data.length());
              // cut extension
              StringTokenizer lsc = new StringTokenizer(textureName);
              String cut = lsc.nextToken();
              if (cut.contains("."))
              {
                cut = cut.substring(0, cut.indexOf('.'));
              }
              textureName = path + "/" + cut + ".png";

              // check if texture is not already processed
              if (!exists(textureName))
              {
                // get image of texture
                BufferedImage img = null;
                BufferedImage resizedImage;
                try
                {
                  if (exists(data.substring(7)))
                  {
                    img = ImageIO.read(new File(data.substring(7)));
                  } else if (exists(fileName))
                  {
                    img = ImageIO.read(new File(fileName));
                  }

                  if (img == null)
                  {
                    System.err.println("Unable to find/open " + fileName);
                    return "(null)";
                  }
                  // get type of texture
                  int type = img.getType();
                  // scale texture
                  int w = 2;
                  int h = 2;
                  while (w < img.getWidth())
                  {
                    w *= 2;
                  }
                  while (h < img.getHeight())
                  {
                    h *= 2;
                  }
                  // resize texture
                  resizedImage = new BufferedImage(w, h, type);
                  Graphics2D g = resizedImage.createGraphics();
                  g.drawImage(img, 0, 0, w, h, null);
                  g.dispose();
                } catch (IOException e)
                {
                  System.err.println("Error parsing " + fileName);
                  fis.close();
                  sc.close();
                  return "(null)";
                }
                // save resized texture
                try
                {
                  ImageIO.write(resizedImage, "png", new File(textureName));
                  if (!exists(textureName))
                  {
                    System.err.println("Unsupported texture format " + fileName);
                    fis.close();
                    sc.close();
                    return "(null)";
                  }
                } catch (IOException e)
                {
                  System.err.println("Unable to write " + textureName);
                  fis.close();
                  sc.close();
                  return "(null)";
                }
              }
              fis.close();
              sc.close();
              return cut + ".png " + phong;
            }

            // end of the material
            if (data.startsWith("newmtl "))
            {
              break;
            }
          }
        }
      }
      fis.close();
    }
    return "* " + phong;
  }

  public void loadObj(String inputFile) throws IOException
  {

    System.out.print("Loading file into memory...");

    // open input file
    FileInputStream fis;
    try
    {
      fis = new FileInputStream(inputFile);
    } catch (FileNotFoundException ex)
    {
      System.err.println("Unable to open input file");
      return;
    }
    BufferedReader sc = new BufferedReader(new InputStreamReader(fis));

    // for broken faces create null material
    models.add(new Model("(null)"));
    // set link for saving edges
    graph.add(new Graph());

    while (sc.ready())
    {
      // read one line of file
      String line = sc.readLine();

      // set prefix of material
      if (line.startsWith("o "))
      {
        // new graph for tracks/lights
        graph.add(new Graph());
        // material prefix
        prefix = line.substring(1);
        if (!prefix.contains("#") && !prefix.contains("$") && !prefix.contains("%") && !prefix.contains("!"))
        {
          prefix = " ";
        }
      }

      // set material library
      if (line.startsWith("mtllib "))
      {
        String mtlPath = inputFile.substring(0, inputFile.lastIndexOf('/') + 1);
        currentMaterialLib = mtlPath + line.substring(7);
      }

      // change current material
      if (line.startsWith("usemtl "))
      {
        // get parameters of material
        String material = getMaterial(currentMaterialLib, line.substring(7), path);
        material += prefix;
        // check if material already exists
        boolean assigned = false;
        for (int i = 0; i < models.size(); i++)
        {
          if (models.get(i).material.compareTo(material) == 0)
          {
            assigned = true;
          }
        }
        // if material doesn't exist then create it
        if (!assigned)
        {
          if (top)
          {
            models.add(new Model(material));
          } else
          {
            models.add(1, new Model(material));
          }
        }
      }

      // add vertex into memory
      if (line.startsWith("v "))
      {
        StringTokenizer lsc = new StringTokenizer(line);
        lsc.nextToken();
        // get values
        float x = stringToFloat(lsc.nextToken());
        float y = stringToFloat(lsc.nextToken());
        float z = stringToFloat(lsc.nextToken());
        // update extreme values
        if (x < minx)
        {
          minx = x;
        }
        if (y < miny)
        {
          miny = y;
        }
        if (z < minz)
        {
          minz = z;
        }
        if (x > maxx)
        {
          maxx = x;
        }
        if (y > maxy)
        {
          maxy = y;
        }
        if (z > maxz)
        {
          maxz = z;
        }
        // add it into memory
        vertices.add(new Point3D(x, y, z));
      }

      // add normal into memory
      if (line.startsWith("vn "))
      {
        StringTokenizer lsc = new StringTokenizer(line);
        lsc.nextToken();
        // get values
        float x = stringToFloat(lsc.nextToken());
        float y = stringToFloat(lsc.nextToken());
        float z = stringToFloat(lsc.nextToken());
        // add it into memory
        normals.add(new Point3D(x, y, z));
      }

      // add texture coordinate into memory
      if (line.startsWith("vt "))
      {
        StringTokenizer lsc = new StringTokenizer(line);
        lsc.nextToken();
        // get values
        float x = stringToFloat(lsc.nextToken());
        float y = stringToFloat(lsc.nextToken());
        // add it into memory
        texCoords.add(new Point2D(x, y));
      }
    }
    fis.close();
    System.out.println("OK");
  }

  // check if file exists
  public static boolean exists(String filename)
  {
    try
    {
      FileInputStream fis = new FileInputStream(filename);
      if (fis.available() > 0)
      {
        fis.close();
        return true;
      } else
      {
        fis.close();
        return false;
      }
    } catch (IOException e)
    {
      return false;
    }
  }

  public void parseObj(String inputFile) throws IOException
  {

    System.out.print("Parsing OBJ file...");
    ArrayList<Triangle> faces = null;
    int graphIndex = 0;
    FileInputStream fis;
    try
    {
      fis = new FileInputStream(inputFile);
    } catch (FileNotFoundException ex)
    {
      System.err.println("Unable to open input file");
      return;
    }
    BufferedReader sc = new BufferedReader(new InputStreamReader(fis));

    // parse all faces
    while (sc.ready())
    {
      // read one line of file
      String line = sc.readLine();

      // set prefix of material
      if (line.startsWith("o "))
      {
        graphIndex++;
        prefix = line.substring(1);
        if (!prefix.contains("#") && !prefix.contains("$") && !prefix.contains("%") && !prefix.contains("!"))
        {
          prefix = " ";
        }
      }

      // set material library
      if (line.startsWith("mtllib "))
      {
        String mtlPath = inputFile.substring(0, inputFile.lastIndexOf('/') + 1);
        currentMaterialLib = mtlPath + line.substring(7);
      }

      // change current material
      if (line.startsWith("usemtl "))
      {
        // get parameters of material
        String material = getMaterial(currentMaterialLib, line.substring(7), path);
        material += prefix;
        // check if material already exists
        boolean assigned = false;
        for (int i = 0; i < models.size(); i++)
        {
          if (models.get(i).material.compareTo(material) == 0)
          {
            faces = models.get(i).faces;
            assigned = true;
          }
        }
        // if material doesn't exist then create it
        if (!assigned)
        {
          if (top)
          {
            models.add(new Model(material));
          } else
          {
            models.add(1, new Model(material));
          }
          faces = models.get(models.size() - 1).faces;
        }
      }

      // add edge into memory
      if (line.startsWith("l "))
      {
        StringTokenizer lsc = new StringTokenizer(line);
        lsc.nextToken();
        // create object for points
        Vertex[] point = new Vertex[4];
        // set index of current point
        int pointsCount = 0;
        // parse vertices
        while (lsc.hasMoreTokens())
        {
          // get point as scanner
          StringTokenizer vsc = new StringTokenizer(lsc.nextToken().replace('/', ' '));

          // convert value into point
          Point3D vertex = vertices.get(Integer.valueOf(vsc.nextToken()) - 1);
          Point2D texCoord = null;
          if (vsc.hasMoreTokens() & (texCoords.size() > 0))
          {
            int index = Integer.valueOf(vsc.nextToken()) - 1;
            if (index < texCoords.size())
            {
              texCoord = texCoords.get(index);
            }
          }
          Point3D normal = null;
          if (vsc.hasMoreTokens() & (normals.size() > 0))
          {
            normal = normals.get(Integer.valueOf(vsc.nextToken()) - 1);
          }

          // save point
          point[pointsCount++] = new Vertex(normal, texCoord, vertex);
        }
        if (pointsCount == 2)
        {
          graph.get(graphIndex).edges.add(new Edge(point[0].v, point[1].v));
        }
      }

      // add face into memory
      if (line.startsWith("f "))
      {
        StringTokenizer lsc = new StringTokenizer(line);
        lsc.nextToken();
        // create object for points
        Vertex[] point = new Vertex[4];
        // set index of current point
        int pointsCount = 0;
        // parse vertices
        while (lsc.hasMoreTokens())
        {
          // get point as scanner
          StringTokenizer vsc = new StringTokenizer(lsc.nextToken().replace('/', ' '));

          // convert value into point
          Point3D vertex = vertices.get(Integer.valueOf(vsc.nextToken()) - 1);
          Point2D texCoord = null;
          if (vsc.hasMoreTokens() & (texCoords.size() > 0))
          {
            int index = Integer.valueOf(vsc.nextToken()) - 1;
            if (index < texCoords.size())
            {
              texCoord = texCoords.get(index);
            }
          }
          Point3D normal = null;
          if (vsc.hasMoreTokens() & (normals.size() > 0))
          {
            normal = normals.get(Integer.valueOf(vsc.nextToken()) - 1);
          }

          // save point
          point[pointsCount++] = new Vertex(normal, texCoord, vertex);
        }

        switch (pointsCount)
        {
        case (2):
          graph.get(graphIndex).edges.add(new Edge(point[0].v, point[1].v));
          break;
        // save triangle
        case (3):
          faces.add(new Triangle(point[0], point[1], point[2]));
          break;
        // save polygons(separate it to two triangles)
        case (4):
          faces.add(new Triangle(point[3], point[0], point[1]));
          faces.add(new Triangle(point[3], point[1], point[2]));
          break;
        }
      }
    }

    fis.close();
    System.out.println("OK");
  }

  public String getExtremes()
  {
    String extremes = minx + " " + miny + " " + minz + " ";
    extremes += maxx + " " + maxy + " " + maxz + "\n";
    return extremes;
  }

  public ArrayList<Graph> getGraphs()
  {
    return graph;
  }

  public ArrayList<Model> getModels()
  {
    return models;
  }

  // convert string to float
  private float stringToFloat(String value)
  {
    try
    {
      return Float.valueOf(value);
    } catch (NumberFormatException e)
    {
      return 0;
    }
  }
}

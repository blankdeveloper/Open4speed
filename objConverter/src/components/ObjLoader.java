package components;

import geometry.Edge;
import geometry.Graph;
import geometry.Point2D;
import geometry.Point3D;
import geometry.Triangle;
import geometry.Vertex;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class ObjLoader
{
  private String               prefix             = " ";
  private String               currentMaterialLib = "";
  private String               path               = "";

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
  private ArrayList<MTLLoader> materials          = new ArrayList<>();
  private ArrayList<Graph>     graph              = new ArrayList<>();
  private FileOutputStream     fos                = null;

  public ObjLoader(String path)
  {
    this.path = path;
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
    materials.add(new MTLLoader("(null)"));
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
        String material = MTLLoader.getMaterial(currentMaterialLib, line.substring(7), path);
        material += prefix;
        // check if material already exists
        boolean assigned = false;
        for (int i = 0; i < materials.size(); i++)
        {
          if (materials.get(i).parameters.compareTo(material) == 0)
          {
            assigned = true;
          }
        }
        // if material doesn't exist then create it
        if (!assigned)
        {
          if (MTLLoader.top)
          {
            materials.add(new MTLLoader(material));
          } else
          {
            materials.add(1, new MTLLoader(material));
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
        String material = MTLLoader.getMaterial(currentMaterialLib, line.substring(7), path);
        material += prefix;
        // check if material already exists
        boolean assigned = false;
        for (int i = 0; i < materials.size(); i++)
        {
          if (materials.get(i).parameters.compareTo(material) == 0)
          {
            faces = materials.get(i).faces;
            assigned = true;
          }
        }
        // if material doesn't exist then create it
        if (!assigned)
        {
          if (MTLLoader.top)
          {
            materials.add(new MTLLoader(material));
          } else
          {
            materials.add(1, new MTLLoader(material));
          }
          faces = materials.get(materials.size() - 1).faces;
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

  public void writeO4S(String outputFile)
  {

    System.out.print("Writing output file...");
    ArrayList<Triangle> faces;

    // count faces
    int facesCount = 0;
    for (int j = 1; j < materials.size(); j++)
    {
      // check if current material has faces
      if (materials.get(j).faces.size() > 0)
      {
        facesCount++;
      }
    }
    // count edges
    int edgesCount = 0;
    for (int j = 0; j < graph.size(); j++)
    {
      // check if current graph has edges
      if (graph.get(j).edges.size() > 0)
      {
        edgesCount++;
      }
    }

    try
    {
      fos = new FileOutputStream(outputFile);

      // save extreme values
      String extremes = minx + " " + miny + " " + minz + " ";
      extremes += maxx + " " + maxy + " " + maxz + "\n";
      fos.write((extremes).getBytes());

      // save faces count
      fos.write((facesCount + "\n").getBytes());

      // save all faces
      for (int j = 1; j < materials.size(); j++)
      {
        // get faces pointer for current material
        faces = materials.get(j).faces;
        // find local center
        minx = 99999;
        miny = 99999;
        minz = 99999;
        maxx = -99999;
        maxy = -99999;
        maxz = -99999;
        for (int i = 0; i < faces.size(); i++)
        {
          if (minx > faces.get(i).a.v.x)
          {
            minx = faces.get(i).a.v.x;
          }
          if (miny > faces.get(i).a.v.y)
          {
            miny = faces.get(i).a.v.y;
          }
          if (minz > faces.get(i).a.v.z)
          {
            minz = faces.get(i).a.v.z;
          }

          if (maxx < faces.get(i).a.v.x)
          {
            maxx = faces.get(i).a.v.x;
          }
          if (maxy < faces.get(i).a.v.y)
          {
            maxy = faces.get(i).a.v.y;
          }
          if (maxz < faces.get(i).a.v.z)
          {
            maxz = faces.get(i).a.v.z;
          }

          if (minx > faces.get(i).b.v.x)
          {
            minx = faces.get(i).b.v.x;
          }
          if (miny > faces.get(i).b.v.y)
          {
            miny = faces.get(i).b.v.y;
          }
          if (minz > faces.get(i).b.v.z)
          {
            minz = faces.get(i).b.v.z;
          }

          if (maxx < faces.get(i).b.v.x)
          {
            maxx = faces.get(i).b.v.x;
          }
          if (maxy < faces.get(i).b.v.y)
          {
            maxy = faces.get(i).b.v.y;
          }
          if (maxz < faces.get(i).b.v.z)
          {
            maxz = faces.get(i).b.v.z;
          }

          if (minx > faces.get(i).c.v.x)
          {
            minx = faces.get(i).c.v.x;
          }
          if (miny > faces.get(i).c.v.y)
          {
            miny = faces.get(i).c.v.y;
          }
          if (minz > faces.get(i).c.v.z)
          {
            minz = faces.get(i).c.v.z;
          }

          if (maxx < faces.get(i).c.v.x)
          {
            maxx = faces.get(i).c.v.x;
          }
          if (maxy < faces.get(i).c.v.y)
          {
            maxy = faces.get(i).c.v.y;
          }
          if (maxz < faces.get(i).c.v.z)
          {
            maxz = faces.get(i).c.v.z;
          }
        }

        // check if current material has faces
        if (faces.size() > 0)
        {
          // save material parameters
          fos.write((minx + " " + miny + " " + minz + " " + maxx + " " + maxy + " " + maxz + " "
              + materials.get(j).parameters + "\n").getBytes());
          // save faces count
          fos.write((faces.size() + "\n").getBytes());
          // save face parameters
          for (int i = 0; i < faces.size(); i++)
          {
            faces.get(i).a.v.x -= minx;
            faces.get(i).a.v.y -= miny;
            faces.get(i).a.v.z -= minz;
            faces.get(i).b.v.x -= minx;
            faces.get(i).b.v.y -= miny;
            faces.get(i).b.v.z -= minz;
            faces.get(i).c.v.x -= minx;
            faces.get(i).c.v.y -= miny;
            faces.get(i).c.v.z -= minz;
            fos.write(faces.get(i).value());
            faces.get(i).a.v.x += minx;
            faces.get(i).a.v.y += miny;
            faces.get(i).a.v.z += minz;
            faces.get(i).b.v.x += minx;
            faces.get(i).b.v.y += miny;
            faces.get(i).b.v.z += minz;
            faces.get(i).c.v.x += minx;
            faces.get(i).c.v.y += miny;
            faces.get(i).c.v.z += minz;
          }
        }
      }
      fos.close();

      // save edges count
      fos = new FileOutputStream(outputFile + ".e");
      fos.write((edgesCount + "\n").getBytes());

      // save all edges
      for (int j = 0; j < graph.size(); j++)
      {
        // get edges pointer for current material
        ArrayList<Edge> edges = graph.get(j).edges;
        // check if current material has edges
        if (edges.size() > 0)
        {
          // save edges count
          fos.write((edges.size() + "\n").getBytes());
          // save face parameters
          for (int i = 0; i < edges.size(); i++)
          {
            fos.write(edges.get(i).value());
          }
        }
      }
    } catch (IOException e)
    {
      System.err.println("Unable to save file");
      return;
    }
    System.out.println("OK");

    // show information
    if (materials.get(0).faces.size() > 0)
    {
      System.out.println(materials.get(0).faces.size() + " polygons weren't parsed.");
    }
    Vertex.printStatistics();

    if (facesCount > 0)
    {
      System.out.println("Converted " + facesCount + " triangle objects");
      if (edgesCount > 0)
      {
        System.out.println("Converted " + edgesCount + " edge tracks");
      }
    } else
    {
      System.out.println("Nothing converted");
    }
  }

  // convert string to float
  public static float stringToFloat(String value)
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

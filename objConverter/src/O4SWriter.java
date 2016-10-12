import geometry.Edge;
import geometry.Graph;
import geometry.Model;
import geometry.Triangle;
import geometry.Vertex;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;


public class O4SWriter
{
  private String outputFile;
  
  public O4SWriter(String outputFile)
  {
    this.outputFile = outputFile;
  }
  
  public void write(ArrayList<Model> models, String extremes)
  {
    System.out.print("Writing output file...");

    // count faces
    int facesCount = 0;
    for (int j = 1; j < models.size(); j++)
    {
      // check if current material has faces
      if (models.get(j).faces.size() > 0)
      {
        facesCount++;
      }
    }

    try
    {
      FileOutputStream fos = new FileOutputStream(outputFile);

      // save extreme values
      fos.write(extremes.getBytes());

      // save faces count
      fos.write((facesCount + "\n").getBytes());

      // save all faces
      for (int j = 1; j < models.size(); j++)
      {
        // get faces pointer for current material
        ArrayList<Triangle> faces = models.get(j).faces;
        // find local center
        float minx = 99999;
        float miny = 99999;
        float minz = 99999;
        float maxx = -99999;
        float maxy = -99999;
        float maxz = -99999;
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
              + models.get(j).material + "\n").getBytes());
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
    } catch (IOException e)
    {
      System.err.println("Unable to save file");
      return;
    }
    System.out.println("OK");

    // show information
    if (models.get(0).faces.size() > 0)
    {
      System.out.println(models.get(0).faces.size() + " polygons weren't parsed.");
    }
    Vertex.printStatistics();

    if (facesCount > 0)
    {
      System.out.println("Converted " + facesCount + " triangle objects");
    } else
    {
      System.out.println("Nothing converted");
    }
  }

  public void writeEdges(ArrayList<Graph> graph)
  {
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
      // save edges count
      FileOutputStream fos = new FileOutputStream(outputFile + ".e");
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
      fos.close();
    } catch (IOException e)
    {
      System.err.println("Unable to save file");
      return;
    }
    
    if (edgesCount > 0)
    {
      System.out.println("Converted " + edgesCount + " edge tracks");
    }
  }
}

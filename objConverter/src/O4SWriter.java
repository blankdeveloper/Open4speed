import geometry.AABB;
import geometry.Edge;
import geometry.Graph;
import geometry.Model;
import geometry.Point3D;
import geometry.Triangle;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

public class O4SWriter
{
  public void write(String outputFile, ArrayList<Model> models, String extremes)
  {
    // count faces
    int facesCount = 0;
    for (int j = 0; j < models.size(); j++)
    {
      if(models.get(j).material.contains("(null)"))
        continue;
      // check if current material has faces
      if (models.get(j).faces.size() > 0)
        facesCount++;
    }

    try
    {
      FileOutputStream fos = new FileOutputStream(outputFile);

      // save extreme values
      fos.write(extremes.getBytes());

      // save faces count
      fos.write((facesCount + "\n").getBytes());

      // save all faces
      for (int j = 0; j < models.size(); j++)
      {
        if(models.get(j).material.contains("(null)"))
          continue;
        // get faces pointer for current material
        ArrayList<Triangle> faces = models.get(j).faces;
        AABB aabb = models.get(j).getAABB();
        Point3D min = aabb.getMin();
        Point3D max = aabb.getMax();

        // check if current material has faces
        if (faces.size() > 0)
        {
          // save material parameters
          fos.write((min.x + " " + min.y + " " + min.z + " " + max.x + " " + max.y + " " + max.z + " "
              + models.get(j).material + "\n").getBytes());
          // save faces count
          fos.write((faces.size() + "\n").getBytes());
          // save face parameters
          for (int i = 0; i < faces.size(); i++)
          {
            faces.get(i).a.v.x -= min.x;
            faces.get(i).a.v.y -= min.y;
            faces.get(i).a.v.z -= min.z;
            faces.get(i).b.v.x -= min.x;
            faces.get(i).b.v.y -= min.y;
            faces.get(i).b.v.z -= min.z;
            faces.get(i).c.v.x -= min.x;
            faces.get(i).c.v.y -= min.y;
            faces.get(i).c.v.z -= min.z;
            fos.write(faces.get(i).value());
            faces.get(i).a.v.x += min.x;
            faces.get(i).a.v.y += min.y;
            faces.get(i).a.v.z += min.z;
            faces.get(i).b.v.x += min.x;
            faces.get(i).b.v.y += min.y;
            faces.get(i).b.v.z += min.z;
            faces.get(i).c.v.x += min.x;
            faces.get(i).c.v.y += min.y;
            faces.get(i).c.v.z += min.z;
          }
        }
      }
      fos.close();
    } catch (IOException e)
    {
      System.err.println("Unable to save file");
      return;
    }
  }

  public void writeEdges(String outputFile, ArrayList<Graph> graph)
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
  }
}

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
    for (Model m : models)
    {
      if (m.material.contains("(null)"))
        continue;
      // check if current material has faces
      if (m.faces.size() > 0)
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
      AABB zero = new AABB(new Point3D(0, 0, 0), new Point3D(0, 0, 0));
      for (Model m : models)
      {
        if (m.material.contains("(null)"))
          continue;
        // get faces pointer for current material
        ArrayList<Triangle> faces = m.faces;
        // get bounding box
        AABB aabb;
        if (m.isDynamic())
          aabb = m.getAABB();
        else
          aabb = zero;
        Point3D min = aabb.getMin();
        Point3D max = aabb.getMax();

        // check if current material has faces
        if (faces.size() > 0)
        {
          // save material parameters
          fos.write((min.x + " " + min.y + " " + min.z + " " + max.x + " " + max.y + " " + max.z + " ").getBytes());
          fos.write((m.material + "\n").getBytes());
          // save faces
          fos.write((faces.size() + "\n").getBytes());
          for (Triangle t : faces)
            fos.write(t.value(min));
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

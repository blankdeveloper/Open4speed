import geometry.Id3D;
import geometry.Model;
import geometry.Triangle;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class Subdivider
{
  public Map<String, ArrayList<Model> > subdivide(ArrayList<Model> models)
  {
    System.out.print("Subdividing model...");
    // subdivide too big triangles
    /*for (int i = 0; i < models.size(); i++)
    {
      Model m = models.get(i);
      for (int j = 0; j < m.faces.size();)
      {
        Triangle t = m.faces.get(j);
        if (t.isTooBig())
        {
          Triangle.subdivide(m.faces, t);
          m.faces.remove(j);
        }
        else
          j++;
      }
    }*/
    
    Id3D id = new Id3D();
    Map<String, ArrayList<Model> >  output = new HashMap<String, ArrayList<Model> >();
    for(int i = 0; i < models.size(); i++)
    {
      // add dynamic models
      Model m = models.get(i);
      if(m.isDynamic())
      {
        id.set(m.getAABB());
        if(!output.containsKey(id.value()))
          output.put(id.value(), new ArrayList<Model>());
        output.get(id.value()).add(m);
      }
      // add static models
      else
      {
        ArrayList<Triangle> faces = null;
        Set<String> set = new HashSet<String>(); 
        for(Triangle t : m.faces)
        {
          if(id.set(t) || (faces == null))
          {
            if(!set.contains(id.value()))
            {
              if(!output.containsKey(id.value()))
                output.put(id.value(), new ArrayList<Model>());
              output.get(id.value()).add(new Model(m.material)); 
              set.add(id.value());
            }
            int last = output.get(id.value()).size() - 1;
            faces = output.get(id.value()).get(last).faces;
          }
          faces.add(t);
        }
      }
    }
    System.out.println("OK");
    return output;
  }
}

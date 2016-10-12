package geometry;

import java.util.ArrayList;


// storage for models
public class Model
{

  public ArrayList<Triangle> faces;
  public String              material;

  public Model(String t)
  {
    faces = new ArrayList<Triangle>();
    material = t;
  }
}

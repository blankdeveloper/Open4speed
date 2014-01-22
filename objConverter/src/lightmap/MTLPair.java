package lightmap;

import geometry.Triangle;

import java.util.ArrayList;

import components.MTLLoader;
import java.util.Collections;
import java.util.Comparator;
import javax.swing.JFrame;

public class MTLPair {

	public ArrayList<Triangle> faces;
	public ArrayList<MTLLoader> mtls;
	
	ArrayList<Triangle> tempFaces;
	public MTLLoader mat;
    public JFrame render;
	public int res;
	
	public MTLPair() {
		faces = new ArrayList<>();
		mtls = new ArrayList<>();
	}
	
	public void add(MTLLoader mat, ArrayList<Triangle> tempFaces) {
		this.mat = mat;
		this.tempFaces = tempFaces;
	}
	
	public void apply(int res) {
		this.res = res;
		faces.addAll(tempFaces);
		mtls.add(mat);
	}
	
	public ArrayList<Triangle> getFaces() {
		ArrayList<Triangle> temp = new ArrayList<>();
		temp.addAll(faces);
		temp.addAll(tempFaces);
        Collections.sort(temp, new Comparator<Triangle>() { 
          @Override
          public int compare(Triangle a, Triangle b) { 
            return (int) ((b.area() - a.area()) * 1000000000); 
          } 
        });
		return temp;
	}
}

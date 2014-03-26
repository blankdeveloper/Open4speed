package lightmap;

import java.awt.Color;
import java.awt.Graphics;

import javax.swing.JFrame;
import javax.swing.JPanel;

public class Renderer {
	
    KDNode localRoot;
  
	public JFrame render(String header, final KDNode root) {
        localRoot = root;
		// image
		JPanel panel = new JPanel() {
			private static final long serialVersionUID = 1L;

            @Override
			public void paintComponent(Graphics g) {
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, 4096, 4096);
				try {
					localRoot.draw(g, 0, 0, 1);
				} catch (Exception e) {
				}
			}
		};

		int scale = 1;
		panel.setSize(scale * 256, scale * 256);

		// window
		JFrame frame = new JFrame(header);
		frame.add(panel);
		frame.setSize(scale * 256, scale * 256 + 48);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setResizable(false);
		frame.setVisible(true);
        return frame;
	}
}

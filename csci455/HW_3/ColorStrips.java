
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.Dimension;
import javax.swing.JFrame;
import javax.swing.JPanel;

public class ColorStrips{
        public static void main(String[] args) {
                // Generate the list of Colors
                ColorList cols = new ColorList();

                // Create the outer window
                JFrame frame = new JFrame();

                // Set the on close action
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

                // Define the grid for the colors to be placed on
                frame.setLayout(new GridLayout(256,1));

                for (int i = 0; i < 256; i++) {

                        // Create the panel/rectangle that will be colored
                        JPanel p = new JPanel();

                        // Get the ith color in the list
                        Color c = cols.colList.get(i);

                        // Set the color of the panel
                        p.setBackground(c);

                        // Set the dimenseion of the rectangle
                        p.setPreferredSize(new Dimension(100,4));

                        // Add the panel to the window created earlier
                        frame.add(p);
                }
                // Arrange the panels
                frame.pack();

                //Make it visible
                frame.setVisible(true);

        }
}

// Steven Karl
// 6-20-2021


//Packages
import java.io.FileWriter;   // Import the FileWriter class
import java.io.File;  // Import the File class
import java.io.IOException;  // Import the IOException class to handle errors
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.image.BufferedImage;
import java.awt.Graphics;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import javax.swing.BorderFactory;
import javax.swing.border.LineBorder;
import java.io.FileOutputStream;
import java.io.PrintStream;

class Mandelbrot {
        //Public members
        int[][] xtable;
        int xRes;
        int yRes;
        int cxMin;
        int cxMax;
        int cyMin;
        int cyMax;
        ColorList colors;

        // Constructor
        Mandelbrot(int xRes, int yRes, int cxMin, int cxMax, int cyMin, int cyMax){
                // Filling the colors list with a random set
                colors = new ColorList();

                // Set the members
                this.xRes = xRes;
                this.yRes = yRes;
                this.cxMin = cxMin;
                this.cyMin = cyMin;
                this.cxMax = cxMax;
                this.cyMax = cyMax;
                double real;
                double imag;
                int iter;
                int maxIter = 255;

                // Create and populate teh array
                xtable = new int[xRes][yRes];

                for (int i = 0; i < xRes; i++) {
                        for (int j = 0; j < yRes; j++) {
                                //Create the real and imag
                                real = cxMin + (i/(xRes-1.0))*(cxMax - cxMin);
                                imag = cyMin + (j/(yRes-1.0))*(cyMax - cyMin);

                                //Create the complex numbers
                                ComplexNumber c = new ComplexNumber(real, imag);
                                ComplexNumber z = new ComplexNumber(0.0,0.0);
                                iter = 0;

                                // Loop and calculate the z value
                                while(iter < maxIter && z.abs() < 2){
                                        z = (z.squared()).add(c);
                                        iter++;
                                }
                                // Check the value of z
                                if(z.abs() >= 2){
                                        xtable[i][j] = iter;
                                }
                                else{
                                        xtable[i][j] = 0;
                                }
                        }
                }
        }

        public void setPixel(Graphics g, int x, int y, int red, int grn, int blu){
                Color c = new Color(red,grn,blu);
                g.setColor(c);
                g.drawLine(x,y,x,y);
        }
        public void displayFractal(Graphics g){
                Color c;

                for (int i = 0; i < xRes; i++) {
                        for (int j = 0; j < yRes; j++) {
                                c = colors.colList.get(xtable[i][j]);
                                setPixel(g, i, j, c.getRed(), c.getGreen(), c.getBlue());
                        }
                }
        }
        public void saveFractal(String fileName){

                try{
                        Color c;
                        File f = new File(fileName);
                        FileOutputStream fout = new FileOutputStream(f);
                        PrintStream out = new PrintStream(fout);
                        out.println("P3\r\n" + xRes + " " + yRes + "\r\n255\r\n");
                        for (int i = 0; i < yRes; i++) {
                                for (int j = 0; j < xRes; j++) {
                                        c = colors.colList.get(xtable[j][i]);
                                        out.println(c.getRed() + " " + c.getGreen() + " " + c.getBlue());
                                }
                        }
                        out.close();
                }
                catch(IOException e){
                        System.out.println("Error Caught");
                        e.printStackTrace();
                }



        }

        public static void main(String[] args) {

                JFrame frame = new JFrame();
                JPanel panel = new JPanel();
                panel.setPreferredSize(new Dimension(640,480)); // set the dimensions of our panel
                Color c = new Color(0,0,0);
                panel.setBackground(c); // color our panel
                frame.add(panel);

                BufferedImage bImg = new BufferedImage(640,480, BufferedImage.TYPE_INT_RGB);
                JLabel label = new JLabel(new ImageIcon(bImg));
                label.setPreferredSize(new Dimension(640,480));
                label.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
                panel.add(label);

                Graphics g1 = bImg.createGraphics();

                // Create Mandelbrot
                Mandelbrot m = new Mandelbrot(640,480,-2,1,-1,1);
                m.displayFractal(g1);
                m.saveFractal("Mandelbrot.ppm");

                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.setVisible(true);
                frame.setResizable(false);
                frame.pack();
        }
}

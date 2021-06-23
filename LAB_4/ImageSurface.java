// imports
import javax.swing.*;
import java.util.Random;
import java.awt.image.BufferedImage;
import java.awt.Color;
import java.awt.Graphics;

public class ImageSurface{
        // Private Variables
        JLabel view;
        BufferedImage surface;
        Random rng = new Random();
        Graphics g;
        int xRes = 512;
        int yRes = 512;

        // Constructor
        public ImageSurface(){
                // Create a new buffered image
                surface = new BufferedImage(xRes, yRes, BufferedImage.TYPE_INT_RGB);
                view = new JLabel(new ImageIcon(surface));
                g = surface.getGraphics();
                g.setColor(Color.GRAY);
                g.fillRect(0, 0, xRes-1, yRes-1);

                doGraphics();
                g.dispose();
        }
        private void setPixel(int x, int y, int red, int grn, int blu) {
                Color c = new Color(red,grn,blu);

                g.setColor(c);
                g.drawLine(x,y,x,y);
        }
        private void drawLine(int x0, int y0, int x1, int y1, int red, int grn, int blu){
                Color c = new Color(red,grn,blu);
                g.setColor(c);
                g.drawLine(x0,y0,x1,y1);
        }
        private void drawCircle(int rad, int red, int grn, int blu){
                Color c = new Color(red,grn,blu);
                g.setColor(c);
                g.fillOval(256-rad,256-rad,2*rad,2*rad);
        }
        private void doGraphics() {
                int r,g,b;
                int x0,x1,y0,y1;
                int rad;
                /*for(int i=0;i<500;i++) {
                        int px = rng.nextInt(xRes);
                        int py = rng.nextInt(yRes);

                        // we want to set, at (x,y), a color value of
                        // 255,255,255, ie.white color (the three values
                        // signify red, green and blue components of a
                        // color specification)
                        r = rng.nextInt(255);
                        g = rng.nextInt(255);
                        b = rng.nextInt(255);
                        //setPixel(px,py,r,g,b);

                        x0 = rng.nextInt(xRes);
                        x1 = rng.nextInt(xRes);
                        y0 = rng.nextInt(yRes);
                        y1 = rng.nextInt(yRes);
                        drawLine(x0,y0,x1,y1,r,g,b);
                }
                */
                /*for (int i=0;i<yRes;i++) {
                        for (int j=0;j<xRes;j++) {
                                r = rng.nextInt(255);
                                g = rng.nextInt(255);
                                b = rng.nextInt(255);
                                setPixel(j,i,r,g,b);
                        }
                }*/
                for (int i = 0; i < 100; i++){
                        rad = rng.nextInt(xRes/2);
                        r = rng.nextInt(255);
                        g = rng.nextInt(255);
                        b = rng.nextInt(255);
                        drawCircle(rad,r,g,b);
                }
        }
        public static void main(String[] args){
                ImageSurface img = new ImageSurface();
                JFrame frame = new JFrame();
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.setContentPane(img.view);
                frame.pack();
                frame.setVisible(true);

        }
}

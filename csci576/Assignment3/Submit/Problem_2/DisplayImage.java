// Steven Karl
// CSCI 576 - Viewer
// --------------------------------

// Libraries
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.nio.file.*;

// Main Class Definition
public class DisplayImage {
    int width;
    int height;
    int low_pass_level;
    BufferedImage img;
    BufferedImage[] img_arr;
    
    public DisplayImage(int width, int height, BufferedImage img){
	// Assign Arguments
	this.width = width;
	this.height = height;
	this.low_pass_level = low_pass_level;
	this.img = img;
	this.img_arr = img_arr;

	// Run Routine
	this.display_single();
    }
    public DisplayImage(int width, int height, BufferedImage[] img_arr){
	// Assign Arguments
	this.width = width;
	this.height = height;
	this.low_pass_level = low_pass_level;
	this.img = img;
	this.img_arr = img_arr;

	// Run Routine
	this.display_multiple();
    }
    public void display_single(){
	JFrame frame = new JFrame();
	JLabel lbIm1 = new JLabel();
	frame.setTitle("Image");
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	// Use label to display the image
	GridBagLayout gLayout = new GridBagLayout();
	frame.getContentPane().setLayout(gLayout);
	ImageIcon test_image = new ImageIcon(img);
	lbIm1.setIcon(test_image);

	GridBagConstraints c = new GridBagConstraints();
	c.fill = GridBagConstraints.HORIZONTAL;
	c.anchor = GridBagConstraints.CENTER;
	c.weightx = 0.5;
	c.gridx = 0;
	c.gridy = 0;

	c.fill = GridBagConstraints.HORIZONTAL;
	c.gridx = 0;
	c.gridy = 1;
	frame.getContentPane().add(lbIm1, c);

	frame.pack();
	frame.setVisible(true);
    }
    public void display_multiple(){
	for(int img_idx = 0; img_idx < img_arr.length; img_idx++){
	    JFrame frame = new JFrame();
	    JLabel lbIm1 = new JLabel();
	    frame.setTitle("Image - " + img_idx);
	    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    // Use label to display the image
	    GridBagLayout gLayout = new GridBagLayout();
	    frame.getContentPane().setLayout(gLayout);
	    ImageIcon test_image = new ImageIcon(img_arr[img_idx]);
	    lbIm1.setIcon(test_image);

	    GridBagConstraints c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.anchor = GridBagConstraints.CENTER;
	    c.weightx = 0.5;
	    c.gridx = 0;
	    c.gridy = 0;

	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.gridx = 0;
	    c.gridy = 1;
	    frame.getContentPane().add(lbIm1, c);

	    frame.pack();
	    frame.setVisible(true);
	}
    } 
}


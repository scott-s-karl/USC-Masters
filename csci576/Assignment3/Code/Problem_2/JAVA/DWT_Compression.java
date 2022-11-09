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
public class DWT_Compression {
    String img_path;
    int low_pass_level;
    int width = 512;
    int height = 512;
    int depth = 3;
    JFrame frame = new JFrame();
    JLabel lbIm1 = new JLabel();
    BufferedImage imgOne;
    
    public DWT_Compression(String[] args){
	// Check args
	check_args(args);

	// Assign Arguments
	this.img_path = args[0];
	this.low_pass_level = Integer.parseInt(args[1]);
	
	// Display Arguments
	this.display_args();

	frame.setTitle("Image");
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    public void check_args(String[] args){
	// Check arg count
	if(args.length != 2){
	    System.out.println("Argument Length: " + args.length);
	    System.out.println("Error: Invalid Arguments");
	    System.exit(0);
	}
    }

    public void display_args(){
	System.out.println("Program Inputs:");
	System.out.println("----------------");
	System.out.println("Image Path: " + img_path);
	System.out.println("Low Pass Level: " + low_pass_level);
    }

    public byte[] read_image(int width,
			     int height,
			     String img_path){
	try
	    {
		int frameLength = width*height*3;
		long len = frameLength;

		// Foreground
		File file = new File(img_path);
		RandomAccessFile raf = new RandomAccessFile(file, "r");
		raf.seek(0);
		byte[] bytes = new byte[(int) len];
		raf.read(bytes);
		return bytes;
	    }
	catch (FileNotFoundException e)
	    {
		e.printStackTrace();
	    }
	catch (IOException e)
	    {
		e.printStackTrace();
	    }
	return null;
    }

    public byte[] get_rgb_from_bytes(byte[] bytes,
				     int ind,
				     int width,
				     int height){

	
	byte a = 0;
	byte r = bytes[ind];
	byte g = bytes[ind+height*width];
	byte b = bytes[ind+height*width*2];

	byte[] output = {a, r, g, b};
	return output;
    }

    public int[] convert_bytes_to_rgb(byte[] bytes){
	int[] rgb = new int[3];
	rgb[0] = bytes[1] & 0xff;
	rgb[1] = bytes[2] & 0xff;
	rgb[2] = bytes[3] & 0xff;
	return rgb;
    }

    public void populate_image(int width,
			       int height,
			       String img_path,
			       BufferedImage img){

	int frameLength = width*height*3;
	long len = frameLength;
	
	// Foreground
	byte[] fg_cur_bytes = new byte[(int) len];
	fg_cur_bytes = read_image(width, height, img_path);
	byte[] fg_cur_rgb_bytes = new byte[4];
	int[] fg_cur_rgb = new int[3];
	

	int ind = 0;
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			// Foreground
			fg_cur_rgb_bytes = get_rgb_from_bytes(fg_cur_bytes, ind, width, height);
			fg_cur_rgb = convert_bytes_to_rgb(fg_cur_rgb_bytes);
			int pix = (fg_cur_rgb[0] << 16) + (fg_cur_rgb[1] << 8) + fg_cur_rgb[2];
			img.setRGB(x,y,pix);
			ind++;
		    }
	    }
    }
    
    public void display_image(){

	// Read in the specified image	
	imgOne = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	populate_image(width, height, img_path, imgOne);

	// Use label to display the image
	GridBagLayout gLayout = new GridBagLayout();
	frame.getContentPane().setLayout(gLayout);
	ImageIcon test_image = new ImageIcon(imgOne);
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
    
    public static void main(String args[]){
	// Create an image display instance to show
	DWT_Compression t1 = new DWT_Compression(args);
	
	// Display Image
	t1.display_image();
	System.out.println("Test");	
    }
}

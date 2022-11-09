// Steven Karl
// CSCI 576 - Viewer
// --------------------------------

// Libraries
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.nio.file.*;
import java.util.Random;

class random_frames {
    byte[] bytes;
}

// Main Class Definition
public class ImageViewer {
    String img_path;
    int width;
    int height;
    int depth;
    JFrame frame = new JFrame();
    JLabel lbIm1 = new JLabel();
    BufferedImage imgOne;
    BufferedImage imgBlur;
    
    public ImageViewer(String[] args){
	// Check args
	check_args(args);

	// Assign Arguments
	this.img_path = args[0];
	this.width = 640;
	this.height = 480;
	this.depth = 3;
	// Display Arguments
	this.display_args();

	frame.setTitle("Image");
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    public void check_args(String[] args){
	// Check arg count
	if(args.length != 1){
	    System.out.println("Argument Length: " + args.length);
	    System.out.println("Error: Invalid Arguments");
	    System.exit(0);
	}
    }

    public void display_args(){
	System.out.println("Program Inputs:");
	System.out.println("----------------");
	System.out.println("Image Path: " + img_path);


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
			int gray_level = (int)(fg_cur_rgb[0]*0.299) + (int)(fg_cur_rgb[1]*0.587) + (int)(fg_cur_rgb[2]*0.114);
			
			int gray = (gray_level << 16) + (gray_level << 8) + gray_level;
			int pix = (fg_cur_rgb[0] << 16) + (fg_cur_rgb[1] << 8) + fg_cur_rgb[2];
			img.setRGB(x,y,gray);
			ind++;
		    }
	    }
    }

    public double[] convert_rgb_to_hsv(double r, double g, double b){
	// Go from 0 - 255 to 0 - 1
	r /=255.0;
	g /=255.0;
	b /=255.0;

	//hsv - hue, saturation, value
	double cmax = Math.max(r, Math.max(g,b));
	double cmin = Math.min(r, Math.min(g,b));
	double diff = cmax - cmin;
	double h = -1;
	double s = -1;

	// Get Hue
	if(cmax == cmin){
	    h = 0;
	}
	else if(cmax == r){
	    h = (60 * ((g - b) / diff) + 360) % 360;
	}
	else if(cmax == g){
	    h = (60 * ((b - r) / diff) + 120) % 360;
	}
	else if(cmax == b){
	    h = (60 * ((r - g) / diff) + 240) % 360;
	}
	// Get Satuation
	if(cmax == 0){
	    s = 0; 
	}
	else{
	    s = (diff/cmax) * 100;
	}
	// Get Value
	double v = cmax * 100;

	// return
	double[] hsv_arr = {h,s,v};
	return hsv_arr;
    }
    public double[] make_kernel(){
	double sigma = 5.0;
	double x, fx, sum = 0.0;
	
	int window_size = 1 + 2 * (int)Math.ceil(2.5 * sigma);
	int center = window_size/2;
	double[] kernel = new double[window_size];
	
	System.out.printf("The kernel has %d elements\n", window_size);
	
	for(int i = 0; i < window_size; i++){
	    x = (i - center);
	    fx = Math.pow(2.71828,-0.5*x*x/(sigma*sigma))/(sigma * Math.sqrt(6.2831853));
	    kernel[i] = fx;
	    sum += fx;
	}
	for(int i = 0; i < window_size; i++){
	    kernel[i] /= sum;
	}
	for(int i = 0; i < window_size; i++){
	    System.out.printf("Kernel[%d] = %f\n", i, kernel[i]);
	}
	return kernel;
    }
    
    public void blur_image(int width, int height, BufferedImage img_orig, BufferedImage img_blur){
	// Make the gaussian kernel
	System.out.println("Start Creating Kernel");
	double[] kernel = make_kernel();
	System.out.println("Done Creating Kernel");

	double[][] tempim = new double[width][height];
	int center = kernel.length/2;
	double dot, sum;
	// Loop through the original populate the blurred
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			sum = 0.0;
			dot = 0.0;
		        for(int cc=(-center);cc<=center;cc++){
			    if(((x+cc) >= 0) && ((x+cc) < width)){
				Color cur_pix = new Color(img_orig.getRGB((x+cc), y));
				int cur_gray = cur_pix.getRed();
				double  new_gray = cur_gray * kernel[center+cc];
				dot += (double) new_gray;
				sum += kernel[center+cc];
			    }
			}
			tempim[x][y] = dot/sum;
		    }
	    }
	for(int x = 0; x < width; x++)
	    {
		for(int y = 0; y < height; y++)
		    {
			sum = 0.0;
			dot = 0.0;
		        for(int rr=(-center);rr<=center;rr++){
			    if(((y+rr) >= 0) && ((y+rr) < height)){
				dot += tempim[x][y] * kernel[center+rr];
				sum += kernel[center+rr];
			    }
			}
			int final_gray = (int)(dot*90.0/sum + 0.5);
			int pix = (final_gray << 16) + (final_gray << 8) + (final_gray);
			img_blur.setRGB(x, y, pix);
		    }
	    }
    }

    public void display_image(){

	// Read in the specified image	
	imgOne = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	populate_image(width, height, img_path, imgOne);

	// Blur the image
	imgBlur = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	blur_image(width, height, imgOne, imgBlur);

	// Use label to display the image
	GridBagLayout gLayout = new GridBagLayout();
	frame.getContentPane().setLayout(gLayout);
	ImageIcon test_image = new ImageIcon(imgBlur);
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
	ImageViewer gs = new ImageViewer(args);

	//
	
	// Display Image
	gs.display_image();
	System.out.println("Test");	
    }
}

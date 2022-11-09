// Steven Karl
// CSCI 576 - Green Screen Project
// --------------------------------

// Libraries
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.nio.file.*;


// Main Class Definition
public class GreenScreen {
    String fg_img_path;
    String bg_img_path;
    int bg_mode;
    int width;
    int height;
    int depth;
    JFrame frame;
    //JLabel lbIm1 = new JLabel();
    BufferedImage imgOne;

    public GreenScreen(String[] args){
	// Check args
	check_args(args);

	// Assign Arguments
	this.fg_img_path = args[0];
	this.bg_img_path = args[1];
	this.bg_mode = Integer.parseInt(args[2]);
	this.width = 640;
	this.height = 480;
	this.depth = 3;
	// Display Arguments
	this.display_args();
    }
    public void check_args(String[] args){
	// Check arg count
	if(args.length != 3){
	    System.out.println("Argument Length: " + args.length);
	    System.out.println("Error: Invalid Arguments");
	    System.exit(0);
	}
    }

    public void display_args(){
	String bg_mode_str = bg_mode > 0 ? "Green Background" : "Real Background";
	System.out.println("Program Inputs:");
	System.out.println("----------------");
	System.out.println("Foreground Image Path: " + fg_img_path);
	System.out.println("Background Image Path: " + bg_img_path);
	System.out.println("Background Mode: " + bg_mode + " - " + bg_mode_str);
    }

    public static String get_leading_zeros(int value){
	value = value == 0 ? 1 : value;
	int c = 0;
	String lzs = "";
	while(value > 0){
	    value/=10;
	    c++;
	}
	while(c <= 3){
	    lzs+="0";
	    c++;
	}
	return lzs;
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

    public int[] get_rgb_from_bytes(byte[] bytes,
				    int ind,
				    int width,
				    int height){

	
	int r = bytes[ind] & 0xff;
	int g = bytes[ind+height*width] & 0xff;
	int b = bytes[ind+height*width*2] & 0xff;

	int[] output = {r, g, b};
	return output;
    }

    public void populate_image(int width,
			       int height,
			       String fg_img_path_cur,
			       String bg_img_path_cur,
			       String fg_img_path_next,
			       String bg_img_path_next,
			       BufferedImage img){

	int frameLength = width*height*3;
	long len = frameLength;
	
	// Foreground
	byte[] fg_cur_bytes = new byte[(int) len];
	byte[] fg_next_bytes = new byte[(int) len];
	fg_cur_bytes = read_image(width, height, fg_img_path_cur);
	fg_next_bytes = read_image(width, height, fg_img_path_next);
	int[] fg_cur_rgb = new int[3];
	int[] fg_next_rgb = new int[3];

	// Background
	byte[] bg_cur_bytes = new byte[(int) len];
	byte[] bg_next_bytes = new byte[(int) len];
	bg_cur_bytes = read_image(width, height, bg_img_path_cur);
	bg_next_bytes = read_image(width, height, bg_img_path_next);
	int[] bg_cur_rgb = new int[3];
	int[] bg_next_rgb = new int[3];

	int ind = 0;
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			// Foreground
			fg_cur_rgb = get_rgb_from_bytes(fg_cur_bytes, ind, width, height);
			fg_next_rgb = get_rgb_from_bytes(fg_next_bytes, ind, width, height);

			// Background
			bg_cur_rgb = get_rgb_from_bytes(bg_cur_bytes, ind, width, height);
			bg_next_rgb = get_rgb_from_bytes(bg_next_bytes, ind, width, height);
				
			// check if the pixel needs to be replaced
			if(bg_mode == 1){
			    if (is_foreground_pixel((fg_cur_rgb[0]),(fg_cur_rgb[1]),(fg_cur_rgb[2]))){
				int pix = 0xff000000 | ((fg_cur_rgb[0]) << 16) | ((fg_cur_rgb[1]) << 8) | (fg_cur_rgb[2]);
				img.setRGB(x,y,pix);
			    }				
			    else{
				int pix = 0xff000000 | ((bg_cur_rgb[0]) << 16) | ((bg_cur_rgb[1]) << 8) | (bg_cur_rgb[2]);
				img.setRGB(x,y,pix);
			    }				
			}
			else{
			    if(pixels_match(fg_cur_rgb, fg_next_rgb)){
				int pix = 0xff000000 | ((bg_cur_rgb[0]) << 16) | ((bg_cur_rgb[1]) << 8) | (bg_cur_rgb[2]);
				img.setRGB(x,y,pix);
			    }
			    else{
				int pix = 0xff000000 | ((fg_cur_rgb[0]) << 16) | ((fg_cur_rgb[1]) << 8) | (fg_cur_rgb[2]);
				img.setRGB(x,y,pix);
			    }
			}
			ind++;
		    }
	    }
    }

    public boolean pixels_match(int[] cur_pixel, int[] next_pixel){
	// Check if the values are equal

	// Current
	double[] hsv_cur = convert_rgb_to_hsv((double)cur_pixel[0],(double)cur_pixel[1],(double)cur_pixel[2]);		

	// Next
	double[] hsv_next = convert_rgb_to_hsv((double)next_pixel[0], (double)next_pixel[1], (double)next_pixel[2]);

	// Bounds
	double h_upper = hsv_next[0] + (0.3 * hsv_next[0]);
	double h_lower = hsv_next[0] - (0.3 * hsv_next[0]);
	
	double s_upper = hsv_next[1] + 10;
	double s_lower = hsv_next[1] - 10;
	
	double v_upper = hsv_next[2] + 10;
	double v_lower = hsv_next[2] - 10;

	
	if((hsv_cur[0] >= h_lower && hsv_cur[0] <= h_upper) && (hsv_cur[1] >= s_lower && hsv_cur[1] <= s_upper) && (hsv_cur[2] >= v_lower && hsv_cur[2] <= v_upper)){
	    return true;
	}
	return false;
    }

    public boolean is_foreground_pixel(int r, int g, int b){
	// Convert the value to hsv
        double[] hsv = new double[3];
	hsv = convert_rgb_to_hsv((double)r,(double)g,(double)b);

	// Check if the pixel is green
	//(36,0,0) ~ (86,255,255)40 70
	if((hsv[0] >= 40.0 && hsv[0] <=180.0) && (hsv[1] >= 30.0 && hsv[1] <=100.0) && (hsv[2] >= 30.0 && hsv[2] <=100.0)){
	    return false;
	}
	return true;
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

    public void display_image(int i){

	// Read in the specified image
	Path path_to_fg_folder = Paths.get(fg_img_path);
	Path path_to_bg_folder = Paths.get(bg_img_path);
	String fg_basename = path_to_fg_folder.getFileName().toString();
	String bg_basename = path_to_bg_folder.getFileName().toString();

	String lzs_cur = get_leading_zeros(i);
	String fg_fp_cur = path_to_fg_folder + "//" + fg_basename + "." + lzs_cur + i + ".rgb";
	String bg_fp_cur = path_to_bg_folder + "//" + bg_basename + "." + lzs_cur + i + ".rgb";

	String lzs_next = get_leading_zeros(i+1);
	String fg_fp_next = path_to_fg_folder + "//" + fg_basename + "." + lzs_next + (i+1) + ".rgb";	
	String bg_fp_next = path_to_bg_folder + "//" + bg_basename + "." + lzs_next + (i+1) + ".rgb";
	
	imgOne = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
	populate_image(width, height, fg_fp_cur, bg_fp_cur, fg_fp_next, bg_fp_next, imgOne);

	JLabel label = (JLabel) frame.getContentPane().getComponent(0); 
	label.setIcon(new ImageIcon(imgOne));
	frame.getContentPane().revalidate(); 
	frame.getContentPane().repaint();
    }

    
    public static void main(String args[]){
	// Create an image display instance to show
	GreenScreen gs = new GreenScreen(args);

	// Create the frame and such
	gs.frame = new JFrame();

	// Use label to display the image
	GridBagLayout gLayout = new GridBagLayout();
	gs.frame.getContentPane().setLayout(gLayout);
	gs.frame.getContentPane().add(new JLabel());
	gs.frame.setTitle("Rendered Video");
	gs.frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	gs.frame.setSize(640, 480);
	gs.frame.setVisible(true);
	
	// Display Image
	for(int i = 0; i < 479; i++){
	    gs.display_image(i);
	    
	    try{
		if(gs.bg_mode == 1){
		    Thread.sleep(13);
		}
		else{
		    Thread.sleep(6);
		}
	    }
	    catch (Exception e){
		System.out.println(e);
	    }
	    gs.frame.getContentPane().revalidate(); 
	    gs.frame.getContentPane().repaint();
	}
    }
}


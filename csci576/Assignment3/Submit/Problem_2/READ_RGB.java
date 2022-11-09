// Steven Karl
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
public class READ_RGB{
    int width;
    int height;
    String img_path;
    double[][][] image_arr;
    public READ_RGB(int width, int height, String img_path){
	// Define variables
	this.width = width;
	this.height = height;
	this.img_path = img_path;
	this.image_arr = new double[width][height][3];
	
	// Preform routine
	this.populate_image();
    }
    public byte[] read_image(){
	try
	    {
		int frameLength = width*height*3;
		long len = frameLength;
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
    public void populate_image(){

	// Frame length 
	int frameLength = width*height*3;
	long len = frameLength;

	// Define arrays to hold all image bytes and the current pixels bytes
	byte[] bytes = new byte[(int) len];
	bytes = read_image();
	byte[] cur_bytes = new byte[4];
	int[] cur_rgb = new int[3];
	
	int ind = 0;
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			cur_bytes = get_rgb_from_bytes(bytes, ind);
			cur_rgb = convert_bytes_to_rgb(cur_bytes);

			image_arr[x][y][0] = (double)cur_rgb[0];
			image_arr[x][y][1] = (double)cur_rgb[1];
			image_arr[x][y][2] = (double)cur_rgb[2];
			ind++;
		    }
	    }
    }

    public byte[] get_rgb_from_bytes(byte[] bytes,
				     int ind){	
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
}

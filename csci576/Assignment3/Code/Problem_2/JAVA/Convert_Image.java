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
public class Convert_Image {
    int width;
    int height;
    BufferedImage buf_img;
    BufferedImage[] buf_img_arr;
    double[][][] image_arr;
    double[][][][] arr_image_arr;
    
    public Convert_Image(int width, int height, int type, double[][][] image_arr, double[][][][] arr_image_arr){
	// Assign Arguments
	this.width = width;
	this.height = height;
	this.image_arr = image_arr;
	this.arr_image_arr = arr_image_arr;
	this.buf_img = new BufferedImage(this.width, this.height, BufferedImage.TYPE_INT_RGB);
	this.buf_img_arr = new BufferedImage[10];

	if(type == 0){
	// Run Routine
	    this.convert_single();
	}
	else{
	    this.convert_multiple();	    
	}
    }

    public int get_pixel(double[] rgb_arr){
	int pix = ((int)rgb_arr[0] << 16) + ((int)rgb_arr[1] << 8) + (int)rgb_arr[2];
	return pix;
    }
    public void convert_single(){
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			int pix = get_pixel(image_arr[x][y]);
			buf_img.setRGB(x,y,pix);
		    }
	    }
    }
    public void convert_multiple(){
	for(int arr_idx = 0; arr_idx < 10; arr_idx++){
	    buf_img_arr[arr_idx] = new BufferedImage(this.width, this.height, BufferedImage.TYPE_INT_RGB);
	    for(int y = 0; y < height; y++)
		{
		    for(int x = 0; x < width; x++)
			{
			    int pix = get_pixel(arr_image_arr[arr_idx][x][y]);
			    buf_img_arr[arr_idx].setRGB(x,y,pix);
			}
		}
	}
    }
}


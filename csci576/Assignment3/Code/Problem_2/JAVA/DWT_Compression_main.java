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
public class DWT_Compression_main {
    String img_path;
    int low_pass_level;
    int width = 512;
    int height = 512;
    public DWT_Compression_main(String[] args){
	// Check args
	check_args(args);

	// Assign Arguments
	this.img_path = args[0];
	this.low_pass_level = Integer.parseInt(args[1]);
	
	// Display Arguments
	this.display_args();
    }
    public void check_args(String[] args){
	// Check arg count
	if(args.length != 2){
	    System.out.println("Argument Length: " + args.length);
	    System.out.println("Error: Invalid Arguments");
	    System.exit(0);
	}

	// Check the low pass filter
	int n = Integer.parseInt(args[1]);
	if(n < -1 || n > 9){
	    System.out.println("Error: Invalid low pass level. Must be a integer from -1 to 9");
	    System.exit(0);
	}
    }

    public void display_args(){
	System.out.println("Program Inputs:");
	System.out.println("----------------");
	System.out.println("Image Path: " + img_path);
	System.out.println("Low Pass Level: " + low_pass_level);
    }
    
    public static void main(String args[]){
	// Create an image display instance to show
	DWT_Compression_main dwt_inst = new DWT_Compression_main(args);
	READ_RGB read_data = new READ_RGB(dwt_inst.width,
					  dwt_inst.height,
					  dwt_inst.img_path);
	Process_Image processed_data = new Process_Image(dwt_inst.width,
							 dwt_inst.height,
							 dwt_inst.low_pass_level,
							 read_data.image_arr);

	if(dwt_inst.low_pass_level == -1){
	    Convert_Image converted_data = new Convert_Image(dwt_inst.width,
							     dwt_inst.height,
							     1,
							     processed_data.image_arr,
							     processed_data.arr_image_arr);
	    DisplayImage image_data = new DisplayImage(dwt_inst.width,
						       dwt_inst.height,
						       converted_data.buf_img_arr);
	}
	else{
	    Convert_Image converted_data = new Convert_Image(dwt_inst.width,
							     dwt_inst.height,
							     0,
							     processed_data.image_arr,
							     processed_data.arr_image_arr);
	    DisplayImage image_data = new DisplayImage(dwt_inst.width,
						       dwt_inst.height,
						       converted_data.buf_img);
	}
    }
}

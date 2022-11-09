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
public class Process_Image{
    int width;
    int cur_width;
    int height;
    int cur_height;
    int set_count;
    double[][][] image_arr;
    double[][][][] arr_image_arr;
    public Process_Image(int width, int height, int set_count, double[][][] image_arr){
	// Define variables
	this.width = width;
	this.cur_width = width;
	this.height = height;
	this.cur_height = height;
	this.set_count = set_count;
	this.image_arr = image_arr;
	this.arr_image_arr = new double[10][width][height][3];

	// Preform routine
	this.process();
    }
    
    public void process(){
	if(this.set_count != -1){
	    int sets_to_preform = 9 - this.set_count;
	    while(sets_to_preform > 0){
		this.process_rows(this.image_arr);
		this.cur_width = cur_width/2;
		this.process_columns(this.image_arr);
		this.cur_height = cur_height/2;
		sets_to_preform--;
	    }

	    // Zero out the unused data
	    this.zero_out_data(width, height, this.image_arr);

	    // Reconstruct the image
	    sets_to_preform = 9 - this.set_count;
	    while(sets_to_preform > 0){
		this.reconstruct_columns(this.image_arr);
		this.cur_height = cur_height*2;
		this.reconstruct_rows(this.image_arr);
		this.cur_width = cur_width*2;
		sets_to_preform--;
		}
	}
	else{
	    // Average Down to smallest
	    int sets_to_preform = 9;
	    while(sets_to_preform > 0){
		this.process_rows(this.image_arr);
		this.cur_width = cur_width/2;
		this.process_columns(this.image_arr);
		this.cur_height = cur_height/2;
		sets_to_preform--;
	    }

	    // Define the base height and width to be used each iteration
	    int base_height = this.cur_height;
	    int base_width = this.cur_width;
	    
	    // Loop through the images in the array and fill
	    for(int img_idx = 0; img_idx < arr_image_arr.length; img_idx++){
		// Reset Width and height
		this.cur_height = base_height;
		this.cur_width = base_width;
		
		// Copy source to current
		this.copy_image(this.image_arr, this.arr_image_arr[img_idx]);
		
		// Define number of non zero data reconstruction steps
		int non_zero_reconstructs_left = img_idx;
		
		// Loop and reconstuct
		while(non_zero_reconstructs_left > 0){
		    this.reconstruct_columns(this.arr_image_arr[img_idx]);
		    this.cur_height = cur_height*2;
		    this.reconstruct_rows(this.arr_image_arr[img_idx]);
		    this.cur_width = cur_width*2;
		    non_zero_reconstructs_left--;
		}
		// Zero out the remaining data
		this.zero_out_data(width, height, this.arr_image_arr[img_idx]);	        

		// Define the number of zero data reconstruction steps
		int zero_reconstructs_left = 9 - img_idx;

		// Loop and reconstruct
		while(zero_reconstructs_left > 0){
		    // Reconstruct/Upsample the columns and rows respectively
		    this.reconstruct_columns(this.arr_image_arr[img_idx]);
		    this.cur_height = cur_height*2;
		    this.reconstruct_rows(this.arr_image_arr[img_idx]);
		    this.cur_width = cur_width*2;
		    zero_reconstructs_left--;
		}
	    }
	}
    }

    public void copy_image(double[][][] source, double[][][] dest){
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			dest[x][y][0] = source[x][y][0];
			dest[x][y][1] = source[x][y][1];
			dest[x][y][2] = source[x][y][2];
		    }
	    }
    }

    public double[][] get_current_column(int x, int arr_len, double[][][] cur_image_arr){
	double[][] arr = new double[arr_len][3];
	for(int y = 0; y < arr_len; y++)
	    {
		arr[y][0] = cur_image_arr[x][y][0];
		arr[y][1] = cur_image_arr[x][y][1];
		arr[y][2] = cur_image_arr[x][y][2];
	    }
	return arr;
    }

    public double[][] get_current_row(int y, int arr_len, double[][][] cur_image_arr){
	double[][] cur_row = new double[arr_len][3];
	for(int x = 0; x < arr_len; x++)
	    {
		cur_row[x][0] = cur_image_arr[x][y][0];
		cur_row[x][1] = cur_image_arr[x][y][1];
		cur_row[x][2] = cur_image_arr[x][y][2];
	    }
	return cur_row;
    }
    public double[] get_avg_pix(double[][] row, int idx, int type){
	double r,g,b;
	if(type == 0){
	    r = (row[idx][0] + row[idx + 1][0])/2.0;
	    g = (row[idx][1] + row[idx + 1][1])/2.0;
	    b = (row[idx][2] + row[idx + 1][2])/2.0;
	}
	else{
	    r = (row[idx][0] - row[idx + 1][0])/2.0;
	    g = (row[idx][1] - row[idx + 1][1])/2.0;
	    b = (row[idx][2] - row[idx + 1][2])/2.0;
	}
	double[] rgb_arr = {r,g,b};
	return rgb_arr;
    }
    
    public void process_columns(double[][][] cur_image_arr){
	// Process columns
	double[][] col_arr = new double[cur_height][3];
	int ind = 0;
	for(int x = 0; x < cur_width; x++)
	    {
		int diff_row_idx = (cur_height/2);
		int col_arr_idx = 0;
		col_arr = get_current_column(x, cur_height, cur_image_arr);
		for(int y = 0; y < cur_height/2; y++)
		    {
			double[] avg_sum_pix = get_avg_pix(col_arr, col_arr_idx, 0);
			double[] avg_diff_pix = get_avg_pix(col_arr, col_arr_idx, 1);
			cur_image_arr[x][y][0] = avg_sum_pix[0];
			cur_image_arr[x][y][1] = avg_sum_pix[1];
			cur_image_arr[x][y][2] = avg_sum_pix[2];
			
			cur_image_arr[x][diff_row_idx][0] = avg_diff_pix[0];
			cur_image_arr[x][diff_row_idx][1] = avg_diff_pix[1];
			cur_image_arr[x][diff_row_idx][2] = avg_diff_pix[2];

			diff_row_idx++;
			col_arr_idx=col_arr_idx+2;
		    }
	    }
    }
    public void process_rows(double[][][] cur_image_arr){

	// Process rows
	double[][] row_arr = new double[cur_width][3];
	int ind = 0;
	for(int y = 0; y < cur_height; y++)
	    {
		int diff_col = cur_width/2;
		int row_arr_idx = 0;
		row_arr = get_current_row(y, cur_width, cur_image_arr);
		for(int x = 0; x < cur_width/2; x++)
		    {
			double[] avg_sum_pix = get_avg_pix(row_arr, row_arr_idx, 0);
			double[] avg_diff_pix = get_avg_pix(row_arr, row_arr_idx, 1);
			cur_image_arr[x][y][0] = avg_sum_pix[0];
			cur_image_arr[x][y][1] = avg_sum_pix[1];
			cur_image_arr[x][y][2] = avg_sum_pix[2];
			
			cur_image_arr[diff_col][y][0] = avg_diff_pix[0];
			cur_image_arr[diff_col][y][1] = avg_diff_pix[1];
			cur_image_arr[diff_col][y][2] = avg_diff_pix[2];

			diff_col++;
			row_arr_idx=row_arr_idx+2;
		    }
	    }
    }

    public void zero_out_data(int final_width, int final_height, double[][][] cur_image_arr){
	int ind = 0;
	for(int y = 0; y < final_height; y++)
	    {
		for(int x = 0; x < final_width; x++)
		    {
			if(y >= cur_height || x >= cur_width){
			    cur_image_arr[x][y][0] = 0;
			    cur_image_arr[x][y][1] = 0;
			    cur_image_arr[x][y][2] = 0;
			}
		    }
	    }
    }

    public double[] get_reconstruct_pix(double[][] row, int idx, int diff_idx, int type){
	double r,g,b;
	if(type == 0){
	    r = (row[idx][0] + row[diff_idx][0]);
	    g = (row[idx][1] + row[diff_idx][1]);
	    b = (row[idx][2] + row[diff_idx][2]);
	}
	else{
	    r = (row[idx][0] - row[diff_idx][0]);
	    g = (row[idx][1] - row[diff_idx][1]);
	    b = (row[idx][2] - row[diff_idx][2]);
	}
	double[] rgb_arr = {r,g,b};
	return rgb_arr;
    }

    public void reconstruct_columns(double[][][] cur_image_arr){
	// Process columns
	double[][] col_arr = new double[cur_height*2][3];
	for(int x = 0; x < cur_width; x++)
	    {
		int diff_row_idx = cur_height;
		int col_arr_idx = 0;
		col_arr = get_current_column(x, cur_height*2, cur_image_arr);
		for(int y = 0; y < cur_height*2; y=y+2)
		    {
			double[] sum_pix = get_reconstruct_pix(col_arr, col_arr_idx, diff_row_idx, 0);
			double[] diff_pix = get_reconstruct_pix(col_arr, col_arr_idx, diff_row_idx, 1);
			cur_image_arr[x][y][0] = sum_pix[0];
			cur_image_arr[x][y][1] = sum_pix[1];
			cur_image_arr[x][y][2] = sum_pix[2];
			
			cur_image_arr[x][y+1][0] = diff_pix[0];
			cur_image_arr[x][y+1][1] = diff_pix[1];
			cur_image_arr[x][y+1][2] = diff_pix[2];
			
			diff_row_idx++;
			col_arr_idx++;
		    }
	    }
    }

    public void reconstruct_rows(double[][][] cur_image_arr){
	double[][] row_arr = new double[cur_width*2][3];
	for(int y = 0; y < cur_height; y++)
	    {
		int diff_col_idx = cur_width;
		int row_arr_idx = 0;
		row_arr = get_current_row(y, cur_width*2, cur_image_arr);
		for(int x = 0; x < cur_width*2; x=x+2)
		    {
			double[] sum_pix = get_reconstruct_pix(row_arr, row_arr_idx, diff_col_idx, 0);
			double[] diff_pix = get_reconstruct_pix(row_arr, row_arr_idx, diff_col_idx, 1);

			cur_image_arr[x][y][0] = sum_pix[0];
			cur_image_arr[x][y][1] = sum_pix[1];
			cur_image_arr[x][y][2] = sum_pix[2];
			
			cur_image_arr[x+1][y][0] = diff_pix[0];
			cur_image_arr[x+1][y][1] = diff_pix[1];
			cur_image_arr[x+1][y][2] = diff_pix[2];

			diff_col_idx++;
			row_arr_idx++;
		    }
	    }
    }
}

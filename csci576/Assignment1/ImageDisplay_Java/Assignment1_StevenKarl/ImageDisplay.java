
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;


public class ImageDisplay {

    static int width = 1920; // default image width and height
    static int height = 1080; // default image height

    /** Read Image RGB
     *  Reads the image of given width and height at the given imgPath into the provided BufferedImage.
     */
    private static double[] readImageRGB(int width,
					 int height,
					 String imgPath)
    {
	try
	    {
		int frameLength = width*height*3;

		File file = new File(imgPath);
		RandomAccessFile raf = new RandomAccessFile(file, "r");
		raf.seek(0);

		long len = frameLength;
		byte[] bytes = new byte[(int) len];

		raf.read(bytes);

		int ind = 0;
		int mat_idx = 0;
		double[] output_rgb_arr = new double [frameLength];
		for(int y = 0; y < height; y++)
		    {
			for(int x = 0; x < width; x++)
			    {
				byte a = 0;
				byte r = bytes[ind];
				byte g = bytes[ind+height*width];
				byte b = bytes[ind+height*width*2];

				// Scale the rgb values to 0 255
				output_rgb_arr[mat_idx] = (r & 0xff);
				output_rgb_arr[mat_idx+1] = (g & 0xff);
				output_rgb_arr[mat_idx+2] = (b & 0xff);
				mat_idx+=3;
				ind++;
			    }
		    }
		return output_rgb_arr;
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

    public static double[] convert_rgb_to_yuv(double r,
					      double g,
					      double b){
	double[] output_YUV_arr = new double[3];
	output_YUV_arr[0] = (0.299*r) + (0.587*g) + (0.114*b);
	output_YUV_arr[1] = (0.596*r) + (-0.274*g) + (-0.322*b);
	output_YUV_arr[2] = (0.211*r) + (-0.523*g) + (0.312*b);

	return output_YUV_arr;
    }

    public static double[] convert_yuv_to_rgb(double y,
					      double u,
					      double v){
	double[] output_RGB_arr = new double[3];
	output_RGB_arr[0] = (y) + (0.956*u) + (0.621*v);
	output_RGB_arr[1] = (y) + (-0.272*u) + (-0.647*v);
	output_RGB_arr[2] = (y) + (-1.106*u) + (1.703)*v;

	return output_RGB_arr;
    }

    public static void display_pixel_image(BufferedImage img,
					   double[] arr,
					   int width,
					   int height){
	int mat_idx=0;
	for(int y = 0; y < height; y++)
	    {
		for(int x = 0; x < width; x++)
		    {
			// Get r g and b but clamp and round the numbers (0-255) -> disscussion comment fixes (-) and (+)
			int r = (int) Math.min(255,Math.round(arr[mat_idx])) > 0 ? (int) Math.min(255,Math.round(arr[mat_idx])) : 0;
			int g = (int) Math.min(255,Math.round(arr[mat_idx+1])) > 0 ? (int) Math.min(255,Math.round(arr[mat_idx+1])) : 0; 
			int b = (int) Math.min(255,Math.round(arr[mat_idx+2])) > 0 ? (int) Math.min(255,Math.round(arr[mat_idx+2])) : 0;

			int pix = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
			img.setRGB(x,y,pix);
			mat_idx+=3;
		    }
	    }
    }

    public static double[] convert_yuv_arr_to_rgb_arr(int width,
						      int height,
						      double[] arr){
	// Loop through the yuv arr
	int mat_idx;
	int arr_len = (width*height*3);
	double [] rgb_output_arr = new double [width*height*3];
	for(mat_idx = 0; mat_idx < arr_len; mat_idx += 3){
	    // Get the mini arr of current rgb values from yuv values
	    double [] current_rgb_arr = new double [3];
	    current_rgb_arr = convert_yuv_to_rgb(arr[mat_idx],
						 arr[mat_idx+1],
						 arr[mat_idx+2]);
	    // Add the mini rgb arr to the output arr
	    rgb_output_arr[mat_idx] = current_rgb_arr[0];
	    rgb_output_arr[mat_idx+1] = current_rgb_arr[1];
	    rgb_output_arr[mat_idx+2] = current_rgb_arr[2];
	}
	return rgb_output_arr;
    }

    public static double[] convert_rgb_arr_to_yuv_arr(int width,
							    int height,
							    double[] rgb_arr){
	// Loop to get the RGB from the bytes
	double[] yuv_output_arr = new double[height*width*3];
	int mat_len = (width*height*3);
	for(int mat_idx = 0; mat_idx < mat_len; mat_idx+=3){
	    // convert the RGB value to YUV
	    double [] current_yuv_arr = new double [3];
	    current_yuv_arr = convert_rgb_to_yuv(rgb_arr[mat_idx],
						    rgb_arr[mat_idx+1],
						    rgb_arr[mat_idx+2]);

	    // Add the subarr of yuv values to the output
	    yuv_output_arr[mat_idx] = current_yuv_arr[0];
	    yuv_output_arr[mat_idx+1] = current_yuv_arr[1];
	    yuv_output_arr[mat_idx+2] = current_yuv_arr[2];
	}

	return yuv_output_arr;
    }

    public static double[] subsample_yuv_arr(int y_sample,
					     int u_sample,
					     int v_sample,
					     int width,
					     int height,
					     double[] arr){
	// Loop through the arr by 3
	int mat_idx;
	int col = 0;
	int mat_len = (width*height*3);
	double[] output_arr = new double[width*height*3];
	for(mat_idx = 0; mat_idx < mat_len; mat_idx += 3){
	    // Check if the current YUV set needs to be replaced
	    output_arr[mat_idx] = (col % y_sample != 0) ? 999 : arr[mat_idx];
	    output_arr[mat_idx+1] = (col % u_sample != 0) ? 999 : arr[mat_idx+1];
	    output_arr[mat_idx+2] = (col % v_sample != 0) ? 999 : arr[mat_idx+2];
	    if(col < width - 1){
		col++;
	    }
	    else{
		col = 0;
	    }
	}
	return output_arr;
    }

    public static double get_next_column(int width,
					 int start_idx,
					 int column,
					 double prev_value,
					 double[] arr){

	int hop_count = 0;
	double output_diff = -999;
	// Loop through arry until find data or end of row
	for(int arr_idx = start_idx; column < width; arr_idx+=3){
	    if(arr[arr_idx] != 999){
		output_diff = ((arr[arr_idx] - prev_value)/(hop_count+1));
		break;
	    }
	    hop_count++;
	    column++;
	}
	return output_diff;
    }

    public static double[] adjust_yuv_sampling(int width,
					       int height,
					       int[] subsample_arr,
					       double[] arr){
	// Define Variables
	double prev_col_data;
	double next_col_diff;
	int mat_idx;
	int column_idx = 0;
	int row = 0;
	int rgb_idx;
	int sub_idx;
	int cur_sub_val;
	int avg_val;
	int mat_len = (width*height*3);
	double[] output_arr = new double[mat_len];
	// Loop through array
	for(mat_idx = 0; mat_idx < mat_len; mat_idx+=3){
	    // Check if the current value needs to be replaced
	    sub_idx = 0;
	    for(rgb_idx = mat_idx; rgb_idx < mat_idx+3; rgb_idx++){
		if(arr[rgb_idx] == 999){
		    // Get the current yuv subsample factor
		    cur_sub_val = subsample_arr[sub_idx];

		    // Get the replacement value
		    prev_col_data = column_idx > 0 ? output_arr[rgb_idx-3] : 0;
		    next_col_diff = get_next_column(width, rgb_idx, column_idx, prev_col_data, arr);

		    // Assign output value based on prev_next
		    if(next_col_diff == -999){
			output_arr[rgb_idx] = prev_col_data + (output_arr[rgb_idx-3] - output_arr[rgb_idx-6]);
		    }
		    else{
			output_arr[rgb_idx] = prev_col_data + next_col_diff;
		    }
		}
		else{
		    output_arr[rgb_idx] = arr[rgb_idx];
		}
		sub_idx++;
	    }
	    if(column_idx < width - 1){
		column_idx++;
	    }
	    else{
		column_idx = 0;
		row++;
	    }
	}
	return output_arr;
    }

    public static double[] collect_rgb_from_neighbors(double[] input_arr,
						      int col,
						      int row,
						      int usa_idx,
						      int width,
						      int height){

	// Define tracker
	double avg_r = 0;
	double avg_g = 0;
	double avg_b = 0;
	double avg_denom_count = 0;
	// Get above
	if(row-1 > 0){
	    avg_r += input_arr[usa_idx-width];
	    avg_g += input_arr[(usa_idx-width)+1];
	    avg_b += input_arr[(usa_idx-width)+2];
	    avg_denom_count++;
	}

	// Get below
	if(row+1 < height - 1){
	    avg_r += input_arr[usa_idx+width];
	    avg_g += input_arr[(usa_idx+width)+1];
	    avg_b += input_arr[(usa_idx+width)+2];
	    avg_denom_count++;
	}

	// Get left
	if(col-1 > 0){
	    avg_r += input_arr[usa_idx-3];
	    avg_g += input_arr[(usa_idx-3)+1];
	    avg_b += input_arr[(usa_idx-3)+2];
	    avg_denom_count++;
	}

	//Get right
	if(col + 1 < width - 1){
	   avg_r += input_arr[usa_idx+3];
	   avg_g += input_arr[(usa_idx+3)+1];
	   avg_b += input_arr[(usa_idx+3)+2];
	   avg_denom_count++;
	}

	avg_r /= avg_denom_count;
	avg_g /= avg_denom_count;
	avg_b /= avg_denom_count;
	double[] output_arr = {avg_r, avg_g, avg_b};

	return output_arr;
    }

    public static double check_bad_idx(double[] input_arr,
				       int possible_idx){
	double arr_at_trial_idx;
	try {
	    //  Block of code to try
	    arr_at_trial_idx = input_arr[possible_idx]; 
	}
	catch(ArrayIndexOutOfBoundsException e) {
	    //  Block of code to handle errors
	    return -1.0;
	}
	return arr_at_trial_idx;
    }

    public static double get_surrounding_avg_value(int input_img_width,
						   int input_img_height,
						   int input_arr_idx,
						   double[] input_arr){
	
	// Collect surrounding values
	int top_left = input_arr_idx - (input_img_width*3) - 3;
	int top_middle = input_arr_idx - (input_img_width*3);
	int top_right = input_arr_idx - (input_img_width*3) + 3;
	int middle_left = input_arr_idx - 3;
	int middle_right = input_arr_idx + 3;
	int bottom_left = input_arr_idx + (input_img_width*3) - 3;
	int bottom_middle = input_arr_idx + (input_img_width*3);
	int bottom_right = input_arr_idx + (input_img_width*3) + 3;

	// Get count used and sum of valid values
	int[] surrounding_arr = {top_left, top_middle, top_right, middle_left,
	    middle_right, bottom_left, bottom_middle, bottom_right};
	int surrounding_count = 0;
	double sum = 0.0;
	for(int i = 0; i < 8; i++){
	    double check_idx_value = check_bad_idx(input_arr, surrounding_arr[i]);
	    if(check_idx_value != -1.0){
		surrounding_count++;
		sum += check_idx_value;
	    }
	}
	// Calculate and return average
	double avg = sum/surrounding_count;
	return avg;
    }

    public static double[] scale_rgb_image(int input_img_width,
					   int input_img_height,
					   int output_img_width,
					   int output_img_height,
					   int antialias_flag,
					   double[] input_arr){
	// Define indexes
	double input_arr_idx = 0;
	int output_arr_idx = 0;
	int input_arr_len = input_img_width * input_img_height * 3;
	int output_arr_len = output_img_width * output_img_height * 3;

	// Define arrays
	double[] output_arr = new double[output_arr_len];
	double[] sampled_rgb_arr = new double[3];

	// Scale factors
	double scale_w = (double) input_img_width / output_img_width;
	double scale_h = (double) input_img_height / output_img_height;

	// Row column tracker for input and output arrays
	double input_arr_col = 0.0;
	double input_arr_row = 0.0;
	double output_arr_col = 0.0;
	double output_arr_row = 0.0;

	if(antialias_flag == 0){
	    for(output_arr_idx = 0; output_arr_idx < output_arr_len; output_arr_idx +=3){
		output_arr_row = (output_arr_idx/output_img_width)/3.0;
		output_arr_col = (output_arr_idx%(output_img_width*3.0))/3.0;
		input_arr_row = Math.floor(output_arr_row*scale_h);
		input_arr_col = Math.floor(output_arr_col*scale_w);
		input_arr_idx = 3.0*((input_arr_row*input_img_width) + input_arr_col);

		
		output_arr[output_arr_idx]   = input_arr[(int)input_arr_idx];
		output_arr[output_arr_idx+1] = input_arr[(int)input_arr_idx+1];
		output_arr[output_arr_idx+2] = input_arr[(int)input_arr_idx+2];
	    }
	}
	else{
	    for(output_arr_idx = 0; output_arr_idx < output_arr_len; output_arr_idx +=3){
		// Collect the rows cols and idx
		output_arr_row = (output_arr_idx/output_img_width)/3.0;
		output_arr_col = (output_arr_idx%(output_img_width*3.0))/3.0;
		input_arr_row = Math.floor(output_arr_row*scale_h);
		input_arr_col = Math.floor(output_arr_col*scale_w);
		input_arr_idx = 3.0*((input_arr_row*input_img_width) + input_arr_col);

		// Calculate average from 3x3 neighborhood
		double r = get_surrounding_avg_value(input_img_width,
						     input_img_height,
						     (int)input_arr_idx,
						     input_arr);
		double g = get_surrounding_avg_value(input_img_width,
						  input_img_height,
						  (int)input_arr_idx+1,
						  input_arr);
		double b = get_surrounding_avg_value(input_img_width,
						  input_img_height,
						  (int)input_arr_idx+2,
						  input_arr);
		output_arr[output_arr_idx]   = r;
		output_arr[output_arr_idx+1] = g;
		output_arr[output_arr_idx+2] = b;
	    }
	}
	return output_arr;
    }

    public void showIms(BufferedImage image, String frame_title){
	// Use label to display the image
	JFrame frame = new JFrame();
	GridBagLayout gLayout = new GridBagLayout();
	frame.getContentPane().setLayout(gLayout);

	JLabel lbIm1 = new JLabel(new ImageIcon(image));

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
	frame.setTitle(frame_title);
	frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    public static void display_results(double[] init_rgb,
				       double[] yuv,
				       double[] sub_yuv,
				       double[] adj_yuv,
				       double[] unscaled_rgb,
				       double[] final_rgb){
	// Loop through the arr
	int mat_idx;
	int mat_len = 60;
	for(mat_idx = 0; mat_idx < mat_len; mat_idx+=3){
	    System.out.println("Initial RGB: " + init_rgb[mat_idx]+ " " + init_rgb[mat_idx+1]+ " " + init_rgb[mat_idx+2]);
	    System.out.println("Initial YUV: " + yuv[mat_idx]+ " " + yuv[mat_idx+1]+ " " + yuv[mat_idx+2]);
	    System.out.println("Subsampled YUV: " + sub_yuv[mat_idx]+ " " + sub_yuv[mat_idx+1]+ " " + sub_yuv[mat_idx+2]);
	    System.out.println("Adjusted YUV: " + adj_yuv[mat_idx]+ " " + adj_yuv[mat_idx+1]+ " " + adj_yuv[mat_idx+2]);
	    System.out.println("Unscaled RGB: " + unscaled_rgb[mat_idx]+ " " + unscaled_rgb[mat_idx+1]+ " " + unscaled_rgb[mat_idx+2]);
	    System.out.println("Final RGB: " + final_rgb[mat_idx]+ " " + final_rgb[mat_idx+1]+ " " + final_rgb[mat_idx+2]);
	    System.out.println("");
	    System.out.println("");
	}
	System.out.println("Arr Compare Done");
    }

    public static void display_arguments(String image_path,
					 int Y,
					 int U,
					 int V,
					 float scale_w,
					 float scale_h,
					 int antialias_flag){
	// Print arguments:
	System.out.println("Program Inputs:");
	System.out.println("----------------");
	System.out.println("Image Path: " + image_path);
	System.out.println("Y: " + Y);
	System.out.println("U: " + U);
	System.out.println("V: " + V);
	System.out.println("Scale Width: " + scale_w);
	System.out.println("Scale Height: "  + scale_h);
	System.out.println("Antialiasing Flag: " + antialias_flag);

    }
    public static void main(String[] args) {

	// Parse command line arguments and display
	String image_path = args[0];
	int Y = Integer.parseInt(args[1]);
	int U = Integer.parseInt(args[2]);
	int V = Integer.parseInt(args[3]);
	float scale_w = Float.parseFloat(args[4]);
	float scale_h = Float.parseFloat(args[5]);
	int antialias_flag = Integer.parseInt(args[6]);
	int new_height = (int) (height*scale_h);
	int new_width = (int) (width*scale_w);
	display_arguments(image_path, Y, U, V, scale_w, scale_h, antialias_flag);

	// Create images and arr to hold rgb images
	ImageDisplay initial_image = new ImageDisplay();
	ImageDisplay scaled_image = new ImageDisplay();
	BufferedImage initialImage = new BufferedImage(width, height,BufferedImage.TYPE_INT_RGB);
	BufferedImage scaledImage = new BufferedImage(new_width, new_height,BufferedImage.TYPE_INT_RGB);

	// Create arrays to hold changing values
	double[] initial_rgb_arr = new double[width*height*3];
	double[] modified_rgb_arr = new double[width*height*3];
	double[] yuv_arr = new double [height*width*3];
	double[] subsampled_yuv_arr = new double [height*width*3];
	double[] adjusted_yuv_arr = new double [height*width*3];
	double[] unscaled_rgb_arr = new double [height*width*3];
	double[] final_rgb_arr = new double [new_height*new_width*3];
	int[] subsample_arr = {Y,U,V};

	// Read in the .rgb file to both images into 2 arrays
	initial_rgb_arr = readImageRGB(width, height, image_path);
	modified_rgb_arr = readImageRGB(width, height, image_path);

	// Convert the scaled image into the YUV Space
	yuv_arr = convert_rgb_arr_to_yuv_arr(width, height, modified_rgb_arr);

	// Process YUV subsampling - for scaled image
	subsampled_yuv_arr = subsample_yuv_arr(Y, U, V, width, height, yuv_arr);

	// Adjust up sampling for display - for scaled
	adjusted_yuv_arr = adjust_yuv_sampling(width, height, subsample_arr, subsampled_yuv_arr);

	// Convert the scaled image back to the RGB Space
	unscaled_rgb_arr = convert_yuv_arr_to_rgb_arr(width, height, adjusted_yuv_arr);


	// Scale the RGB image with scale_w and scale_h and alias flag
	final_rgb_arr = scale_rgb_image(width,
					height,
					new_width,
					new_height,
					antialias_flag,
					unscaled_rgb_arr);
	// Show image
	display_pixel_image(initialImage,
			    initial_rgb_arr,
			    width,
			    height);
	display_pixel_image(scaledImage,
			    final_rgb_arr,
			    new_width,
			    new_height);

	initial_image.showIms(initialImage, "Initial");
	scaled_image.showIms(scaledImage, "Scaled");
    }
}

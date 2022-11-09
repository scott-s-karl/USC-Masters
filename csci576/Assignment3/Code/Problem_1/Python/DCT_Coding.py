#####################################################
## Steven Karl
## CSCI 576 - Assignment 3
## Problem 1 - DCT Coding
## Description: Understand the working of DCT in the
##              context of JPEG. Using the given 8x8
##              luminance block of pixel values and
##              its corresponding DCT coefficients
##              do the following.
##
## To Do:
## 1. Using the 2D DCT formula, compute the 64 DCT values.
##    Assume that you quantize your DCT coefficients using
##    the luminance quantization table K1 on page 143 of the
##    uploaded ITU-T JPEG standard. What does your table
##    look like after quantization? (5 points)
## 2. In the JPEG pipeline, the quantized DCT values
##    are then further scanned in a zigzag order.
##    Ignoring your DC value, show the resulting zigzag
##    scan AC values. (2 points).
## 3. For this zigzag AC sequence, write down
##    the intermediary notation (5 points)
## 4. For these are luminance values, write down
##    the resulting JPEG bit stream. You will need
##    to consult standard luminance code tables on
##    page 150 of the ITU-T JPEG standard. (6 points)
## 5. What compression ratio do you get
##    for this luminance block? (2 points)
##
##########################################################################


# Imports
import math

# Globals
LUM_BLOCK = [[188, 180, 155, 149, 179, 116, 86, 96],
             [168, 179, 168, 174, 180, 111, 86, 95],
             [150, 166, 175, 189, 165, 101, 88, 97],
             [163, 165, 179, 184, 135, 90, 91, 96],
             [170, 180, 178, 144, 102, 87, 91, 98],
             [175, 174, 141, 104, 85, 83, 88, 96],
             [153, 134, 105, 82, 83, 87, 92, 96],
             [117, 104, 86, 80, 86, 90, 92, 103]]

"""LUM_BLOCK = [[139,144,149,153,155,155,155,155],
             [144,151,153,156,159,156,156,156],
             [150,155,160,163,158,156,156,156],
             [159,161,162,160,160,159,159,159],
             [159,160,161,162,162,155,155,155],
             [161,161,161,161,160,157,157,157],
             [162,162,161,163,162,157,157,157],
             [162,162,161,161,163,158,158,158]]"""

LUM_QUANT_BLOCK = [[16, 11, 10, 16, 24, 40, 51, 61],
                   [12, 12, 14, 19, 26, 58, 60, 55],
                   [14, 13, 16, 24, 40, 57, 69, 56],
                   [14, 17, 22, 29, 51, 87, 80, 62],
                   [18, 22, 37, 56, 68, 109, 103, 77],
                   [24, 35, 55, 64, 81, 104, 113, 92],
                   [49, 64, 78, 87, 103, 121, 120, 101],
                   [72, 92, 95, 98, 112, 100, 103, 99]]
BLOCK_SIZE = 8*8
PI = 3.141492

def print_hdr(text):
    print(30*"*")
    print(text)
    print(30*"*")

    return None

def display_data(data_block):
    print_hdr("Source Image Samples")
    for row, r_list in enumerate(data_block):
        for col, c_list in enumerate(r_list):
            print("{a:8.2f} ".format(a=data_block[row][col]), end = "")
        print("")
    print_hdr("Done")
    return None

def DCT(input_block):
    print_hdr("Forward DCT Coefficients")
    output = [[0 for i in range(0,8)] for j in range(0,8)]
    for u in range(0,8):
        for v in range(0,8):
            sum_in = 0
            for x in range(0,8):
                for y in range(0,8):
                    sum_in += (input_block[x][y])*math.cos(((2*x+1)*u*PI)/16)*math.cos(((2*y+1)*v*PI)/16)
                cu = 1/math.sqrt(2) if u == 0 else 1
                cv = 1/math.sqrt(2) if v == 0 else 1
            output[u][v] = round(1/4 * cu * cv * sum_in,2) 
            print("{a:8.2f} ".format(a = output[u][v]), end = "")
        print("")
    print_hdr("Done")
    return output

def get_quantized_coef(input_block, quant_block):
    print_hdr("Beginning Quantization")
    output = [[0 for i in range(0,8)] for j in range(0,8)]
    for x in range(0,8):
        for y in range(0,8):
            q_val = (input_block[x][y]/quant_block[x][y])
            #output[x][y] = q_val if q_val > 0.6 or q_val < -0.6 else 0
            output[x][y] = round(q_val)
            print("{a:8.2f} ".format(a = output[x][y]), end = "")
        print("")
    print_hdr("Done with Quantization")
            
    return output

def get_denormalized_quantized_coef(input_block, quant_block):
    print_hdr("Beginning Denormalized Quantization Calculations")
    output = [[0 for i in range(0,8)] for j in range(0,8)]
    for x in range(0,8):
        for y in range(0,8):
            output[x][y] = round(input_block[x][y]*quant_block[x][y], 2)
            print("{a:8.2f} ".format(a = output[x][y]), end = "")
        print("")
    print_hdr("Done with Denormalized Quantization Calculations")
            
    return output

def inverse_DCT(input_block):
    print_hdr("Inverse DCT")
    output = [[0 for i in range(0,8)] for j in range(0,8)]
    for x in range(0,8):
        for y in range(0,8):
            sum_in = 0
            for u in range(0,8):
                for v in range(0,8):
                    cu = 1/math.sqrt(2) if u == 0 else 1
                    cv = 1/math.sqrt(2) if v == 0 else 1
                    sum_in += (cu * cv * input_block[u][v] * math.cos((((2*x)+1)*u*PI)/16) * math.cos((((2*y)+1)*v*PI)/16))
            output[u][v] = round((1/4 * sum_in), 2)
            print("{a:8.2f} ".format(a = output[u][v]), end = "")
        print("")
    print_hdr("Done")
    return output

def zz_display_data(data_block):
    r = c = 8
    output = [[] for i in range(r + c - 1)]
    print_hdr("Zig Zag Notation")
    for i in range(r):
        for j in range(c):
            sum = i + j

            if(sum%2 == 0):
                output[sum].insert(0,data_block[i][j])
            else:
                output[sum].append(data_block[i][j])

    flat_list = [item for sublist in output for item in sublist]
    print(flat_list)
    print_hdr("Done")
    return flat_list

def display_inter_ac_notation(data_block):
    print_hdr("Intermidary AC Notation")
    # Init runlength to 0
    rl = 0

    # Loop through the ac coef in zig zag order
    total_sz = 0
    for idx,item in enumerate(data_block[1:]):
        sz = 1 if item == 0 else math.floor(math.log2(abs(item)))+1
        amp = item
        if item != 0:
            total_sz += sz
            print(f"< {rl}, {sz} >, < {amp} >")
        rl = rl + 1 if item == 0 else 0
        if len(set(data_block[idx:])) == 1:
            break
    print("<EOB>")
    print(f"Total Size {total_sz}")
    print_hdr("Done")
    return None

def main():
    print("Starting Program")
    display_data(LUM_BLOCK)
    dct_block = DCT(LUM_BLOCK)
    quantized_block = get_quantized_coef(dct_block, LUM_QUANT_BLOCK)
    #dequantized_block = get_denormalized_quantized_coef(quantized_block, LUM_QUANT_BLOCK)
    #dct_block = inverse_DCT(dequantized_block)
    zz_data_block = zz_display_data(quantized_block)
    display_inter_ac_notation(zz_data_block)
    print("Ending Program")
    
# Main
if __name__ == "__main__":
    main();


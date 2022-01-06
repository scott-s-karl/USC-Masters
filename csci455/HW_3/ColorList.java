import java.awt.Color;
import java.util.ArrayList;
import java.util.Random;

class ColorList {

  // 'colList' is the color list we are going to fill and keep. Because it is
  // public, if someone wants a color list (of 256 random colors), they just do:
  // ColorList colors = new ColorList(); // 'colors' will now contain 256 random colors!
  // To access a specific color (eg. the color at index 12), the user just does:
  // Color aColor = colors.colList.get(12); // .get() is a method of ArrayList
  // Search for 'Java ArrayList class' to get more info on using the templated ArrayList class.
  public ArrayList <java.awt.Color> colList = new ArrayList <java.awt.Color> ();

  ColorList(){
          int r;
          int g;
          int b;
          Random rng = new Random();
          // Loop 256 times
          for (int i = 0; i < 256 ; i++) {
                  // Create r, g, b as random values between 0 and 255
                  r = rng.nextInt(255);
                  g = rng.nextInt(255);
                  b = rng.nextInt(255);

                  // Create a color using the Color c = new syntax
                  Color c = new Color(r,g,b);

                  // Add the color to the list of colors
                  colList.add(c);
          }
  }
}

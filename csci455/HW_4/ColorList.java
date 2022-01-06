// Steven Karl
// 6-20-2021

//Packages
import java.awt.Color;
import java.util.ArrayList;
import java.util.Random;

//Class
class ColorList {
        // Public Members
        public ArrayList <java.awt.Color> colList = new ArrayList <java.awt.Color> ();

        // Methods
        ColorList(){
                // Create randoms rgb values
                int r;
                int g;
                int b;
                Random rng = new Random();

                for (int i = 0; i < 256; i++) {
                        r = rng.nextInt(255);
                        g = rng.nextInt(255);
                        b = rng.nextInt(255);
                        Color c = new Color(r,g,b);
                        colList.add(c);
                }
        }
}

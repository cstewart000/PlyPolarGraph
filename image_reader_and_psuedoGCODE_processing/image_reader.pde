

// Read in bitmap

PImage img;  // Declare a variable of type PImage
int imgWidth;
int imgHeight;

  
PrintWriter output;

void setup() {
  
  selectInput("Select a file to process:", "fileSelected");
  
  //size(1200,1200);
  // Make a new instance of a PImage by loading an image file
  //img = loadImage(selection);
  //img = loadImage("Hackland-2Lines.bmp");
  
  //imgWidth = img.width;
  //imgHeight = img.height;
  //size(imgWidth,imgHeight);
  
  //output = createWriter("positions.txt"); 
  //size(183, 131);
}

void draw() {
  
  
  background(0);
  // Draw the image to the screen at coordinate (0,0)
  //image(img,0,0);
  loadPixels();
  
  
  
  
  for(int i =0 ; i<(imgWidth*imgHeight);i++){
    
    //output.println(int(pixels[i]));
    
    //output.println(pixels[i]);
    if((int)pixels[i]<-1){
      output.println(int(pixels[i]));
      dotPrint((int)i%imgWidth, (int)i/imgWidth);
    }
  }
  
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); 
  
  
}

void dotPrint(int x,int y){
  
  output.print("X:");
  output.print(x);
  output.print(" Y:");
  output.print(y);
  output.println(" D:1;");
  output.println("D:0;");
  
}



void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    println("User selected " + selection.getAbsolutePath());
  }
}
//Create single array for the image.

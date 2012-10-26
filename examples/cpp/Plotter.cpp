#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include"IVALib/ivalib.hpp"
#include"IVALib/Plotter/SurfaceWindow.hpp"


 
using namespace std;
using namespace cv;

int main(int argc, char **argv){

  //Plot some stuff
  arma::vec x = arma::linspace<arma::vec>(-4,4,256);
  arma::vec normal = exp(-(square(x)/5));
  //Must always be called before using PlotterWindow
  Gtk::Main main(argc,argv);
  PlotterWindow plot;
  //Add curve to the PlotterWindow plot
  plot.add_Curve(x,normal);
  //Set the title of the plot
  plot.set_Title("Test Hello World");
  //The program will continue to run but the window will be shown
  plot.show();
  //The program will continue rinnung once the window has been closed
  plot.run(argc,argv);
  //Label the x and y axis
  plot.set_Xlabel("X axis");
  plot.set_Ylabel("Y axis");
  plot.run(argc,argv);
  //add another curve that is a different color
  arma::vec normal2 = exp(-(square(x)/2));
  plot.add_Curve(x,normal2,"red",PlotMM::CURVE_LINES,PlotMM::SYMBOL_NONE);
  plot.run(argc,argv);

  /*
   *This is an example of how to use the SurfaceWindow class
   */
  //declare variables used and make 3D ball
  arma::cube cdata(50,50,50);
  
  for(int row = 0;row < cdata.n_rows; row++){
    for(int col = 0; col < cdata.n_cols; col++){
      for(int slice = 0;slice < cdata.n_slices; slice++){
	double x = (double) row - 25;
	double y = (double) col - 25;
	double z = (double) slice - 25;	
	double dist = sqrt(pow(x,2) + 1.5*pow(y,2) + 3*pow(z,2));
	if(dist < 20)
	  cdata(row,col,slice) = 255;
	else
	  cdata(row,col,slice) = 0;
      }
    }
  }
  
  SurfaceWindow surf(cdata);
  surf.run();

}

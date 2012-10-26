/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * This is an example explaining how to use active surfaces with the SFM3D class
 * and the energy already provided.
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include"IVALib/ivalib.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<opencv2/highgui/highgui.hpp>
#include"IVALib/ActiveSurfaces/Kappa3D.hpp"

using namespace std;
using namespace cv;

typedef vector<SFM_point<double> > llist;

int main(int argc, char **argv){

  //declare and initialize the variables that will be use
  arma::Cube<double> out(50,50,50);
  arma::Cube<double> phi(50,50,50);
  arma::Cube<double> sphere(50,50,50);
  functor<double> *KForce = new Kappa3D();
  
  //create sphere label map for initialization
  for(int slice = 0; slice<sphere.n_slices; slice++){
    for(int  row =0; row<sphere.n_rows; row++){
      for(int col = 0; col<sphere.n_cols; col++){
	double x = (double) row - 25;
	double y = (double) col - 25;
	double z = (double) slice - 25;	
	double dist = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
	if(dist < 20)
	  sphere(row,col,slice) = 1;
	else
	  sphere(row,col,slice) = 0;
      }//end row for
    }//end col for
  }//end slice for

  SurfaceWindow plotter1(sphere*255);
  plotter1.run();

  //initialize the contour
  SFM3D<double> sfm_test(sphere,KForce);
  sfm_test.Initialize();
  int count =0;
  //iterate the surface
  while(count < 200){
    sfm_test.Update();
    count++;
  }

  //display the output
  phi = sfm_test.getPhi();
  for(int slice = 0; slice<phi.n_slices; slice++){
    for(int  row =0; row<phi.n_rows; row++){
      for(int col = 0; col<phi.n_cols; col++){
	if(phi(row,col,slice) <= 0)
	  out(row,col,slice) = 255;
	else
	  out(row,col,slice) = 0;
      }
    }
  }


  SurfaceWindow plotter2(out);
  plotter2.run();
  
}

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * This is an example explaining how to use the recunstruction functor
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include<cmath>
#include"IVALib/ivalib.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<opencv2/highgui/highgui.hpp>
#include "IVALib/ActiveSurfaces/Reconstruct3d.hpp"


using namespace std;
using namespace cv;

Matx<double,3,3> Rot(Vec3d axis,double zeta){
  Vec3d Naxis = axis/norm(axis);
  return Matx<double,3,3>::eye() + sin(zeta)*Matx<double,3,3>(0,-Naxis[2],Naxis[1],Naxis[2],0,-Naxis[0],-Naxis[1],Naxis[0],0) + (1-cos(zeta))*(Naxis*Naxis.t() - Matx<double,3,3>::eye());
  

}

int main(int argc, char **argv){

  Mat Blank(500,500,CV_8UC3,Scalar(0,0,0)); 
  arma::cube labelMap(100,100,100);
  double x,y,z,dist;
  
  //create the circle label map for initilization
  for(int row = 0; row < labelMap.n_rows;row++){
      for(int collum = 0; collum < labelMap.n_cols;collum++){
	  for(int slice = 0;slice< labelMap.n_slices;slice++){
	      x = (int) row - 50;
	      y = (int) collum - 50;
	      z = (int) slice - 50;
	      dist  = sqrt(pow(x,2) + 2*pow(y,2) + 2*pow(z,2));
	      if(dist <= 25)
		labelMap(row,collum,slice) = 1;
	  }//end slice for loop
      }//end collum for loop
  }//end row for loop

  Matx<double,3,3> CamMat(200,0,250,0,200,250,0,0,1); 
  Object3D Surf(labelMap,2,Vec3d(0,0,0));
  Surf.setCenter(Vec3d(0,0,0));
  Camera Cam(Surf,Blank,CamMat);
  Cam.moveCamera(Vec3d(0,0,-200));
  vector<Mat> Images;
  vector<Camera> Cameras;
  vector<Camera> Cameras2;
  Matx<double,3,3> R;
  Cam.ProjectSurface();
  Cam.setImg(Cam.getSurfImg());
  Images.push_back(Cam.getImg()*255);
  /*  namedWindow( "Camera View", CV_WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Camera View", Images[0] );
  waitKey(0);// Wait for a keystroke in the window*/
  Vec3d cord;
  Vec3d X(0,0,-200);

  //create images for the list of cameras
  for(int i = 1; i<22; i++){
    R = Rot(Vec3d(0,1,0),M_PI/11*i);
    cord = R*X;
    cout<<"the new coordinates are : ("<<cord[0]<<","<<cord[1]<<","<<cord[2]<<")"<<endl;
    Cam.moveCamera(R,cord);
    Cam.ProjectSurface();
    Images.push_back(Cam.getSurfImg()*255);
    /*   imshow( "Camera View", Images[i] );
	 waitKey(0);// Wait for a keystroke in the window    */  
  } 


  //create the circle label map for initilization
  labelMap.zeros();
  for(int row = 0; row < labelMap.n_rows;row++){
      for(int collum = 0; collum < labelMap.n_cols;collum++){
	  for(int slice = 0;slice< labelMap.n_slices;slice++){
	      x = (int) row - 50;
	      y = (int) collum - 50;
	      z = (int) slice - 50;
	      dist  = sqrt(pow(x,2) +pow(y,2) + pow(z,2));
	      if(dist <= 25)
		labelMap(row,collum,slice) = 1;
	  }//end slice for loop
      }//end collum for loop
  }//end row for loop

  Surf.setLabelMap(labelMap);
  for(int i = 0; i <22;i++){
    R = Rot(Vec3d(0,1,0),M_PI/8*i);
    cord = R*X;
    Cameras.push_back(Camera(Surf,Images[i],CamMat));    
    Cameras[i].moveCamera(R,cord);
  }

  /*SurfaceWindow plotter(labelMap*255);
    plotter.run();*/

  
  //Compute the active surface from the cameras
  Mat Img;
  Mat Img2;
  Mat Img3;
  Mat Img4;
  plist sil;
  functor<double> *Force = new Reconstruct3d(Cameras,labelMap);
  SFM3D<double> sfm_test(labelMap,Force);
  sfm_test.Initialize();
  int count =0;
  namedWindow( "Camera 0", CV_WINDOW_AUTOSIZE );// Create a window for display.
  namedWindow( "Camera 7", CV_WINDOW_AUTOSIZE );// Create a window for display.
  namedWindow( "Camera 11", CV_WINDOW_AUTOSIZE );// Create a window for display.
  namedWindow( "Camera 15", CV_WINDOW_AUTOSIZE );// Create a window for display.


  //iterate the surface  
  while(count < 660){
    sfm_test.Update();
    cout<<"Update Complete count is : "<<count <<endl;
    count++;
    if(count % 20 == 0){
      Cameras2 = ((Reconstruct3d*)Force)->getCameras();
      Cameras2[0].computeSilhouette();
      sil = Cameras2[0].getSilhouette();
      Img = Cameras2[0].getImg().clone();
      for(plist::iterator i=sil.begin(); i<sil.end();i++){
	Img.at<uchar>((*i)[0],(*i)[1]) =155;
      }
      Cameras2[4].computeSilhouette();
      sil = Cameras2[4].getSilhouette();
      Img2 = Cameras2[4].getImg().clone();
      for(plist::iterator i=sil.begin(); i<sil.end();i++){
	Img2.at<uchar>((*i)[0],(*i)[1]) =155;
      }
      Cameras2[6].computeSilhouette();
      sil = Cameras2[6].getSilhouette();
      Img3 = Cameras2[6].getImg().clone();
      for(plist::iterator i=sil.begin(); i<sil.end();i++){
	Img3.at<uchar>((*i)[0],(*i)[1]) =155;
      }
      Cameras2[9].computeSilhouette();
      sil = Cameras2[9].getSilhouette();
      Img4 = Cameras2[9].getImg().clone();
      for(plist::iterator i=sil.begin(); i<sil.end();i++){
	Img4.at<uchar>((*i)[0],(*i)[1]) =155;
      }
      imshow( "Camera 0", Img );
      imshow( "Camera 7", Img2 );
      imshow( "Camera 11", Img3 );
      imshow( "Camera 15", Img4 );
      waitKey(100);
      } 
  }

  Cameras2 = ((Reconstruct3d*)Force)->getCameras();
  Cameras2[0].computeSilhouette();
  sil = Cameras2[0].getSilhouette();
  Img = Cameras2[0].getImg();
  for(plist::iterator i=sil.begin(); i<sil.end();i++){
      Img.at<uchar>((*i)[0],(*i)[1]) =155;
  }
  Cameras2[4].computeSilhouette();
  sil = Cameras2[4].getSilhouette();
  Img2 = Cameras2[4].getImg();
  for(plist::iterator i=sil.begin(); i<sil.end();i++){
      Img2.at<uchar>((*i)[0],(*i)[1]) =155;
  }
  Cameras2[6].computeSilhouette();
  sil = Cameras2[6].getSilhouette();
  Img3 = Cameras2[6].getImg();
  for(plist::iterator i=sil.begin(); i<sil.end();i++){
      Img3.at<uchar>((*i)[0],(*i)[1]) =155;
  }
  Cameras2[9].computeSilhouette();
  sil = Cameras2[9].getSilhouette();
  Img4 = Cameras2[9].getImg();
  for(plist::iterator i=sil.begin(); i<sil.end();i++){
      Img4.at<uchar>((*i)[0],(*i)[1]) =155;
  }
  imshow( "Camera 0", Img );
  imshow( "Camera 7", Img2 );
  imshow( "Camera 11", Img3 );
  imshow( "Camera 15", Img4 );
  waitKey(0);
      
  //display the output
  arma::cube phi;
  phi = sfm_test.getPhi();
  arma::cube out(phi.n_rows,phi.n_cols,phi.n_slices);
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
  SurfaceWindow plotter3(Cameras2[0].getSurface().getLabelMap()*255);
  plotter3.run();
}

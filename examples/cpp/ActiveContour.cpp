/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * This is an example explaining how to use active contours with the SFM class
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
#include"IVALib/ActiveSurfaces/Bhattacharyya.hpp"
#include"IVALib/ActiveSurfaces/ChanVese.hpp"


using namespace std;
using namespace cv;

typedef vector<SFM_point<double> > llist;

int main(int argc, char **argv){

  //declare and initialize the variables that will be used
  vector<SFM_point<double> > L0;
  Mat _Img = imread("zebra3.jpg",-1);
  resize(_Img,_Img,Size(300,300));
  Mat_<double> Ball(_Img.size());
  Mat_<Vec3b> Img(_Img.clone());
  functor<double> *KForce = new Bhattacharyya(_Img,256,0,255);//Initialize the functor
  

  //create the circle label map for initilization
  for(int row = 0; row < _Img.rows;row++){
    for(int collum = 0; collum < _Img.cols;collum++){
      
      double x  = (double)row -_Img.rows/2 -35;
      double y  = (double)collum -_Img.cols/2 + 20;
      double dist  = sqrt(pow(x,2) + pow(y,2));
      if(dist < min(_Img.rows,_Img.cols)/4)
	Ball(row,collum) = 1;
      else
	Ball(row,collum) = 0;
    }//end row for loop
  }//end collum for loop

  //initialize the contour
  SFM<double> sfm_test(Ball,KForce);
  sfm_test.Initialize();
  //obtain the contour and draw it on the image
  L0 = sfm_test.getLz();
  
  typename vector<SFM_point<double> >::iterator i = L0.begin();
  for(i;i<L0.end();i++){
    Img((*i)[0],(*i)[1]) =  Vec3b(0,0,255);
  }

  if(! Img.data )// Check for invalid input
     {
      cout <<  "Could not open or find the image" <<endl ;
      return -1;
    }
  
  //show the image
  namedWindow( "Display window", CV_WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Display window", Img );// Show our image inside it.
  
  waitKey(0);// Wait for a keystroke in the window

  L0.clear();// Clear list of points representing the contour
  int count = 0;
  cout<<"Start Iterating."<<endl;
  sfm_test.Update();
 
  
  while(count < 2000){
      sfm_test.Update();
      count++;     
      if(count%50 == 0){
	  //get new L0
	  L0.clear();
	  L0 = sfm_test.getLz();
	  Img = _Img.clone();
	  //draw the contour
	  for(i=L0.begin();i<L0.end();i++){
	      Img((*i)[0],(*i)[1]) = Vec3b(0,0,255);       
	  }
	  //show the image
	  imshow( "Display window", Img );                 
	  waitKey(1); 
      }
  }  
  cout<<"Done iterating!!"<<endl;
 
  //get new L0
  L0 = sfm_test.getLz();
  Img = _Img.clone();
  //draw the contour
  for(i=L0.begin();i<L0.end();i++){
    Img((*i)[0],(*i)[1]) = Vec3b(0,0,255);       
  }
  //show the image
  imshow( "Display window", Img );                 
  waitKey(0); 

  //Use active contour with ChanVese energy
  
  Mat _Img2 = imread("surfer.jpg",-1);
  resize(_Img2,_Img2,Size(300,300));
  Mat_<Vec3b> Img2(_Img2.clone());  
  delete KForce;
  KForce = new ChanVese(_Img2);
  
  //create the circle label map for initilization
  for(int row = 0; row < _Img.rows;row++){
    for(int collum = 0; collum < _Img.cols;collum++){
      
      double x  = (double)row -_Img.rows/2 -35;
      double y  = (double)collum -_Img.cols/2 + 20;
      double dist  = sqrt(pow(x,2) + pow(y,2));
      if(dist < min(_Img.rows,_Img.cols)/6)
	Ball(row,collum) = 1;
      else
	Ball(row,collum) = 0;
    }//end row for loop
  }//end collum for loop

  sfm_test = SFM<double>(Ball,KForce);
  sfm_test.Initialize();
  //obtain the contour and draw it on the image
  L0 = sfm_test.getLz();
      
  i = L0.begin();
  for(i;i<L0.end();i++){
    Img2((*i)[0],(*i)[1]) =  Vec3b(0,0,255);
  }

  if(! Img2.data )// Check for invalid input
     {
      cout <<  "Could not open or find the image" <<endl ;
      return -1;
    }
    
  imshow( "Display window", Img2 );// Show our image inside it.
  waitKey(0);// Wait for a keystroke in the window

  L0.clear();// Clear list of points representing the contour
  count = 0;
  cout<<"Start Iterating."<<endl;
  sfm_test.Update();
 
  
  while(count < 1000){
      sfm_test.Update();
      count++;     
      if(count%50 == 0){
	  //get new L0
	  L0.clear();
	  L0 = sfm_test.getLz();
	  Img2 = _Img2.clone();
	  //draw the contour
	  for(i=L0.begin();i<L0.end();i++){
	      Img2((*i)[0],(*i)[1]) = Vec3b(0,0,255);       
	  }
	  //show the image
	  imshow( "Display window", Img2 );                 
	  waitKey(1); 
      }
  }  
  cout<<"Done iterating!!"<<endl;
 
  //get new L0
  L0 = sfm_test.getLz();
  Img2 = _Img2.clone();
  //draw the contour
  for(i=L0.begin();i<L0.end();i++){
    Img2((*i)[0],(*i)[1]) = Vec3b(0,0,255);       
  }
  //show the image
  imshow( "Display window", Img2 );                 
  waitKey(0); 
  

}

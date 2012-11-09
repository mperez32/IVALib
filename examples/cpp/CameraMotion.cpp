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
    //initialize variables needed
    Mat Blank(500,500,CV_8UC3,Scalar(0,0,0)); 
    arma::cube labelMap(100,100,100);
    double x,y,z,dist;
    //create the circle label map for initilization
    for(int row = 0; row < labelMap.n_rows;row++){
	for(int collum = 0; collum < labelMap.n_cols;collum++){
	    for(int slice = 0;slice< labelMap.n_slices;slice++){
		x = (int) collum - 50;
		y = (int) row - 50;
		z = (int) slice - 50;
		dist  = sqrt(pow(x,2) + 2*pow(y,2) + 2*pow(z,2));
		if(dist <= 25)
		    labelMap(row,collum,slice) = 1;
	    }//end slice for loop
	}//end collum for loop
    }//end row for loop
    
    //create the object3d, camera, camera image and camera vector
    Matx<double,3,3> CamMat(200, 0, 249,0, 200, 249,0,0,1);
    Object3D Surf(labelMap,2,Vec3d(0,0,0));
    Surf.setCenter(Vec3d(0,0,0));
    Camera Cam(Surf,Blank,CamMat);
    Cam.moveCamera(Vec3d(0,0,-200));
    vector<Camera> Cameras;
    Matx<double,3,3> R;
    Cam.ProjectSurface();
    Cam.setImg(Cam.getSurfImg().clone()*255);
    cout<<"This is the camera parameters before disturbance"<<endl;
    cout<<"Rot :"<<endl;
    for(int row = 0; row<Cam.getRot().rows;row++){
	for(int col = 0;col<Cam.getRot().cols;col++){
	    cout<<Cam.getRot()(row,col)<<" ";
	}
	cout<<endl;
    }

    cout<<"The cam cord are :"<<endl;
    cout<<"("<<Cam.getCord()[0]<<","<<Cam.getCord()[1]<<","<<Cam.getCord()[2]<<")"<<endl;

    cout<<"The center coordinates of the object in the camera frame :"<<endl;
    cout<<"("<<Cam.getSurface().getCenter()[0]<<","<<Cam.getSurface().getCenter()[1]<<","<<Cam.getSurface().getCenter()[2]<<")"<<endl;

    
    //distortion camera location and display  error
    Cam.rotateCamera(Vec3d(0,1,0),CV_PI/15);
    cout<<"This is the camera parameters after disturbance"<<endl;
    cout<<"Rot :"<<endl;
    for(int row = 0; row<Cam.getRot().rows;row++){
	for(int col = 0;col<Cam.getRot().cols;col++){
	    cout<<Cam.getRot()(row,col)<<" ";
	}
	cout<<endl;
    }

    cout<<"The cam cord are :"<<endl;
    cout<<"("<<Cam.getCord()[0]<<","<<Cam.getCord()[1]<<","<<Cam.getCord()[2]<<")"<<endl;

    cout<<"The center coordinates of the object in the camera frame :"<<endl;
    cout<<"("<<Cam.getSurface().getCenter()[0]<<","<<Cam.getSurface().getCenter()[1]<<","<<Cam.getSurface().getCenter()[2]<<")"<<endl;

    Cameras.push_back(Cam);    
    Mat img = Cameras[0].getImg().clone();
    Cameras[0].computeSilhouette();
    plist sil = Cameras[0].getSilhouette();
    for(plist::iterator i = sil.begin();i<sil.end();i++){
	img.at<uchar>((*i)[0],(*i)[1]) = 125;
    }
    namedWindow( "Camera View", CV_WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Camera View", img );
    waitKey(0);
    
    //create the phi to compute the camera update
    functor<double> *Force = new Reconstruct3d(Cameras,labelMap,0.25);
    SFM3D<double> sfm_test(labelMap,Force);
    sfm_test.Initialize();
    arma::cube phi = sfm_test.getPhi();
    vector<SFM_point<double> > Lz = sfm_test.getLz();
    
    //update Camera
    int count =0;
    while(count < 1200){
	count++;
	((Reconstruct3d*) Force)->updateCamera(phi,Lz);
	//display the update
	if(count % 2 == 0){
	    ((Reconstruct3d*) Force)->getCameras()[0].computeSilhouette();
	    sil = ((Reconstruct3d*) Force)->getCameras()[0].getSilhouette();
	    img = ((Reconstruct3d*) Force)->getCameras()[0].getImg().clone();
	    for(plist::iterator i = sil.begin();i<sil.end();i++){
		img.at<uchar>((*i)[0],(*i)[1]) = 125;
	    }
	    namedWindow( "Camera View", CV_WINDOW_AUTOSIZE );// Create a window for display.
	    imshow( "Camera View", img );
	    waitKey(50);
	    
	}
    }
    
    Camera cam_new = ((Reconstruct3d*) Force)->getCameras()[0];
    cout<<"This is the camera parameters after the update"<<endl;
    cout<<"Rot :"<<endl;
    for(int row = 0; row<cam_new.getRot().rows;row++){
	for(int col = 0;col<cam_new.getRot().cols;col++){
	    cout<<cam_new.getRot()(row,col)<<" ";
	}
	cout<<endl;
    }

    cout<<"The cam cord are :"<<endl;
    cout<<"("<<cam_new.getCord()[0]<<","<<cam_new.getCord()[1]<<","<<cam_new.getCord()[2]<<")"<<endl;

    cout<<"The center coordinates of the object in the camera frame :"<<endl;
    cout<<"("<<cam_new.getSurface().getCenter()[0]<<","<<cam_new.getSurface().getCenter()[1]<<","<<cam_new.getSurface().getCenter()[2]<<")"<<endl;
    
    ((Reconstruct3d*) Force)->getCameras()[0].computeSilhouette();
    sil = ((Reconstruct3d*) Force)->getCameras()[0].getSilhouette();
    img = ((Reconstruct3d*) Force)->getCameras()[0].getImg().clone();
    for(plist::iterator i = sil.begin();i<sil.end();i++){
	img.at<uchar>((*i)[0],(*i)[1]) = 125;
    }
    namedWindow( "Camera View", CV_WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Camera View", img );
    waitKey(0);
    
}

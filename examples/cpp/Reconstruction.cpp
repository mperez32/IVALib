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
#include<sstream>
#include<string>
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
    //create the object3d
    double x,y,z,dist;
    arma::cube labelMap(160,90,45);
    arma::cube phi;
    vector<SFM_point<double> > Lz;
    labelMap.zeros();
    //create the circle label map for initilization
    for(int row = 0; row < labelMap.n_rows;row++){
	for(int collum = 0; collum < labelMap.n_cols;collum++){
	    for(int slice = 0;slice< labelMap.n_slices;slice++){
		x = (double) collum - 45.0;
		y = (double) row - 80;
		z = (double) slice - 22.5;
		dist  = sqrt(pow((75.0/41)*x,2) + pow(y,2) + pow((75.0/20)*z,2));
		if(dist <= 75)
		    labelMap(row,collum,slice) = 1;
	    }//end slice for loop
	}//end collum for loop
    }//end row for loop

    Object3D Surf(labelMap,1.0/9.5,Vec3d(0,0,0));    
    Surf.setCenter(Vec3d(0,0,0));
    
    //create a vector of cameras and specify the parameters
    Matx<double,3,3> R;
    vector<Camera> Cameras;
    Vec3d cord;
    Vec3d X(0,0,-30);
    Vec3d T(0.25,0.3,0);
    Mat Img;
    Mat Img2;
    Mat Img3;
    Mat Img4;
    Matx<double,3,3> CamMat(849.68512203161049, 0, 319, 0,
			    862.35333051700650, 239, 0, 0, 1);

    stringstream filename;
    stringstream cam_info;
    for(int i = 1;i<10;i++){
	//read the files
	filename <<"images/IMAG000"<<i<<".JPG";
	Img = imread(filename.str(),-1);
	medianBlur( Img, Img, 9);
	Img = Img*2;
	if(! Img.data ){
	    cout <<  "Could not open or find the image" <<endl ;
	    return -1;
	}
	//locate camera
	R = Rot(Vec3d(0,1,0),-CV_PI/12*(i-1));
	cord = R*X;
	Cameras.push_back(Camera(Surf,Img.clone(),CamMat));
	Cameras[i-1].moveCamera(R,cord-T);
	filename.str("");
	
    }
    
    for(int i=10;i<=24;i++){
	//read the files
	filename <<"images/IMAG00"<<i<<".JPG";
	Img = imread(filename.str(),-1);
	medianBlur( Img, Img, 9);
	Img = Img*2;
	if(! Img.data ){
	    cout <<  "Could not open or find the image" <<endl ;
	    return -1;
	}
	//locate cameras
	R = Rot(Vec3d(0,1,0),-CV_PI/12*(i-1));
	cord = R*X;
	Cameras.push_back(Camera(Surf,Img.clone(),CamMat));
	Cameras[i-1].moveCamera(R,cord-T);
	filename.str("");
    }

    
    plist sil;
    namedWindow( "Cameras", CV_WINDOW_AUTOSIZE );// Create a window for display.
    VideoWriter record("./ReconstructionVideo5.avi", CV_FOURCC('D','I','V','X'), 3, Img.size(), true);
    if( !record.isOpened() ) {// check to see if the recorder is open
	cout<<"VideoWriter failed to open"<<endl;
	return -1;}
    //Show images with silhouette
    for(int k=0;k<24;k++){
	Img = Cameras[k].getImg().clone();
	cam_info.str("");
	putText(Img,"Iteration : 0",Point(20,20),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,255,255));	
	cam_info.str("");
	cam_info <<"Camera : "<<k+1;
	putText(Img,cam_info.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.45,Scalar(255,0,255));
	cam_info.str("");
	cam_info <<"The coordinate of the camera is : ";
	putText(Img,cam_info.str(),Point(20,60),FONT_HERSHEY_SIMPLEX,0.45,Scalar(255,0,255));
	cam_info.str("");
        cam_info <<Cameras[k].getCord();
	putText(Img,cam_info.str(),Point(20,80),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,0,255));
	    
	Cameras[k].computeSilhouette();
	sil = Cameras[k].getSilhouette();
	for(plist::iterator i = sil.begin();i<sil.end();i++){
	    Img.at<Vec3b>((*i)[0],(*i)[1]) = Vec3b(0,255,0);
	    }
	record << Img;
	imshow( "Cameras", Img);
	waitKey(300);
    }



    //destroyAllWindows();
    functor<double> *Force = new Reconstruct3d(Cameras,labelMap,0.002);
    Cameras.clear();
    cout<<"The size of Cameras is : "<<Cameras.size()<<endl;
    SFM3D<double> sfm_test(labelMap,Force);
    sfm_test.Initialize();
    int count =0;
    //iterate the surface  
    while(count < 250){
	sfm_test.Update();
	if(count % 30 == 0){
	    //display the cameras view with the sillhouette curve of the surface
	    for(int k = 0;k<24;k++){
		
		((Reconstruct3d*)Force)->getCameras()[k].computeSilhouette();
		sil = ((Reconstruct3d*)Force)->getCameras()[k].getSilhouette();
		Img2 = ((Reconstruct3d*)Force)->getCameras()[k].getImg().clone();
		cam_info.str("");
		cam_info << "Iteration : "<<count+1;
		putText(Img2,cam_info.str(),Point(20,20),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,255,255));	
		cam_info.str("");
		cam_info <<"Camera : "<<k+1;
		putText(Img2,cam_info.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<"The cordinate of the camera is :";
		putText(Img2,cam_info.str(),Point(20,60),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<((Reconstruct3d*)Force)->getCameras()[k].getCord();
		putText(Img2,cam_info.str(),Point(20,80),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,0,255));

		for(plist::iterator i=sil.begin(); i<sil.end();i++)
		    Img2.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
		imshow( "Cameras update", Img2 );
		record << Img2;
		waitKey(300);
		}
	}    
	count++;
	cout<<"Update Complete count is : "<<count <<endl;
}
    
    while(count < 1200){
	sfm_test.Update();
	//iterate the camera parameters	    
	if(count % 12 == 0){
	    phi = sfm_test.getPhi(); 
	    Lz = sfm_test.getLz();
	    for(int j = 0; j<4 ;j++)
		((Reconstruct3d*) Force)->updateCamera(phi,Lz);
	}	    
	
	if(count % 30 == 0){
	    //display the cameras view with the sillhouette curve of the surface
	    for(int k = 0;k<24;k++){
		
		((Reconstruct3d*)Force)->getCameras()[k].computeSilhouette();
		sil = ((Reconstruct3d*)Force)->getCameras()[k].getSilhouette();
		Img2 = ((Reconstruct3d*)Force)->getCameras()[k].getImg().clone();
		cam_info.str("");
		cam_info << "Iteration : "<<count+1;
		putText(Img2,cam_info.str(),Point(20,20),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,255,255));	
		cam_info.str("");
		cam_info <<"Camera : "<<k+1;
		putText(Img2,cam_info.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<"The cordinate of the camera is :";
		putText(Img2,cam_info.str(),Point(20,60),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<((Reconstruct3d*)Force)->getCameras()[k].getCord();
		putText(Img2,cam_info.str(),Point(20,80),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,0,255));

		for(plist::iterator i=sil.begin(); i<sil.end();i++)
		    Img2.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
		imshow( "Cameras update", Img2 );
		record << Img2;
		waitKey(300);
		}
	}    
	count++;
	cout<<"Update Complete count is : "<<count <<endl;
    }

    while(count < 1800){
	sfm_test.Update();
	//iterate the camera parameters	    
	if(count % 8 == 0){
	    phi = sfm_test.getPhi(); 
	    Lz = sfm_test.getLz();
	    for(int j = 0; j<3 ;j++)
		((Reconstruct3d*) Force)->updateCamera(phi,Lz);
	}
	if(count % 30 == 0){
	    //display the cameras view with the sillhouette curve of the surface
	    for(int k = 0;k<24;k++){
		
		((Reconstruct3d*)Force)->getCameras()[k].computeSilhouette();
		sil = ((Reconstruct3d*)Force)->getCameras()[k].getSilhouette();
		Img2 = ((Reconstruct3d*)Force)->getCameras()[k].getImg().clone();
		cam_info.str("");
		cam_info << "Iteration : "<<count+1;
		putText(Img2,cam_info.str(),Point(20,20),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,255,255));	
		cam_info.str("");
		cam_info <<"Camera : "<<k+1;
		putText(Img2,cam_info.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<"The cordinate of the camera is :";
		putText(Img2,cam_info.str(),Point(20,60),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
		cam_info.str("");
		cam_info <<((Reconstruct3d*)Force)->getCameras()[k].getCord();
		putText(Img2,cam_info.str(),Point(20,80),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,0,255));
		
		for(plist::iterator i=sil.begin(); i<sil.end();i++)
		    Img2.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
		imshow( "Cameras update", Img2 );
		record << Img2;
		waitKey(300);
	    }
	}    
	count++;
	cout<<"Update Complete count is : "<<count <<endl;
    }
    
    //display the cameras view with the sillhouette curve of the surface
    for(int k = 0;k<24;k++){
	
	((Reconstruct3d*)Force)->getCameras()[k].computeSilhouette();
	sil = ((Reconstruct3d*)Force)->getCameras()[k].getSilhouette();
	Img2 = ((Reconstruct3d*)Force)->getCameras()[k].getImg().clone();
	cam_info.str("");
	cam_info << "Iteration : "<<count+1;
	putText(Img2,cam_info.str(),Point(20,20),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,255,255));	
	cam_info.str("");
	cam_info <<"Camera : "<<k+1;
	putText(Img2,cam_info.str(),Point(20,40),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
	cam_info.str("");
	cam_info <<"The cordinate of the camera is :";
	putText(Img2,cam_info.str(),Point(20,60),FONT_HERSHEY_SIMPLEX,0.45,Scalar(170,0,255));
	cam_info.str("");
	cam_info <<((Reconstruct3d*)Force)->getCameras()[k].getCord();
	putText(Img2,cam_info.str(),Point(20,80),FONT_HERSHEY_SIMPLEX,0.45,Scalar(0,0,255));

	for(plist::iterator i=sil.begin(); i<sil.end();i++)
	    Img2.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
	imshow( "Cameras update", Img2 );
	record << Img2;
	filename.str("");
	if(k<10)
	    filename<<"./resultImg/Img100"<<k<<".JPG";
	else
	    filename<<"./resultImg/Img10"<<k<<".JPG";
	imwrite(filename.str(),Img2);
	waitKey(300);
    }


    //draw Silhouette cureve on camera views
    ((Reconstruct3d*)Force)->getCameras()[1].computeSilhouette();
    sil = ((Reconstruct3d*)Force)->getCameras()[1].getSilhouette();
    Img = ((Reconstruct3d*)Force)->getCameras()[1].getImg().clone();
    for(plist::iterator i=sil.begin(); i<sil.end();i++)
	Img.at<Vec3b>((*i)[0],(*i)[1]) = Vec3b(0,255,0);
    
   
    ((Reconstruct3d*)Force)->getCameras()[4].computeSilhouette();
    sil = ((Reconstruct3d*)Force)->getCameras()[4].getSilhouette();
    Img2 = ((Reconstruct3d*)Force)->getCameras()[4].getImg().clone();
    for(plist::iterator i=sil.begin(); i<sil.end();i++)
	Img2.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
    
   
    ((Reconstruct3d*)Force)->getCameras()[6].computeSilhouette();
    sil = ((Reconstruct3d*)Force)->getCameras()[6].getSilhouette();
    Img3 = ((Reconstruct3d*)Force)->getCameras()[6].getImg().clone();
    for(plist::iterator i=sil.begin(); i<sil.end();i++)
	Img3.at<Vec3b>((*i)[0],(*i)[1]) =Vec3b(0,255,0);
   
   
    ((Reconstruct3d*)Force)->getCameras()[9].computeSilhouette();
    sil = ((Reconstruct3d*)Force)->getCameras()[9].getSilhouette();
    Img4 = ((Reconstruct3d*)Force)->getCameras()[9].getImg().clone();
    for(plist::iterator i=sil.begin(); i<sil.end();i++)
	Img4.at<Vec3b>((*i)[0],(*i)[1]) = Vec3b(0,255,0);


    //display output images
    imshow( "Cameras 1", Img );
    imshow( "Cameras 4", Img2 );
    imshow( "Cameras 6", Img3 );
    imshow( "Cameras 9", Img4 );
    waitKey(0);
    //destroyAllWindows() ;
    //waitKey(0);
    //display the output
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
    out.save("LabelMap5.mat",arma::arma_ascii);


}    
    



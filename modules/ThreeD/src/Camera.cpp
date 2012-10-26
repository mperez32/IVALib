/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the Camera class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


#include"IVALib/ThreeD/Camera.hpp"


using namespace std;
using namespace cv;


void Camera::ProjectSurface(){
    //Delete the previously projected image and create a new image 
    this->SurfImg = Mat::zeros(SurfImg.size(),CV_8U);
    
    //Project the points onto the surface
    typename vector<SFM_point<double> >::iterator i=Surface.getLz().begin();
    for(i;i<Surface.getLz().end();i++){
	ProjectPoint((*i));
    }
}

void Camera::computeOcc(){
    //Delete the previously projected image and create a new image and delete the prvious Occ
    this->SurfImg = Mat::zeros(SurfImg.size(),CV_8U);
    this->ProjImg.clear();
    this->Occ.clear();

    //Project the points onto the surface
    typename vector<SFM_point<double> >::iterator i=Surface.getLz().begin();
    for(i;i<Surface.getLz().end();i++){
	ProjectOccPoint((*i));
    }
    vlist backpoints;
    SFM_point<double> point(ProjImg.rows,ProjImg.cols);
    //find all the points that are on the edge of the image, project back and add those points to the end of the list Occ
    for(int row=0;row<ProjImg.rows;row++){
	for(int col=0; col<ProjImg.cols;col++){
	    if(this->SurfImg.at<uchar>(row,col) == 1){
		point.set(row,col);
		backpoints = this->ProjImg.row(row)[col];
		if(point.checkForward() && this->SurfImg.at<uchar>(row+1,col) == 0){
		    this->Occ.insert(this->Occ.end(),backpoints.begin(),backpoints.end());}
		else if(point.checkBackward() && this->SurfImg.at<uchar>(row-1,col) == 0){
		    this->Occ.insert(this->Occ.end(),backpoints.begin(),backpoints.end());}
		else if(point.checkUp() && this->SurfImg.at<uchar>(row,col+1) == 0){
		    this->Occ.insert(this->Occ.end(),backpoints.begin(),backpoints.end());}
		else if(point.checkDown() && this->SurfImg.at<uchar>(row,col-1) == 0){
		    this->Occ.insert(this->Occ.end(),backpoints.begin(),backpoints.end());}	
	    }//end if checking Init(row,collum)
	}//end of for loop col
    }//end of for loop row
};


inline void Camera::ProjectOccPoint(Vec3i const &point){
    //get the world point Wpoint in (x,y,z) coordinates 
    Vec3d Wpoint;
    Surface.getPoint(point,Wpoint);
    //Project the world point onto the image
    Vec2d u1 = Vec2d(intrin(1,2) + (intrin(1,1)*Wpoint[1]/Wpoint[2]), intrin(0,2) + (intrin(0,0)*Wpoint[0]/Wpoint[2]));
    //normalize the world point Wpoint_n
    Vec3d Wpoint_n = Wpoint/(Wpoint[0]*Wpoint[0] + Wpoint[1]*Wpoint[1] + Wpoint[2]*Wpoint[2]);
    //Compute V = <Wpoint_n,(1,0,0)>Wpoint_n - (1,0,0)
    Vec3d V(Wpoint_n[0]*Wpoint_n[0]-1,Wpoint_n[0]*Wpoint_n[1],Wpoint_n[0]*Wpoint_n[2]);
    //Compute V2 and project it onto the image
    Vec3d V2 = Wpoint - V*(Surface.getScale()/(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]));
    Vec2d u2 = Vec2i( intrin(1,2) + (intrin(1,1)*V2[1]/V2[2]), intrin(0,2) + (intrin(0,0)*V2[0]/V2[2]));
    //Compute the distance between u and u2
    double dist = sqrt((u1[0]-u2[0])*(u1[0]-u2[0]) + (u1[1]-u2[1])*(u1[1]-u2[1]));
    //get the list of Vec2i that are in the circle and set the image of the list of point =1 and push back the 3dpoint
    vlist *rowptr;
    if(dist == 0){
	if(u1[0] >= 0 && u1[0] < ProjImg.rows && u1[1] >= 0 && u1[1] < ProjImg.cols){ 
	    SurfImg.at<uchar>(u1[0],u1[1]) = 1;
	    ProjImg.row((int)u1[0])[(int)u1[1]].push_back(point);
	}
    }
    else{
	std::vector<Vec2i > pointl;
	ivaCircle(u1,dist,Img.size(),pointl);
	typename std::vector<Vec2i >::iterator i = pointl.begin(); 
	for(i;i<pointl.end();i++){
	    if((*i)[0] >= 0 && (*i)[0] < ProjImg.rows && (*i)[1] >= 0 && (*i)[1] < ProjImg.cols){ 
		SurfImg.at<uchar>((*i)[0],(*i)[1]) = 1;
		ProjImg.row((*i)[0])[(*i)[1]].push_back(point);
	    }
	}
    }
};

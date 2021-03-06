#ifndef _CAMERA_H_
#define _CAMERA_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * This file contains the header that  contains the decleration of the Camera 
 * class in IVALib.
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/

#include"opencv2/opencv.hpp"
#include<armadillo_bits/config.hpp>
#include<armadillo>
#include<stdexcept> 
#include<sstream>
#include<string>
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include"IVALib/ThreeD/Object3D.hpp"
#include"IVALib/ThreeD/ProjPlane.hpp"


typedef std::vector<SFM_point<double> > plist;
typedef std::vector<cv::Vec3i> vlist;


class Camera {
public:

  /**
   *Constructor 
   *@param Surface Object3D used to represent the surface
   *that you are modeling or observing with the camera
   */ 
 Camera(Object3D & Surface){
    rot = cv::Matx<double,3,3>(1, 0 , 0, 0, 1, 0, 0, 0, 1);
    intrin = cv::Matx<double,3,3>(1,0,0,0,1,0,0,0,1);
    cord = cv::Vec3d(0,0,0);
    this->Surface = Surface;
    this->SurfCenter = Surface.getCenter();
  }
  /**
   *Constructor 
   *@param Surface Object3D used to represent the surface that you are modeling or observing with the camera
   *@param Img cv::Mat that represents the real life image
   */
  Camera(Object3D &Surface,cv::Mat const &Img){
    
    double centerx = Img.cols/2;
    double centery = Img.rows/2;
    rot = cv::Matx<double,3,3>(1, 0, 0, 0, 1, 0, 0, 0, 1);
    intrin = cv::Matx<double,3,3>(1,0,centerx,0,1,centery,0,0,1);
    cord = cv::Vec3d(0,0,0);
    this->Img = Img;
    this->SurfImg = cv::Mat::zeros(Img.size(),CV_8U);
    this->ProjImg.setSize(Img.rows,Img.cols);
    this->Surface = Surface;
    this->SurfCenter = Surface.getCenter();
  }
  /**
   *Constructor 
   *@param Surface Object3D used to represent the surface that you are modeling or observing with the camera
   *@param Img cv::Mat that represents the real life image
   *@param intrin cv::Matx<double,3,3> that represents the intrinsic parameters of the camera
   */
  Camera(Object3D &Surface,cv::Mat const &Img,cv::Matx<double, 3,3> const &intrin):
    intrin(intrin)
  {
    
    double centerx = Img.cols/2;
    double centery = Img.rows/2;
    rot = cv::Matx<double,3,3>(1, 0, 0, 0, 1, 0, 0, 0, 1);
    this->intrin = intrin;
    cord = cv::Vec3d(0,0,0);
    this->Img = Img;
    this->SurfImg = cv::Mat::zeros(Img.size(),CV_8U);
    this->ProjImg.setSize(Img.rows,Img.cols);
    this->Surface = Surface;
    this->SurfCenter = Surface.getCenter();
  }
  /**
   *Constructor 
   *@param Surface Object3D used to represent the surface that you are modeling or observing with the camera
   *@param Img cv::Mat that represents the real life image
   *@param intrin cv::Matx<double,3,3> is a matrix of the intrinsic camera parameters
   *@param rot cv::Matx<double,3,3> is a matrix representing the camera rotattion in the world frame
   *@param cord cv::Vec3d is a cvector that represents the cameras coordinates in the world frame
   */
  Camera(Object3D &Surface,cv::Mat const &Img,cv::Matx<double, 3,3> const &intrin, cv::Matx<double, 3, 3> const &rot, cv::Vec3d const &cord):
    intrin(intrin),
    rot(rot),
    cord(cord)
{
    this->cord = cv::Vec3d(0,0,0);
    this->Img = Img;
    this->SurfImg = cv::Mat::zeros(Img.size(),CV_8U);
    this->ProjImg.setSize(Img.rows,Img.cols);
    this->Surface = Surface;    
    this->SurfCenter = Surface.getCenter();
};
  
  
  /**
   *ProjectSurface
   *Function used to project the surface onto SurfImg and ProjImg. 
   */
  void ProjectSurface();
  /**
   *computeSilhouet
   *computes the silhouet curve of the surface 
   */
  inline void computeSilhouette(){
    this->ProjectSurface();
    Silhouette.clear();
    SFM_point<double> point(SurfImg.rows,SurfImg.cols);
    //find all the points that are on the edge of the image, project back and add those points to the end of the list Occ
    for(int row=0;row<SurfImg.rows;row++){
      for(int col=0; col<SurfImg.cols;col++){
	if(this->SurfImg.at<uchar>(row,col) == 1){
	  point.set(row,col);
	  if(point.checkForward() && this->SurfImg.at<uchar>(row+1,col) == 0){
	    Silhouette.push_back(point);}
	  else if(point.checkBackward() && this->SurfImg.at<uchar>(row-1,col) == 0){
	    Silhouette.push_back(point);}	  
	  else if(point.checkUp() && this->SurfImg.at<uchar>(row,col+1) == 0){
	    Silhouette.push_back(point);}
	  else if(point.checkDown() && this->SurfImg.at<uchar>(row,col-1) == 0){
	    Silhouette.push_back(point);}
	}//end if checking Init(row,collum)
      }//end of for loop col
    }//end of for loop row
  }

  /**
   *ProjectPoint
   *Function used to project a point onto SurfImg and ProjImg.
   *@param point cv::vec3i that will be projected to the image plane
   */
  inline void ProjectPoint(cv::Vec3i const &point){
    using namespace cv;
    using namespace std;
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
    if(dist == 0){
	if(u1[0] >= 0 && u1[0] < ProjImg.rows && u1[1] >= 0 && u1[1] < ProjImg.cols){ 
	    SurfImg.at<uchar>(u1[0],u1[1]) = 1;
	}
    }
    else{
	std::vector<Vec2i > pointl;
	ivaCircle(u1,dist,Img.size(),pointl);
	typename std::vector<Vec2i >::iterator i = pointl.begin(); 
	for(i;i<pointl.end();i++){
	    if((*i)[0] >= 0 && (*i)[0] < ProjImg.rows && (*i)[1] >= 0 && (*i)[1] < ProjImg.cols){ 
		SurfImg.at<uchar>((*i)[0],(*i)[1]) = 1;
	    }
	}
    }
  };
  
  /**
   *ProjectPoint
   *Function used to project a point onto SurfImg and ProjImg.
   *@param point cv::vec3i that will be projected to the image plane
   */
  std::vector<cv::Vec2i > inline  piPoint(cv::Vec3i const &point){
    using namespace cv;
    using namespace std;
    //get the world point Wpoint in (x,y,z) coordinates 
    cv::Vec3d Wpoint;
    Surface.getPoint(point,Wpoint);
    //Project the world point onto the image
    cv::Vec2d u1 = Vec2d(intrin(1,2) + (intrin(1,1)*Wpoint[1]/Wpoint[2]), intrin(0,2) + (intrin(0,0)*Wpoint[0]/Wpoint[2]));
    //normalize the world point Wpoint_n
    cv::Vec3d Wpoint_n = Wpoint/(Wpoint[0]*Wpoint[0] + Wpoint[1]*Wpoint[1] + Wpoint[2]*Wpoint[2]);
    //Compute V = <Wpoint_n,(1,0,0)>Wpoint_n - (1,0,0)
    cv::Vec3d V(Wpoint_n[0]*Wpoint_n[0]-1,Wpoint_n[0]*Wpoint_n[1],Wpoint_n[0]*Wpoint_n[2]);
    //Compute V2 and project it onto the image
    cv::Vec3d V2 = Wpoint - V*(Surface.getScale()/(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]));
    cv::Vec2d u2 = cv::Vec2i( intrin(1,2) + (intrin(1,1)*V2[1]/V2[2]), intrin(0,2) + (intrin(0,0)*V2[0]/V2[2]));
    //Compute the distance between u and u2
    double dist = sqrt((u1[0]-u2[0])*(u1[0]-u2[0]) + (u1[1]-u2[1])*(u1[1]-u2[1]));
    //get the list of Vec2i that are in the circle and set the image of the list of point =1 and push back the 3dpoint
    std::vector<cv::Vec2i > pointl;
    if(dist == 0){
	if(u1[0] >= 0 && u1[0] < SurfImg.rows && u1[1] >= 0 && u1[1] < SurfImg.cols){ 
	  pointl.push_back((cv::Vec2i) u1);
	}
    }
    else{
	ivaCircle(u1,dist,Img.size(),pointl);
    }
    return pointl;
  };
  /**
   *getImg
   *Returns the Img image
   */
  inline cv::Mat & getImg(){return this->Img;};

  inline cv::Mat const & getImg() const {return this->Img;};
  /**
   *getSurfImg
   *Get the projected image
   */
  inline cv::Mat & getSurfImg(){return this->SurfImg;};

  inline cv::Mat const & getSurfImg() const {return this->SurfImg;};

  /**
   *getSurface
   *Return the Object3D that represents the surface
   */
  inline Object3D & getSurface(){return Surface;};

  inline Object3D const & getSurface() const {return Surface;};

  /**
   *getOcc
   *Returns a list of the occluding curve
   */
  inline vlist & getOcc(){ return Occ;};

  inline vlist const & getOcc() const { return Occ;};
  
  /**
   *getSilhouette
   *returns the Silhouette 
   */
  inline plist & getSilhouette(){ return Silhouette;};

  inline plist const & getSilhouette() const { return Silhouette;};

  /**
   *getCord
   *Return the value of the cordinates of the camera in the world frame
   */
  inline cv::Vec3d & getCord(){return this->cord;};

  inline cv::Vec3d const & getCord() const {return this->cord;};
  
  /**
   *getRot
   *Returns the rotation matrix of the camera
   */
  inline cv::Matx<double,3,3> &getRot(){ return this->rot;};
  
  inline cv::Matx<double,3,3> const &getRot() const {return this->rot;};

  /**
   *getIntrin
   *Returns the intrinsic parameter matrix
   */
  inline cv::Matx<double,3,3> &getIntrin(){ return intrin;};
  
  inline cv::Matx<double,3,3> const &getIntrin() const { return intrin;};

  /**
   *getProjImg
   *Returns the Projection Plane of this camera
   */
  inline ProjPlane &getProjImg(){ return ProjImg;};

  inline ProjPlane const &getProjImg() const { return ProjImg;};


  /**
   *ProjectOccPoint
   *Project the value of a point on to the SurfImg and ProjImg
   *@param point cv::Vec3i that contians the point in Surf
   */
  inline void ProjectOccPoint(cv::Vec3i const &point);

  /**
   *computeOcc
   *This function computes the occluding curve of the surface with respect to this camera
   */
  void computeOcc();
  /**
   *computeG
   *computes the value of g from the rot matrix
   */  
  inline void computeG(){
    double zeta = acos((trace(this->rot)-1)/2);
    if(abs(zeta - 3.141592654) < 0.0001){
      cv::Mat eigenValues;
      cv::Mat eigenVectors;
      eigen(this->rot,eigenValues,eigenVectors);
      for(int row = 0;row<eigenValues.rows; row++){
	if(eigenValues.at<double>(row,1) == 1){
	  g = cv::Vec3d(eigenVectors.at<double>(1,1),eigenVectors.at<double>(1,2),eigenVectors.at<double>(1,3))*zeta;
	  break;
	}
      }
    }else if(abs(zeta)< 0.0001){
      g = cv::Vec3d(0,0,0);
    }else{
      g = cv::Vec3d(rot(2,1)-rot(1,2),rot(0,2)-rot(2,0),rot(1,0)-rot(0,1));
      g = zeta/(2*sin(zeta))*g;
   }
      
  }
  
  
   /**
   *SetImg
   *Set the World image of the camera
   *@param Img cv::Mat that will be set to the Img;
   */
  inline void setImg(cv::Mat const &Img){
    this->Img = Img;
  }
  
  /**
   *setIntrin
   *Set the intrinsic cv::Matx 
   *@param Intrin cv::Matx<double, 3,3> of intrinsic values of the camera
   */
  inline void setIntrin(cv::Matx<double,3,3> Intrin){
    this->intrin = Intrin;
  }

  /**
   *setG
   *Set the value of g
   *@param g the cv::Vec3d that will be set as the Cameras g
   */
  inline void setG(cv::Vec3d const &g){
    this->g = g;
    double zeta = norm(g);
    cv::Vec3d nZ = g/zeta;
    this->rot = cv::Matx<double,3,3>::eye() + sin(zeta)*cv::Matx<double,3,3>(0,-nZ[2],nZ[1],nZ[2],0,-nZ[0],-nZ[1],nZ[0],0) +(1 - cos(zeta))*(nZ*nZ.t() - cv::Matx<double,3,3>::eye());
    Surface.setRot(this->rot.inv());
  }
  /**
   *setSurface
   *set the surface of the camera
   */
  inline void setSurface(Object3D const &surface){ this->Surface = Surface;}

  /**
   *moveCamera
   *This function moves the camera by a rotation matrix and a translation vector
   *@param R a cv::Matx<double,3,3> Rotation matrix used to rotate the camera by
   *@param tran a cv::Vec3d used to represent the translation vector
   */
  inline void moveCamera(cv::Matx<double,3,3> const &R,cv::Vec3d const &tran){
    if(abs(determinant(R)-1) > 0.00000001)
      throw std::runtime_error("The matrix you have entered is not a Rotational matrix");
    rot = R;
    cord = tran;
    Surface.setCenter(rot.t()*(SurfCenter - tran));
    Surface.setRot(rot.t());
  }

  /**
   *moveCamera
   *This function moves the camera by a rotation matrix and a translation vector
   *@param tran a cv::Vec3d used to represent the translation vector
   */
  inline void moveCamera(cv::Vec3d const &tran){
    cord = tran;
    Surface.setCenter(rot.t()*(SurfCenter - tran));
    Surface.setRot(rot.t());
  }


  /**
   *moveCamera
   *This function moves the camera by a rotation matrix and a translation vector from the origin
   *@param axis a cv::Vec3d used to represent the rotation axis that the camera will be rotated by
   *@param zeta double representing how much the camera will be rotated by
   *@param tran a cv::Vec3d used to represent the translation vector
   */
  inline void moveCamera(cv::Vec3d const &axis,double zeta,cv::Vec3d const &tran){
    using namespace cv;
    Vec3d Naxis = axis/norm(axis);
    Matx<double,3,3> R = Matx<double,3,3>::eye() + sin(zeta)*Matx<double,3,3>(0,-Naxis[2],Naxis[1],Naxis[2],0,-Naxis[0],-Naxis[1],Naxis[0],0) + (1-cos(zeta))*(Naxis*Naxis.t() - Matx<double,3,3>::eye());
    //rotate the camera
    rot = R;
    cord = tran;
    Surface.setCenter(rot.t()*(SurfCenter - tran));    
    Surface.setRot(rot.t());
  }
  /**
   *rotateCamera
   *rotates the camera by a rotational matrix from the origin
   *@param R cv::Matx<double,3,3> that represents the rotation matrix
   */
  inline void rotateCamera(cv::Matx<double,3,3> const &R){
    rot = R;
    Surface.setCenter(rot.t()*(SurfCenter - cord));        
    Surface.setRot(rot.t());
  }
  /**
   *rotateCamera
   *rotates the camera by a rotational matrix from the origin
   *@param R cv::Matx<double,3,3> that represents the rotation matrix
   */
  inline void rotateCamera(cv::Vec3d const axis,double const &zeta){
    using namespace cv;
    Vec3d Naxis = axis/norm(axis);
    Matx<double,3,3> R = Matx<double,3,3>::eye() + sin(zeta)*Matx<double,3,3>(0,-Naxis[2],Naxis[1],Naxis[2],0,-Naxis[0],-Naxis[1],Naxis[0],0) + (1-cos(zeta))*(Naxis*Naxis.t() - Matx<double,3,3>::eye());
    rot = R;
    Surface.setCenter(rot.t()*(SurfCenter - cord));    
    Surface.setRot(rot.t());
  }

private:
  /**
   *Image from actual world
   */
  cv::Mat Img;
  /**
   *Binary Image created from the surface or artificial world
   */
  cv::Mat SurfImg;
  /**
   *Image of list of points projected onto ProjImg
   */
  ProjPlane ProjImg;
  /**
   *Object3D representing the surface being modeled or manipulated by the camera
   */
  Object3D Surface;
  /**
   *cv::Vec3d representing the coordinates of the camera in the world frame
   */
  cv::Vec3d cord;
  /**
   *R^3 representation of the rotation matrix in terms of the exponential mapping
   */
  cv::Vec3d g;
  /**
   *cv::Matx representing the rotation matrx of the camera in the world view
   */ 
  cv::Matx<double, 3,3> rot;
  /**
   *cv::Matx used to represent the intrinsic camera parameters
   */
  cv::Matx<double, 3,3> intrin;
  /**
   *plist used to store the points on the occluding curve
   */
  vlist Occ;
  /**
   *cv::Vec3d the coordinates of the center of the surface
   */
  cv::Vec3d SurfCenter;
  /**
   *plist used to store the value of the silhouette  curve
   */
  plist Silhouette;
};


#endif

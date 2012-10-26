#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

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
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<math.h>
#include"IVALib/ActiveSurfaces/SFM_point.hpp"
#include"IVALib/Utils/Utils.hpp"

typedef std::vector<SFM_point<double> > plist;

class Object3D {
public:

  /**
   *Constructor
   */
  Object3D(){
    labelMap = NULL;
    Phi = NULL;
  };

  /**
   *Constructor
   *@param labelMap label map that represents the 3D object
   */
  Object3D(arma::cube const &labelMap);
  /**
   *Constructor
   *@param labelMap label map that represents the 3D object
   *@param Phi sign distance function that represents the 3D object
   *@param Scale sale that represents distance between each voxel
   */
  Object3D(arma::cube const &labelMap, arma::cube const &Phi, double const &scale);
  /**
   *Constructor
   *@param labelMap label map that represents the 3D object
   *@param Phi sign distance function that represents the 3D object
   *@param Scale sale that represents distance between each voxel
   *@param cord cordinates of the top left back corner of the lable map
   */
  Object3D(arma::cube  const &labelMap, arma::cube const &Phi, double const &scale, cv::Vec3d const &cord);  
  /**
   *Constructor
   *@param labelMap label map that represents the 3D object
   *@param Phi sign distance function that represents the 3D object
   *@param Scale sale that represents distance between each voxel
   *@param cord cordinates of the top left back corner of the lable map
   */
  Object3D(arma::cube  const &labelMap, double const &scale, cv::Vec3d const &cord);
  /**
   *Constructor
   *@param labelMap label map that represents the 3D object
   *@param Phi sign distance function that represents the 3D object
   *@param Scale sale that represents distance between each voxel
   */
  Object3D(arma::cube  const &labelMap, double const &scale);


  /**
   *Copy Constructor
   *@param Object3D that you will copy into the new Object3D
   */
  Object3D(Object3D const &Obj){
    if(Obj.labelMap != NULL)
      this->labelMap = new arma::cube(*Obj.labelMap);
    else
      this->labelMap = NULL;
    if(Obj.Phi != NULL)
      this->Phi = new arma::cube(*Obj.Phi);
    else 
      this->Phi = NULL;
    this->scale = Obj.scale;
    this->cord = Obj.cord;
    this->Lz = Obj.Lz;
    this->rot = Obj.rot;
    this->Center = Obj.Center;
  }
  /**
   *copy assignment operator
   */
  Object3D & operator= (const Object3D &Obj){
    if(this != &Obj){

      if(Obj.labelMap != NULL){
	arma::cube *temp_label = new arma::cube;
	*temp_label = *Obj.labelMap;      
	delete this->labelMap; 
	this->labelMap = temp_label;}
      else
	this->labelMap = NULL;
      if(Obj.Phi != NULL){
	arma::cube *temp_phi = new arma::cube;
	*temp_phi = *Obj.Phi;
	delete this->Phi;
	this->Phi = temp_phi;}
      else
	this->Phi = NULL;
      this->scale = Obj.scale;
      this->cord = Obj.cord;
      this->Lz = Obj.Lz;
      this->rot = Obj.rot;
      this->Center = Obj.Center;
    }
    return *this;
  };

  /**
   *Destructor
   */
  ~Object3D(){
    delete labelMap;
    delete Phi;
  }
  /**
   *computeLz
   *computes Lz from the label map
   */
  inline void computeLz(){
    if(this->labelMap != NULL){
      this->Lz.clear();
      //initializes the zero-level set
      SFM_point<double> point(labelMap->n_rows,labelMap->n_cols,labelMap->n_slices);
      for(int slice = 0; slice < this->labelMap->n_slices;slice++){
	//run through rows
	for(int row = 0; row < this->labelMap->n_rows; row++){
	  //next run through collums 
	  for(int collum =0; collum < this->labelMap->n_cols;collum++){
	    //check to see if the point is in the zero levelset
	    point.set(row,collum,slice);           
	    if((*this->labelMap)(row,collum,slice) == 1){
	      if(point.checkForward() && (*this->labelMap)(point[0] +1,point[1],point[2]) == 0)
		this->Lz.push_back(point);
	      else if(point.checkBackward() && (*this->labelMap)(point[0] -1,point[1],point[2]) == 0)
		this->Lz.push_back(point);
	      else if(point.checkUp() && (*this->labelMap)(point[0],point[1]+1,point[2]) == 0)
		this->Lz.push_back(point);
	      else if(point.checkDown() && (*this->labelMap)(point[0],point[1]-1,point[2]) == 0)
		this->Lz.push_back(point);
	      else if(point.checkFSlice() && (*this->labelMap)(point[0],point[1],point[2]+1) == 0)
		this->Lz.push_back(point);
	      else if(point.checkBSlice() && (*this->labelMap)(point[0],point[1],point[2]-1) == 0)
		this->Lz.push_back(point);
	    }//end if checking labelMap(row,collum)
	  }//end collum for loop
	}//end row for loop
      }//end slice for loop
    }//end check NULL labelMap
  }


  /**
   *getLabelMap
   *Returns the labelMap of the Surface
   */
  inline arma::cube & getLabelMap(){ return *labelMap;};  

  inline arma::cube const & getLabelMap() const { return *labelMap;};  
  /**
   *getCord
   *Function that returns the value of the coordinate vector
   */
  inline cv::Vec3d & getCord(){return this->cord;};

  inline cv::Vec3d const & getCord() const {return this->cord;};
  /**
   *Function to get the point in the world frame from a point in the label map
   *@param point cv::Vec3i in the label map that will be transformed into the world frame
   *@param output cv::Vec3d that will represent the point in real life. The tuple is represented as (x,y,z)
   */
  inline void  getPoint(cv::Vec3i const &point, cv::Vec3d &output){
    cv::Vec3d Wpoint(point[1],point[0],point[2]);
    output = cord + (this->rot*Wpoint)*scale;
  };

  inline void  getPoint(cv::Vec3i const &point, cv::Vec3d &output) const{
    cv::Vec3d Wpoint(point[1],point[0],point[2]);
    output = cord + (this->rot*Wpoint)*scale;
  };
  /**
   *getXo
   *get the point from the center set at (0,0,0) and R = I
   *@param point cv::Vec3i that will be transformed into the centered frame
   *@param point cv::Vec3d that will represent the point in the center frame. The tuple is represented as (x,y,z)
   */
  inline void getXo(cv::Vec3i const &point, cv::Vec3d &output){
    cv::Vec3d Wpoint(point[1],point[0],point[2]);
    output = Wpoint*scale - cv::Vec3d(labelMap->n_cols/2.0,labelMap->n_rows/2.0,labelMap->n_slices/2.0)*scale;
  }

  inline void getXo(cv::Vec3i const &point, cv::Vec3d &output) const {
    cv::Vec3d Wpoint(point[1],point[0],point[2]);
    output = Wpoint*scale - cv::Vec3d(labelMap->n_cols/2.0,labelMap->n_rows/2.0,labelMap->n_slices/2.0)*scale;
  }
  /**
   *getScale
   *Function that returns the value of the coordinate vector
   */
  inline double & getScale(){return this->scale;};

  inline double const & getScale() const {return this->scale;};
  /**
   *getLz
   *Function that returns Lz 
   */
  inline plist & getLz(){return this->Lz;};

  inline plist const & getLz() const {return this->Lz;};
  /**
   *getCenter
   *Returns the center of the object3d in a cv::Vec3d
   */
  inline cv::Vec3d &getCenter(){ return this->Center;};

  inline cv::Vec3d const &getCenter() const { return this->Center;}; 
  /**
   *getRot
   *Returns the rotation matrix
   */
  inline cv::Matx<double,3,3> &getRot(){ return this->rot;};

  inline cv::Matx<double,3,3> const &getRot() const {return this->rot;};

  /**
   *SetCenter
   *Set the surface center to a specific location
   *@param cord cv::Vec3d that represents where the center of the surface should be
   */
  inline void setCenter(cv::Vec3d const &center_cord){
    this->Center = center_cord;
    cv::Vec3d center = cv::Vec3d(labelMap->n_cols/2.0,labelMap->n_rows/2.0,labelMap->n_slices/2.0);
    this->cord = center_cord -(this->rot*center)*scale;
  };
  /**
   *setLabelMap
   *Sets a new labelmap and definds the surface region. All other regional parameters are mantained 
   *@param arma::cube that represents the new label map
   */
  inline void setLabelMap(arma::cube &label){
    //set the labelMap pointer
    *this->labelMap = label;
    if(this->Phi != NULL)
      delete this->Phi;
    this->Phi = NULL;
    this->Lz.clear();
    //initializes the zero-level set
    SFM_point<double> point(label.n_rows,label.n_cols,label.n_slices);
    for(int slice = 0; slice < this->labelMap->n_slices;slice++){
	//run through rows
	for(int row = 0; row < this->labelMap->n_rows; row++){
	    //next run through collums 
	    for(int collum =0; collum < this->labelMap->n_cols;collum++){
		//check to see if the point is in the zero levelset
		point.set(row,collum,slice);           
		if((*this->labelMap)(row,collum,slice) == 1){
		    if(point.checkForward() && (*this->labelMap)(point[0] +1,point[1],point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkBackward() && (*this->labelMap)(point[0] -1,point[1],point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkUp() && (*this->labelMap)(point[0],point[1]+1,point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkDown() && (*this->labelMap)(point[0],point[1]-1,point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkFSlice() && (*this->labelMap)(point[0],point[1],point[2]+1) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkBSlice() && (*this->labelMap)(point[0],point[1],point[2]-1) == 0)
		      this->Lz.push_back(point);
		}//end if checking labelMap(row,collum)
	    }//end collum for loop
	}//end row for loop
    }//end slice for loop
  }
  
  /**
   *setLabelMap
   *set the label map from a pointer (only use if you know what you are doing)
   *@param label arma::cube pointer that contains the new labelMap
   */
  inline void setLabelMap(arma::cube *label){
    //set the labelMap pointer
    if(this->labelMap != NULL)
      delete this->labelMap;
    this->labelMap = label;
    if(this->Phi != NULL){
      delete this->Phi;
      this->Phi = NULL;}
    this->Lz.clear();
    //initializes the zero-level set
    SFM_point<double> point(label->n_rows,label->n_cols,label->n_slices);
    for(int slice = 0; slice < this->labelMap->n_slices;slice++){
	//run through rows
	for(int row = 0; row < this->labelMap->n_rows; row++){
	    //next run through collums 
	    for(int collum =0; collum < this->labelMap->n_cols;collum++){
		//check to see if the point is in the zero levelset
		point.set(row,collum,slice);           
		if((*this->labelMap)(row,collum,slice) == 1){
		    if(point.checkForward() && (*this->labelMap)(point[0] +1,point[1],point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkBackward() && (*this->labelMap)(point[0] -1,point[1],point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkUp() && (*this->labelMap)(point[0],point[1]+1,point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkDown() && (*this->labelMap)(point[0],point[1]-1,point[2]) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkFSlice() && (*this->labelMap)(point[0],point[1],point[2]+1) == 0)
		      this->Lz.push_back(point);
		    else if(point.checkBSlice() && (*this->labelMap)(point[0],point[1],point[2]-1) == 0)
		      this->Lz.push_back(point);
		}//end if checking labelMap(row,collum)
	    }//end collum for loop
	}//end row for loop
    }//end slice for loop
  }
  
  /**
   *setCord
   *Function that sets the coordinate of the surface
   */
  inline void setCord(cv::Vec3d const &cord){
    this->cord = cord;
    this->Center = this->cord + scale*this->rot*cv::Vec3d(this->labelMap->n_cols/2,this->labelMap->n_rows/2,this->labelMap->n_slices/2);
  };  
  /**
   *setRot
   *Sets the rotation matrix
   *@param R cv::Matx<double,3,3> is a rotation matrix that will be set to rot
   */  
  inline void setRot(cv::Matx<double,3,3> const &R){
    if(abs(determinant(R)-1) >= 0.00000000000000001)
      throw std::runtime_error("The matrix you have entered is not a Rotational matrix");
    this->rot = cv::Matx<double,3,3>::eye();
    this->Rotate(R);
  };
  /**
   *setRot
   *Sets the rotation matrix
   *@param axis a cv::Vec3d used to represent the rotation axis that the camera will be rotated by
   *@param zeta double representing how much the camera will be rotated by
   *@param tran a cv::Vec3d used to represent the translation vector
   */  
  inline void setRot(cv::Vec3d const &axis,double zeta){
    using namespace cv;
    using namespace std;
    Vec3d Naxis = axis/norm(axis);
    Matx<double,3,3> R = Matx<double,3,3>::eye() + sin(zeta)*Matx<double,3,3>(0,-Naxis[2],Naxis[1],Naxis[2],0,-Naxis[0],-Naxis[1],Naxis[0],0) + (1-cos(zeta))*(Naxis*Naxis.t() - Matx<double,3,3>::eye());
    if(abs(determinant(R)-1) >= 0.00000000000000001){
      cout<<"This is the value of det(R) : "<< determinant(R)<<endl;
      throw std::runtime_error("The matrix you have entered is not a Rotational matrix");
    }
    this->rot = cv::Matx<double,3,3>::eye();
    this->Rotate(R);
  }
   /**computePhi
   *Function that computes Phi from the label map
   */
   inline void computePhi();
  /**
   *Rotate
   *Function that rotates the object by a matrix R
   *@param R cv::Matx<double,3,3> that represents how much the object will be rotated by
   */
  inline void Rotate(cv::Matx<double,3,3> const &R){
    if(abs(determinant(R)-1) >= 0.00000000000000001)
      throw std::runtime_error("The matrix you have entered is not a Rotational matrix");

    this->rot = R*this->rot;
    this->setCord(this->Center - scale*this->rot*cv::Vec3d(this->labelMap->n_cols/2,this->labelMap->n_rows/2,this->labelMap->n_slices/2));
  }
  /**
   *Rotate
   *Function that rotates the object in the Z direction
   *@param Z cv::Vec3d that represents how much the object will be rotated by
   */
  inline void Rotate(cv::Vec3d const &Z){
    double Z_n = sqrt(Z[0]*Z[0] + Z[1]*Z[1] + Z[2]*Z[2]);
    cv::Vec3d nZ = Z/Z_n;
    cv::Matx<double,3,3> R = cv::Matx<double,3,3>::eye() + sin(Z_n)*cv::Matx<double,3,3>(0,-nZ[2],nZ[1],nZ[2],0,-nZ[0],-nZ[1],nZ[0],0) +(1 - cos(Z_n))*(nZ*nZ.t() - cv::Matx<double,3,3>::eye());
    if(abs(determinant(R)-1) >= 0.00000000000000001)
      throw std::runtime_error("The matrix you have entered is not a Rotational matrix");
    this->rot = R*this->rot;
    this->setCord(this->Center - scale*this->rot*cv::Vec3d(this->labelMap->n_cols/2,this->labelMap->n_rows/2,this->labelMap->n_slices/2));
  }

private:
  /**
   *arma::cube representing the label map of the object with 1 for the foreground and 0 for the background. The rows are the y axis,
   *the collumns are the x axis, and the z axis is the slices.
   */
  arma::cube *labelMap;
  /**
   *arma::cube represention the sign distance function of the 3D object
   */
  arma::cube *Phi;
  /**
   *double that represents the scale between points in the label map
   */
  double scale;
  /**
   *cv::Vec3D representing the cordinate of the point labelMap(0,0,0) in the world frame. In other words this represents the 
   *cordinate of the top right corner of the labelMap. The coordinates are stored (x,y,z). 
   */
  cv::Vec3d cord;
  /**
   *cv::Matx<double,3,3> that represents the rotation matrix that rotates the block the object is in
   */
  cv::Matx<double,3,3> rot;
  /**
   *std::vector<SFM_Point<double> > representing the label map points of the surface
   */
  std::vector<SFM_point<double> > Lz;
  /**
   *cv::Vec3d used to represent the center of the element in the workd frame
   */
  cv::Vec3d Center;

};

#endif

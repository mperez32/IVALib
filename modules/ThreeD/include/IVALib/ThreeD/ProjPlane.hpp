#ifndef _PROJPLANE_H_
#define _PROJPLANE_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * This file contains the header that  contains the decleration of the ProjPlane 
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

typedef std::vector<cv::Vec3i> vlist;


class ProjPlane {
public:
  /**
   *Constructor
   */
  ProjPlane(){
    data = NULL;
    rdata = NULL;
  }
  /**
   *Constructor
   *@param rows number of rows that the ProjPlane has
   *@param cols number of collumns that the ProjPlane has 
   */
  ProjPlane(int const &rows, int const &cols){
    data = new vlist[rows*cols];
    rdata = new vlist*[rows];
    for(int i=0;i<rows;i++){
      rdata[i] = data + i*cols;
    }
    this->rows = rows;
    this->cols = cols;
  }
  /**
   *Destructor
   */
  ~ProjPlane(){
    delete[] data;
    delete[] rdata;
  }
  /**
   *Copy constructor
   */
  inline ProjPlane(ProjPlane const &obj){
    //copy memory
    data = new vlist[obj.rows*obj.cols];
    rdata = new vlist*[obj.rows];
    this->rows = obj.rows;
    this->cols = obj.cols;
    for(int i = 0;i<obj.rows*obj.cols;i++)
      this->data[i] = obj.data[i];
    for(int i = 0;i<this->rows;i++)
      rdata[i] = data + i*this->cols;   
  }
  
  
  /**
   *assign operator
   */
  inline ProjPlane operator=(ProjPlane const &obj){
    //Check to see if obj is null or self assignment
    if(&obj != this && obj.data != NULL){
      vlist *temp_data = new vlist[obj.rows*obj.cols];
      vlist **temp_rdata = new vlist*[obj.rows];
      for(int i = 0;i<obj.rows*obj.cols;i++)
	temp_data[i] = obj.data[i];
      for(int i = 0;i<obj.rows;i++)
	temp_rdata[i] = temp_data + i*obj.cols;   
      
      if(this->data != NULL){
	delete[] data;
	delete[] rdata;
      }
      data = temp_data;
      rdata = temp_rdata;
      this->rows = obj.rows;
      this->cols = obj.cols;      
    }else if(obj.data == NULL){
      data = NULL;
      rdata = NULL;
    }
    return *this;    
  }

  /**
   *clear
   *clears all the elements in the ProjPlane
   */
  inline void clear(){
    for(int i = 0;i<rows*cols;i++){
      data[i].clear();
    }
  }
  
  /**
   *setSize
   *set the size of the Projection Plane
   *@param rows int that represents how many rows will be set
   *@param cols int that represents how many cols will be set
   */
  inline void setSize(int rows,int cols){
    this->rows = rows;
    this->cols = cols;
    if(data == NULL)
      delete data;
    if(rdata == NULL)
      delete rdata;
    this->data = new vlist[rows*cols];
    this->rdata = new vlist*[rows];
    for(int i = 0;i<rows;i++){
      rdata[i] = data + i*cols;
    }
  }
  /**
   *row
   *Function that returns the pointer to the row of the ProjPlane
   *@param row the row that will be returned
   */
  inline vlist* row(int row){ return rdata[row];};
  /**
   *number of rows
   */
  int rows;
  /**
   *number of collumns
   */
  int cols;

protected:
  /**
   *Pointer to the pointers to vlist
   */
  vlist **rdata;
  /**
   *Pointer to vlist data
   */
  vlist *data;
};

#endif

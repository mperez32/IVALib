#ifndef UTILS_H_
#define UTILS_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Header file that contains a bunch of helper functions
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/


#include <stdexcept> 
#include <armadillo_bits/config.hpp>
#include <armadillo>
#include "opencv2/opencv.hpp"
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>


/**
 *Function used to convert an arma::Cube to a cv::Mat 
 *@param input the variable that you wish to convert into cv::Mat
 */
template<class T>
inline cv::Mat armaCube2cvMat(arma::Cube<T> const &input){
  int dims [3];
  dims[0] = input.n_rows;
  dims[1] = input.n_cols;
  dims[2] = input.n_slices;
  cv::Mat output(3,dims,input.memptr(),sizeof(T));
  return output;
} 
 
/**
 *Funtion used to convert a cv::Mat_ to arma::Cube. This method is slow if the matrix is not continuous
 *@param input the variable you with to convert into an arma::Cube
 */
template<class T>
inline arma::Cube<T> cvMat2armaCube(cv::Mat_<T> const &input){
  if(input.dims != 3){
    throw std::runtime_error("The input Mat is not 3 dimensional.");
    exit(0);
  }
 
  if(input.isContinuous()){
    arma::Cube<T> output((T*) input.ptr(),input.size.p[0],input.size.p[1],input.size.p[2]);
  }
  else{
    arma::Cube<T> output(input.size.p[0],input.size.p[1],input.size.p[2]);
    for(int slice = 0;slice<input.size.p[2];slice++){
      for(int row = 0;row<input.size.p[0];row++){
	for(int col = 0;col<input.size.p[1];col++){
	  output(row,col,slice) = input(row,col,slice);
	}
      }
    }
  }
}



/**
 *Function used to convert an Armadillo Cube into a vtkImageData
 *@param input the variable that you wish to convert into vtkImageData
 *@param output the variable that will become your vtkImageData in the form of vtkSmartPointer<vtkImageData>
 */
 inline void ArmaCube2VTKImageData(arma::cube const &input, vtkSmartPointer<vtkImageData> &output){
   
   if(input.memptr() == NULL){
     throw std::runtime_error("The input Cube is NULL.");
     exit(0);
   }
   vtkImageImport *importer = vtkImageImport::New();
   importer->SetOutput(output);
   importer->SetDataSpacing( 1, 1, 1 );
   importer->SetDataOrigin( 0, 0, 0 );
   importer->SetWholeExtent( 0, input.n_rows-1, 0, input.n_cols-1, 0, input.n_slices-1); 
   importer->SetDataExtentToWholeExtent();
   importer->SetDataScalarTypeToDouble();
   importer->SetNumberOfScalarComponents(1);
   importer->SetImportVoidPointer((void *) input.memptr());
   importer->Update();
 
}
/**
 *Function used to convert an OpenCV Mat into a vtkImageData
 *@param input the variable that you wish to convert into vtkImageData
 *@param output the variable that will become your vtkImageData in the form of vtkSmartPointer<vtkImageData> 
*/
 inline void CVMat2VTKImageData(cv::Mat &input, vtkSmartPointer<vtkImageData> &output){
   
   if(input.data == NULL){
     throw std::runtime_error("The input Mat is NULL.");
     exit(0);
   }
   vtkImageImport *importer = vtkImageImport::New();
   if(input.dims == 3){
     importer->SetOutput(output);
     importer->SetDataSpacing( 1, 1, 1 );
     importer->SetDataOrigin( 0, 0, 0 );
     importer->SetWholeExtent( 0, input.size.p[0]-1, 0, input.size.p[1]-1, 0, input.size.p[2]-1);
     importer->SetDataExtentToWholeExtent();
     importer->SetDataScalarTypeToDouble();
     importer->SetNumberOfScalarComponents(input.channels());
     importer->SetImportVoidPointer(input.data);
     importer->Update();
   }
   if(input.dims == 2){
     importer->SetOutput(output);
     importer->SetDataSpacing( 1, 1, 1 );
     importer->SetDataOrigin( 0, 0, 0 );
     importer->SetWholeExtent( 0, input.size.p[0]-1, 0, input.size.p[1]-1, 0, 0);
     importer->SetDataExtentToWholeExtent(); 
     importer->SetDataScalarTypeToDouble();
     importer->SetNumberOfScalarComponents(input.channels());
     importer->SetImportVoidPointer(input.data);
     importer->Update();       
   }

 };
/**
 *Function that returns a list of coordinates of the points that would be occupied by a circle with a given radius
 *@param center the center of the circle
 *@param radius the radius of the circle
 *@param cv::Size that represents the size of the image that will have the circle drawn on
 *@param out vector<cv::Vec2i > that 
 */
inline void ivaCircle(cv::Vec2d const &center, double const &radius, cv::Size const &size, std::vector< cv::Vec2i> &out){
  int min_x,min_y,max_x,max_y,y,x,dx,dy;
  min_x = std::max((int) (center[0] -radius),0);
  min_y = std::max((int) (center[1] -radius),0);
  max_x = std::min((int) (center[0] +radius),size.height-1);
  max_y = std::min((int) (center[1] +radius),size.width-1);
  for(x =min_x; x<=max_x;x++){
    for(y = min_y; y<=max_y; y++){
      dx =(x-center[0]);
      dy =(y-center[1]);
      if( dx*dx + dy*dy <= radius*radius)
	out.push_back(cv::Vec2i(x,y));
    }
  }

}


#endif

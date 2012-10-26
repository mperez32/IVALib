/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the ChanVese class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/

#include<opencv2/highgui/highgui.hpp>
#include "IVALib/ActiveSurfaces/ChanVese.hpp"


using namespace std;



 void ChanVese::Compute(vector<SFM_point<double> > const &Lz, vector<SFM_point<double> > const &Lp1, vector<SFM_point<double> > const &Lp2, vector<SFM_point<double> > const &Ln1,vector<SFM_point<double> > const &Ln2){
  
   if(this->Img->depth() == CV_8U)
     this->computeEner<uchar>(Lz);
   else if(this->Img->depth() == CV_8S)
     this->computeEner<uchar>(Lz);
   else if(this->Img->depth() == CV_16U)
     this->computeEner<ushort>(Lz);
   else if(this->Img->depth() == CV_16S)
     this->computeEner<short>(Lz);
   else if(this->Img->depth() == CV_32S)
     this->computeEner<int>(Lz);
   else if(this->Img->depth() == CV_32F)
     this->computeEner<float>(Lz);
   else if(this->Img->depth() == CV_64F)
     this->computeEner<double>(Lz);
   else{
     throw runtime_error("Your image has an invalid depth.");
     exit(0);}
 }; 

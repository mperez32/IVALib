/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the Bhattacharyya class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/
#include<opencv2/highgui/highgui.hpp>
#include<cv.h>
#include "IVALib/ActiveSurfaces/Bhattacharyya.hpp"


using namespace std;
using namespace cv;

typedef vector<SFM_point<double> > plist;


 void Bhattacharyya::Compute(plist const &Lz, plist const &Lp1, plist const &Lp2, plist const &Ln1,plist const &Ln2){

   //Declare the variables that will be used
   double dx, dy, dxx, dyy, dx2, dy2, dxy, kappa, ener_bhatta;
   bool Xok, Yok;
   Mat SqrtIn;
   Mat SqrtOut;
   int *I = new int[3];
   *H_in = *this->Phi <= 0;
   *H_out = *this->Phi > 0;
   double A_out = countNonZero(*H_out)+0.0000001;
   double A_in = countNonZero(*H_in)+0.0000001;
   if(this->Img->depth() == CV_8U)
     this->Hist<uchar>();
   else if(this->Img->depth() == CV_8S)
     this->Hist<uchar>();
   else if(this->Img->depth() == CV_16U)
     this->Hist<ushort>();
   else if(this->Img->depth() == CV_16S)
     this->Hist<short>();
   else if(this->Img->depth() == CV_32S)
     this->Hist<int>();
   else if(this->Img->depth() == CV_32F)
     this->Hist<float>();
   else if(this->Img->depth() == CV_64F)
     this->Hist<double>();
   else{
     throw runtime_error("Your image has an invalid depth.");
     exit(0);}

   if(this->channels == 1){//declering vaariables for case of black&white image
     P_in[0] = arma::conv(P_in[0],normal[0])+0.0000001;
     P_out[0] = arma::conv(P_out[0],normal[0])+0.0000001;
     P_out[0] = P_out[0].subvec(bins/2-1,bins/2-1+bins )/(arma::sum(P_out[0].subvec(bins/2-1,bins/2-1+bins)));
     P_in[0] = P_in[0].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[0].subvec(bins/2-1,bins/2-1+bins)));
     B =arma::sum(arma::sqrt(P_in[0]%P_out[0]));
     this->V_in[0] = arma::conv(arma::sqrt((this->P_out[0])/(this->P_in[0])+0.0001),*this->normal);
     V_in[0] = V_in[0].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[0] = arma::conv(arma::sqrt((this->P_in[0])/(this->P_out[0])+0.0001),*this->normal);
     V_out[0] = V_out[0].subvec(bins/2-1,bins/2-1+bins);
   }
   else if(this->channels == 2){
     this->P_in[0] = arma::conv(this->P_in[0],*this->normal)+0.000001;
     this->P_in[1] = arma::conv(this->P_in[1],*this->normal)+0.000001;
     this->P_out[0] = arma::conv(this->P_out[0],*this->normal)+0.000001;
     this->P_out[1] = arma::conv(this->P_out[1],*this->normal)+0.000001;
     P_out[0] = P_out[0].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_out[0].subvec(bins/2-1,bins/2-1+bins)));
     P_out[1] = P_out[1].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_out[1].subvec(bins/2-1,bins/2-1+bins)));
     P_in[0] = P_in[0].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[0].subvec(bins/2-1,bins/2-1+bins)));
     P_in[1] = P_in[1].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[1].subvec(bins/2-1,bins/2-1+bins)));
     B = arma::sum(arma::sqrt(P_in[0]%P_out[0]))*arma::sum(arma::sqrt(P_in[1]%P_out[1]));
     
     this->V_in[0] = arma::conv(arma::sqrt((this->P_out[0])/(this->P_in[0])+0.0001),*this->normal);
     V_in[0] = V_in[0].subvec(bins/2-1,bins/2-1+bins);
     this->V_in[1] = arma::conv(arma::sqrt((this->P_out[1])/(this->P_in[1])+0.0001),*this->normal);
     V_in[1] = V_in[1].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[0] = arma::conv(arma::sqrt((this->P_in[0])/(this->P_out[0])+0.0001),*this->normal);
     V_out[0] = V_out[0].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[1] = arma::conv(arma::sqrt((this->P_in[1])/(this->P_out[1])+0.0001),*this->normal);
     V_out[1] = V_out[1].subvec(bins/2-1,bins/2-1+bins);

   }
   else if(this->channels == 3){// declering variables for case of color image
     this->P_in[0] = arma::conv(this->P_in[0],*this->normal)+0.000001;
     this->P_in[1] = arma::conv(this->P_in[1],*this->normal)+0.000001;
     this->P_in[2] = arma::conv(this->P_in[2],*this->normal)+0.000001;
     this->P_out[0] = arma::conv(this->P_out[0],*this->normal)+0.000001;
     this->P_out[1] = arma::conv(this->P_out[1],*this->normal)+0.000001;
     this->P_out[2] = arma::conv(this->P_out[2],*this->normal)+0.000001;
     P_out[0] = P_out[0].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_out[0].subvec(bins/2-1,bins/2-1+bins)));
     P_out[1] = P_out[1].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_out[1].subvec(bins/2-1,bins/2-1+bins)));
     P_out[2] = P_out[2].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_out[2].subvec(bins/2-1,bins/2-1+bins)));
     P_in[0] = P_in[0].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[0].subvec(bins/2-1,bins/2-1+bins)));
     P_in[1] = P_in[1].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[1].subvec(bins/2-1,bins/2-1+bins)));
     P_in[2] = P_in[2].subvec(bins/2-1,bins/2-1+bins)/(arma::sum(P_in[2].subvec(bins/2-1,bins/2-1+bins)));
     B = arma::sum(arma::sqrt(P_in[0]%P_out[0]))*arma::sum(arma::sqrt(P_in[1]%P_out[1]))*arma::sum(arma::sqrt(P_in[2]%P_out[2]));
     
     this->V_in[0] = arma::conv(arma::sqrt((this->P_out[0])/(this->P_in[0])+0.0001),*this->normal);
     V_in[0] = V_in[0].subvec(bins/2-1,bins/2-1+bins);
     this->V_in[1] = arma::conv(arma::sqrt((this->P_out[1])/(this->P_in[1])+0.0001),*this->normal);
     V_in[1] = V_in[1].subvec(bins/2-1,bins/2-1+bins);
     this->V_in[2] = arma::conv(arma::sqrt((this->P_out[2])/(this->P_in[2])+0.0001),*this->normal);
     V_in[2] = V_in[2].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[0] = arma::conv(arma::sqrt((this->P_in[0])/(this->P_out[0])+0.0001),*this->normal);
     V_out[0] = V_out[0].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[1] = arma::conv(arma::sqrt((this->P_in[1])/(this->P_out[1])+0.0001),*this->normal);
     V_out[1] = V_out[1].subvec(bins/2-1,bins/2-1+bins);
     this->V_out[2] = arma::conv(arma::sqrt((this->P_in[2])/(this->P_out[2])+0.0001),*this->normal);
     V_out[2] = V_out[2].subvec(bins/2-1,bins/2-1+bins);
   }
  
   //cout<<"This is B :"<<B<<endl;
   //iterate trhough Lz list and compute energy
   for(plist::const_iterator i = Lz.begin();i < Lz.end();i++){
    
     dx = dy = dxx = dyy = dxy = 0;
     Xok = Yok = false;
     //Compute dx ,dxx, dx2, dy, dyy, dy2, dxy
     if((*i)[0]-1 >= 0 && (*i)[0] +1 < i->getRows()){
       dx = ((*this->Phi)((*i)[0]-1,(*i)[1]) - (*this->Phi)((*i)[0]+1,(*i)[1]))/2;
       dxx = ((*this->Phi)((*i)[0]-1,(*i)[1])-2*(*this->Phi)((*i)[0],(*i)[1]) + (*this->Phi)((*i)[0]+1,(*i)[1]));
       dx2 = dx*dx;
       Xok = true;
     }
      
     if((*i)[1]-1 >= 0 && (*i)[1]+1 < i->getColumns()){ 
       dy = ((*this->Phi)((*i)[0],(*i)[1]-1) - (*this->Phi)((*i)[0],(*i)[1]+1))/2;
       dyy = ((*this->Phi)((*i)[0],(*i)[1]-1)-2*(*this->Phi)((*i)[0],(*i)[1]) + (*this->Phi)((*i)[0],(*i)[1]+1));
       dy2 = dy*dy;
       Yok = true;
     }
      
     if(Xok && Yok){
       dxy = ((*this->Phi)((*i)[0]-1,(*i)[1]-1) + (*this->Phi)((*i)[0]+1,(*i)[1]+1) - (*this->Phi)((*i)[0]+1,(*i)[1]-1) -
	      (*this->Phi)((*i)[0]-1,(*i)[1]+1))/4;
     }

     //Compute curvuture at each point and obtain I(x,y); 
     kappa = (dxx*dy2 + dyy*dx2 -2*dx*dy*dxy)/(dx2 + dy2+ 0.0000001);
     
     if(this->channels == 1){
       I[0] = (int) Img->at<uchar>((*i)[0],(*i)[1]);
       ener_bhatta  = kappa + (1.0/2)*V_in[0].mem[I[0]] - (1.0/2)*V_out[0].mem[I[0]];
     }
     else if(this->channels == 2){
       I[0] = (int) this->Img->at<Vec3b>((*i)[0],(*i)[1])[0];
       I[1] = (int) this->Img->at<Vec3b>((*i)[0],(*i)[1])[1];
       ener_bhatta = 25*kappa +(100.0/2)*B*(1/A_out - 1/A_in)+(100.0*(Img->rows*Img->cols)/(2*A_in))*V_in[0].mem[I[0]]*V_in[1].mem[I[1]]-((Img->rows*Img->cols)/(2*A_out))*V_out[0].mem[I[0]]*V_out[1].mem[I[1]];
     }
     else if(this->channels == 3){
       I[0] = (int) this->Img->at<Vec3b>((*i)[0],(*i)[1])[0];
       I[1] = (int) this->Img->at<Vec3b>((*i)[0],(*i)[1])[1];
       I[2] = (int) this->Img->at<Vec3b>((*i)[0],(*i)[1])[2];	    
       ener_bhatta = 25*kappa +(100.0/2)*B*(1/A_out - 1/A_in)+(100.0*(Img->rows*Img->cols)/(2*A_in))*V_in[0].mem[I[0]]*V_in[1].mem[I[1]]*V_in[2].mem[I[2]]-((Img->rows*Img->cols)/(2*A_out))*V_out[0].mem[I[0]]*V_out[1].mem[I[1]]*V_out[2].mem[I[2]];
     }
     this->setPointF(*i,ener_bhatta);
   }//end for loop that iterates through LZ 
 };

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the Kappa3D class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/
#include "IVALib/ActiveSurfaces/Kappa3D.hpp"


using namespace std;

typedef vector<SFM_point<double> > list;


 void Kappa3D::Compute(vector<SFM_point<double> > const &Lz, vector<SFM_point<double> > const &Lp1, vector<SFM_point<double> > const &Lp2, vector<SFM_point<double> > const &Ln1,vector<SFM_point<double> > const &Ln2){

    //Declare the variables that will be used
   double dx, dy, dz, dzz, dxx, dyy, dx2, dy2, dz2, dxz, dyz, dxy, kappa;
   bool Xok, Yok, Zok;
    for(list::const_iterator i = Lz.begin();i < Lz.end();i++){
    
      dx = dy = dz = dzz = dxx = dyy = dxy = dxz = dyz = 0;
      Xok = Yok = false;
      //Compute dx ,dxx, dx2, dy, dyy, dy2, dz, dzz, dz2, dxz, dyz, dxy
      if((*i)[0]-1 >= 0 && (*i)[0] +1 < i->getRows()){
	dx = ((*Phi3D)((*i)[0]-1,(*i)[1],(*i)[2]) - (*Phi3D)((*i)[0]+1,(*i)[1],(*i)[2]))/2;
	dxx = ((*Phi3D)((*i)[0]-1,(*i)[1],(*i)[2])-2*(*Phi3D)((*i)[0],(*i)[1],(*i)[2]) + (*Phi3D)((*i)[0]+1,(*i)[1],(*i)[2]));
	dx2 = dx*dx;
	Xok = true;
      }
      
      if((*i)[1]-1 >= 0 && (*i)[1]+1 < i->getColumns()){ 
	dy = ((*this->Phi3D)((*i)[0],(*i)[1]-1,(*i)[2]) - (*this->Phi3D)((*i)[0],(*i)[1]+1,(*i)[2]))/2;
	dyy = ((*Phi3D)((*i)[0],(*i)[1]-1,(*i)[2])-2*(*Phi3D)((*i)[0],(*i)[1],(*i)[2]) + (*Phi3D)((*i)[0],(*i)[1]+1,(*i)[2]));
	dy2 = dy*dy;
	Yok = true;
      }

      if((*i)[2]-1 >= 0 && (*i)[2]+1 < i->getSlices()){ 
	dz = ((*Phi3D)((*i)[0],(*i)[1],(*i)[2]-1) - (*Phi3D)((*i)[0],(*i)[1],(*i)[2]+1))/2;
	dzz = ((*Phi3D)((*i)[0],(*i)[1],(*i)[2]-1)-2*(*Phi3D)((*i)[0],(*i)[1],(*i)[2]) + (*Phi3D)((*i)[0],(*i)[1],(*i)[2]+1));
	dz2 = dz*dz;
	Yok = true;
      }
      
      if(Xok && Yok){
	dxy = ((*Phi3D)((*i)[0]-1,(*i)[1]-1,(*i)[2]) + (*Phi3D)((*i)[0]+1,(*i)[1]+1,(*i)[2]) - (*Phi3D)((*i)[0]+1,(*i)[1]-1,(*i)[2]) - (*Phi3D)((*i)[0]-1,(*i)[1]+1,(*i)[2]))/4;
      }

      if(Xok && Zok){
	dxz = ((*Phi3D)((*i)[0]-1,(*i)[1],(*i)[2]-1) + (*Phi3D)((*i)[0]+1,(*i)[1],(*i)[2]+1) - (*Phi3D)((*i)[0]+1,(*i)[1],(*i)[2]-1) - (*Phi3D)((*i)[0]-1,(*i)[1],(*i)[2]+1))/4;
      }

      if(Yok && Zok){
	dyz = ((*Phi3D)((*i)[0],(*i)[1]-1,(*i)[2]-1) + (*Phi3D)((*i)[0],(*i)[1]+1,(*i)[2]+1) - (*Phi3D)((*i)[0],(*i)[1]+1,(*i)[2]-1) - (*Phi3D)((*i)[0],(*i)[1]-1,(*i)[2]+1))/4;
      }
      
      //Compute curvuture at each point and set it to F
      kappa = (dxx*(dy2+dz2)+dyy*(dx2+dz2)+dzz*(dx2+dy2)-2*dx*dy*dxy-2*dx*dz*dxz-2*dy*dz*dyz)/(dx2+dy2+dz2+.00000001);
      this->setPointF(*i,kappa);
 

    }//end for loop that iteratos through LZ 

    
}; 

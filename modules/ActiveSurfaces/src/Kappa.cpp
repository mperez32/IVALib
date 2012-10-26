/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the Kappa class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/
#include "IVALib/ActiveSurfaces/Kappa.hpp"


using namespace std;

typedef vector<SFM_point<double> > list;


 void Kappa::Compute(vector<SFM_point<double> > const &Lz, vector<SFM_point<double> > const &Lp1, vector<SFM_point<double> > const &Lp2, vector<SFM_point<double> > const &Ln1,vector<SFM_point<double> > const &Ln2){

    //Declare the variables that will be used
    double dx, dy, dxx, dyy, dx2, dy2, dxy, kappa;
    bool Xok, Yok;
    for(list::const_iterator i = Lz.begin();i < Lz.end();i++){
    
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

      //Compute curvuture at each point and set it to F
      kappa = (dxx*dy2 + dyy*dx2 -2*dx*dy*dxy)/(dx2 + dy2+ 0.0000001);
      this->setPointF(*i,kappa);
 

    }//end for loop that iteratos through LZ 

    
}; 

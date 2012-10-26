/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the SFM3D class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


#include"IVALib/ActiveSurfaces/SFM3D.hpp"

using namespace std;

/*Initialize
 *This method initializes the L Lists, Label and Phi
 */
template<class T>
void SFM3D<T>::Initialize(){

  if(this->Init == NULL){
    cout<<"The matrix Init has not been set while running Initialize. You must set Init before running Initialize"<<endl;
    exit(0);
  }

  this->Phi = new arma::Cube<T>(Init->n_rows,Init->n_cols,Init->n_slices);
  this->Label = new arma::Cube<int>(Init->n_rows,Init->n_cols,Init->n_slices);

 
  SFM_point<T> point(Init->n_rows,Init->n_cols,Init->n_slices);
  vector<SFM_point<T> > Neighbours;
 

  //run through all points in Init to initialize Phi and Label
  //run through slices
  for(int slice = 0; slice< this->Init->n_slices;slice++){
    //first run through rows 
    for(int row = 0; row < this->Init->n_rows; row++){
      //next run through collums 
      for(int collum =0; collum < this->Init->n_cols;collum++){
	//check to see if the is in the foreground or background
	if((*this->Init)(row,collum,slice) == 0){
	  (*this->Label)(row,collum,slice) = 3;
	  (*this->Phi)(row,collum,slice) = 3;
	}
	else {
	  (*this->Label)(row,collum,slice) = -3;
	  (*this->Phi)(row,collum,slice) = -3;
	}      
      }//end colum for
    }//end row for
  }//end slice for

  //initializes the zero-level set
  for(int slice = 0; slice < this->Init->n_slices;slice++){
    //run through rows
    for(int row = 0; row < this->Init->n_rows; row++){
      //next run through collums 
      for(int collum =0; collum < this->Init->n_cols;collum++){
	//check to see if the point is in the zero levelset
	point.set(row,collum,slice);           
	if((*this->Init)(row,collum,slice) == 1){
	  if(point.checkForward() && (*this->Init)(point[0] +1,point[1],point[2]) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}
	  else if(point.checkBackward() && (*this->Init)(point[0] -1,point[1],point[2]) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}
	  else if(point.checkUp() && (*this->Init)(point[0],point[1]+1,point[2]) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}
	  else if(point.checkDown() && (*this->Init)(point[0],point[1]-1,point[2]) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}
	  else if(point.checkFSlice() && (*this->Init)(point[0],point[1],point[2]+1) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}	  
	  else if(point.checkBSlice() && (*this->Init)(point[0],point[1],point[2]-1) == 0){
	    this->Lz->push_back(point);
	    (*this->Label)(row,collum,slice) = 0;
	    (*this->Phi)(row,collum,slice) = 0;}	  
	}//end if checking Init(row,collum)
      }//end collum for loop
    }//end row for loop
  }//end slice for loop
  
   //find the +1 and -1 levelset
  for(typename vector<SFM_point<T> >::iterator p = this->Lz->begin(); p < this->Lz->end(); p++){
    //check the neighbors of p
    if(p->checkForward()){ 
      if((*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
	(*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) = -1;
	(*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = -1;}
      else if((*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
	(*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) = 1;
	(*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = 1;}
    }//check forward
    if(p->checkBackward()){ 
      if((*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
	(*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) = -1;
	(*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = -1;}
      else if((*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
	(*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) = 1;
	(*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = 1;}
    }//check backward
    if(p->checkUp()){ 
      if((*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
	(*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) = -1;
	(*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = -1;}
      else if((*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
	(*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) = 1;
	(*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = 1;}
    }//check up
    if(p->checkDown()){ 
      if((*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
	(*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) = -1;
	(*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = -1;}
      else if((*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
	(*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) = 1;
	(*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = 1;}
    }//check down
    if(p->checkFSlice()){
      if((*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) = -1;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = -1;}
      else if((*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) = 1;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = 1;}      
    }//check forward slice
    if(p->checkBSlice()){
      if((*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) == -3){	 
	this->Ln1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) = -1;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = -1;}
      else if((*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) == 3){
	this->Lp1->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) = 1;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = 1;}      
    }//check Backward slice    
  }//finishes Lz for loops
   

   //Find the -2 levelset 
   for(typename vector<SFM_point<T> >::iterator p = this->Ln1->begin(); p < this->Ln1->end(); p++){
     //check the neighbors of p
     if(p->checkForward() && (*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
       (*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) = -2;
       (*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = -2;
     }//check forward
     if(p->checkBackward() && (*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
       (*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) = -2;
       (*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = -2;
     }//check backward
     if(p->checkUp() && (*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
       (*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) = -2;
       (*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = -2;
     }//check up
     if(p->checkDown() && (*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
       (*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) = -2;
       (*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = -2;
     }//check down
     if(p->checkFSlice() && (*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
       (*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) = -2;
       (*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = -2;
     }//check forward slice
     if(p->checkBSlice() && (*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) == -3){
       this->Ln2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
       (*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) = -2;
       (*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = -2;
     }//check backward slice     
   }//end Ln1 for

   //Find the 2 levelset
   for(typename vector<SFM_point<T> >::iterator p = this->Lp1->begin(); p < this->Lp1->end(); p++){
     //check the neighbors of p
     if(p->checkForward() && (*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
       (*this->Label)((*p)[0]+1,(*p)[1],(*p)[2]) = 2;
       (*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = 2;
     }//check forward
     if(p->checkBackward() && (*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
       (*this->Label)((*p)[0]-1,(*p)[1],(*p)[2]) = 2;
       (*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = 2;
     }//check backward
     if(p->checkUp() && (*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
       (*this->Label)((*p)[0],(*p)[1]+1,(*p)[2]) = 2;
       (*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = 2;
     }//check up
     if(p->checkDown() && (*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
       (*this->Label)((*p)[0],(*p)[1]-1,(*p)[2]) = 2;
       (*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = 2;
     }//check down
     if(p->checkFSlice() && (*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
       (*this->Label)((*p)[0],(*p)[1],(*p)[2]+1) = 2;
       (*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = 2;
     }//check forward slice
     if(p->checkBSlice() && (*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) == 3){
       this->Lp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
       (*this->Label)((*p)[0],(*p)[1],(*p)[2]-1) = 2;
       (*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = 2;
     }//check forward slice
   }//end Lp1 for loop
}


/*
 *Computes the update Phi(p) = Force(p) + Phi(p) for each point p in Lz by using the Compute() method of the object Force. 
 *The Force object is a functor. The functor class has Compute  as a absolute virtual method. You must override this method 
 *and specify in that method how Phi should be updated. There are more instructions in the functor class.SFM also calls on Normalize() 
 *so your Compute() method should not normalize Phi. 
 *Finally the pointer to Phi will be released since we don't want Force to change Phi outside of the SFM class.
 */
template<class T>
void SFM3D<T>::Compute(){
  
  //Check to see if force has been set.
  if(this->Force == NULL){
    cout<<"You have not set a functor for computing the force. Please do so before calling the Compute method in SFM"<<endl;
    exit(0);
  }
  this->Force->set3D();
  this->Force->setPhi3D(this->Phi);
  this->Force->Compute(*this->Lz,*this->Lp1,*this->Lp2,*this->Ln1,*this->Ln2);
  this->Force->Normalize();
  this->Force->Update();
  this->Force->Release();
}


/*Update
 *This method is used to compute the update of Phi by adding F to Phi(Lz)
 *This method requires to have initialized a functor that describes how to compute F 
 */
template<class T>
void SFM3D<T>::Update(){
  //Check to see if force has been set.
  if(this->Force == NULL){
    cout<<"You have not set a functor for computing the force. Please do so before calling on the Update() method in SFM"<<endl;
    exit(0);
  }

  if(this->Label == NULL || this->Phi == NULL){
    cout<<"You have not initialized your SFM object. Please call on the Intialize() method before calling on the Run()"<<endl;
    cout<<"or the Update() method in SFM"<<endl;
    exit(0);
  }
  //declare variables needed
  double Max;
  double min;

  //Compute the update of Phi(Lz)
  this->Compute();

  //cout<<"This is Phi after Computing : "<<endl;
  //cout<<*this->Phi<<endl;
  
  //Remove points that have Phi out of range from Lz
  typename vector<SFM_point<T> >::iterator p;
  p = this->Lz->begin();
  while(p < this->Lz->end()){
    if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]) > 0.5){
      this->Sp1->push_back(*p);
      p = this->Lz->erase(p);     
    }
    else if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]) < -0.5){
      this->Sn1->push_back(*p);
      p = this->Lz->erase(p);  
    }else{
      p++;
    } 
  } // end Lz for loop

  //Update the -1 and 1 level sets
  //scan through the Ln1 list

  p = this->Ln1->begin();
  while( p < this->Ln1->end()){
    if(CheckNeighboursLabel(*p,0)){
      this->Sn2->push_back(*p);
      this->Ln1->erase(p);     
    }else{
      Max = MaxNeighboursPhi(*p,0);
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) =  Max - 1;
      //check to see if in range if not remove
      if(Max-1>=-0.5){
	this->Sz->push_back(*p);
	p = this->Ln1->erase(p);
      }
      else if(Max-1 < -1.5){
	this->Sn2->push_back(*p);
	p = this->Ln1->erase(p);
      }else{
	p++;	
      }
    }//end else
  }//end Ln1 for loop

  //Scan through Lp1 list
  p = this->Lp1->begin();
  while( p < this->Lp1->end()){
    if(CheckNeighboursLabel(*p,0)){
      this->Sp2->push_back(*p);
      p = this->Lp1->erase(p);       
    }else{
      min = minNeighboursPhi(*p,0);
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) =  min + 1;
      //check to see if in range if not remove
      if(min+1<= 0.5){
	this->Sz->push_back(*p);
	p = this->Lp1->erase(p);
      }
      else if(min+1 > 1.5){
	this->Sp2->push_back(*p);
	p = this->Lp1->erase(p);
      }
      else{
	p++;
      }//end if to check removal of point
    }//end if to check removal the neighbors labels
  }//end Lp1 for loop

  //Update the +2 and -2 level set
  //Scan through Ln2 list
  p = this->Ln2->begin();
  while(p < this->Ln2->end()){
    if(CheckNeighboursLabel(*p,-1)){
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) = -3;
      (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = -3;
      p = this->Ln2->erase(p);
    }
    else{
      Max = MaxNeighboursPhi(*p,-1);
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) =  Max - 1;
      //check to see if in range if not remove
      if(Max-1 >= -1.5){
	this->Sn1->push_back(*p);
	p = this->Ln2->erase(p);
      }
      else if(Max-1 < -2.5){
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]) = -3;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]) = -3;
	p = this->Ln2->erase(p);
      }else{
	p++;
	  }//end if checking to see out of range
    }//end if checking labels
  }//end Ln2 for loop

  //Scan through Lp2 list
  p = this->Lp2->begin();
  while( p < this->Lp2->end()){
    if(CheckNeighboursLabel(*p,1)){
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) = 3;
      (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = 3;
      p = this->Lp2->erase(p);
    }
    else{
      min = minNeighboursPhi(*p,1);
      (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) =  min + 1;
      //check to see if in range if not remove
      if(min + 1 <= 1.5){
	this->Sp1->push_back(*p);
	p = this->Lp2->erase(p);
      }
      else if(min + 1 > 2.5){
	(*this->Label)((*p)[0],(*p)[1],(*p)[2]) = 3;
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]) = 3;
	p = this->Lp2->erase(p);
      }
      else{
	p++;
      }//end if checking out of range
    }//end if checking labels
  }//end Lp2 for loop  


  //declare the Neighbours SFM_point<T>s list used throughout the rest of the function 
  p = this->Sz->begin();
  vector<SFM_point<T> > Neighbours;

  //Move points from Sz into zero level sets
  while(p < this->Sz->end()){
    //set label(p) = 0, add p to Lz and remove p from Sz
    (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = 0;
    this->Lz->push_back(*p);
    p = this->Sz->erase(p);
  }


  //Move points into -1 and +1 Neighbours
  //Ensure -2, +2 neighbours

  //Iterate through the Sn1 list
  p = this->Sn1->begin();
  while(p < this->Sn1->end()){
    //set Label(p) = -1 add p to Ln1
    (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = -1;
    this->Ln1->push_back(*p);
    
    //Iterate through neighbours to  
    //check the neighbours and make sure they are -2 away
    if(p->checkForward()){
      if((*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) == -3){
	(*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
      }
    }
    if(p->checkBackward()){
      if((*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) == -3){
	(*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
      }
    }
    if(p->checkUp()){
      if((*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) == -3){
	(*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
      }
    }
    if(p->checkDown()){
      if((*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) == -3){
	(*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
      }
    }
    if(p->checkFSlice()){
      if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) == -3){
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
      }
    }
    if(p->checkBSlice()){
      if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) == -3){
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) -1;
	this->Sn2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
      }
    }//end checking neighbours
      //erase p from Sn1
    p = this->Sn1->erase(p);
  }//finish Sn1 while iteration


  //Iterate through the Sp1 list
  p = this->Sp1->begin();
  while(p < this->Sp1->end()){
    //set Label(p) = 1 add p to Lp1
    (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = 1;
    this->Lp1->push_back(*p);
    
    //check the neighbours and make sure they are 2 away
    if(p->checkForward()){
      if((*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) == 3){
	(*this->Phi)((*p)[0]+1,(*p)[1],(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]+1,(*p)[1],(*p)[2]));
      }
    }
    if(p->checkBackward()){
      if((*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) == 3){
	(*this->Phi)((*p)[0]-1,(*p)[1],(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0]-1,(*p)[1],(*p)[2]));
      }
    }
    if(p->checkUp()){
      if((*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) == 3){
	(*this->Phi)((*p)[0],(*p)[1]+1,(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]+1,(*p)[2]));
      }
    }
    if(p->checkDown()){
      if((*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) == 3){
	(*this->Phi)((*p)[0],(*p)[1]-1,(*p)[2]) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1]-1,(*p)[2]));
      }
    } 
    if(p->checkFSlice()){
      if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) == 3){
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]+1) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]+1));
      }
    }
    if(p->checkBSlice()){
      if((*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) == 3){
	(*this->Phi)((*p)[0],(*p)[1],(*p)[2]-1) = (*this->Phi)((*p)[0],(*p)[1],(*p)[2]) +1;
	this->Sp2->push_back(SFM_point<T>(p->getRows(),p->getColumns(),p->getSlices(),(*p)[0],(*p)[1],(*p)[2]-1));
      }
    }
    //finish checking neighbors
    //remove p from Sn1
    p = this->Sp1->erase(p);
  
  }//finish Sp1 while iteration

  //Move points into -2 and +2 level sets from Sn2 and Sp2 respectfully
  
  //iterate through the Sn2 list
  p = this->Sn2->begin();
  while(p < this->Sn2->end()){
    //set label(p) = -2, add p to Lz and remove p from Sz
    (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = -2;
    this->Ln2->push_back(*p);
    p = this->Sn2->erase(p);
  }
  

  //iterate throught the Sp2 list
  p = this->Sp2->begin();
  while(p < this->Sp2->end()){
    //set label(p) = 2, add p to Lz and remove p from Sz
    (*this->Label)((*p)[0],(*p)[1],(*p)[2]) = 2;
    this->Lp2->push_back(*p);
    p = this->Sp2->erase(p);
  }
}

/*Run
 *This method is used to Run the evolution of the curve until one hits the stopping condition specified in the Force object
 */
template<class T>
void SFM3D<T>::Run(){
  //Initialize our count and Update until we hit our stopping condition
  this->count = 0;
  while(!this->Force->Stop(*this->Lz, *this->Lp1, *this->Lp2, *this->Ln1,*this->Ln2, count)){    
    this->Update();
    this->count++;
  }
}
   
//Declare all the template types that will be used with this class
//template class SFM3D<int>;
//template class SFM3D<float>;
template class SFM3D<double>;
//template class SFM3D<char>;

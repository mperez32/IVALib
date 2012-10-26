#ifndef _SFM_HPP
#define _SFM_HPP

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Header file containing the SFM class in IVALib 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/
#include<armadillo_bits/config.hpp>
#include<armadillo>
#include"opencv2/core/core.hpp"
#include<opencv2/highgui/highgui.hpp>
#include"IVALib/ActiveSurfaces/functor.hpp"
#include<stdio.h>
#include<iostream>
#include<vector>
#include"IVALib/ActiveSurfaces/SFM_point.hpp"

/**
 *SFM CLASS
 *This class is used to solve the partial differential equation d/dt(phi(x,y,t)) - grad(phi)*F(x,y,t) = 0 over the region C = {(x,y)| phi(x,y)=0}
 *which is refered to as the active contour. We also want to have that |grad(phi(x,y,t)| = 1 as we evolve phi. This class takes in a mask over
 *a region Omega \subset R^2, a functor that specifies how to compute F(x,y,t) and how to evalute a stopping condition. The class then returns
 *5 std::vector<SFM_points> that contian the -2,-1,0,1,2 level sets as well as arma::Mat<T> or cv::Mat_<T> that contains the values of phi(x,y). 
 *
 */
template <class T>
class SFM{
public:
  //This is the set of constructors and destrutors
  /**
   *Default constructor
   */
  SFM<T>(){};
  /**
   *Constructor with arma::Mat input as initialization mask
   *@param Mask the input mask used for initialization
   */
  inline SFM<T>(arma::Mat<T> const &Mask){
  this->Init = new cv::Mat_<T>(Mask.n_rows,Mask.n_cols);
  // convert Mask into cv::Mat_<T>

  this->Lz = new std::vector<SFM_point<T> >();
  this->Ln1 = new std::vector<SFM_point<T> >();
  this->Ln2 = new std::vector<SFM_point<T> >();
  this->Lp1 = new std::vector<SFM_point<T> >();
  this->Lp2 = new std::vector<SFM_point<T> >();
  this->Sz = new std::vector<SFM_point<T> >();
  this->Sn1 = new std::vector<SFM_point<T> >();
  this->Sn2 = new std::vector<SFM_point<T> >();
  this->Sp1 = new std::vector<SFM_point<T> >();
  this->Sp2 = new std::vector<SFM_point<T> >();
 
};
  /**
   *Contructor with arma::Mat as input initialization mask and a pointer to a functor that computes the force
   *@param Mask the input mask used for intialization
   *@param Force pointer to functor used to compute the update force
   */
  inline SFM<T>(arma::Mat<T> const &Mask, functor<T> *Force){
  
  this->Init = new cv::Mat_<T>(Mask.n_rows,Mask.n_cols);
  //convert Mask into cv::Mat_<T>

  //*this->Init = Mask;
  this->Force = Force;
  this->Lz = new std::vector<SFM_point<T> >();
  this->Ln1 = new std::vector<SFM_point<T> >();
  this->Ln2 = new std::vector<SFM_point<T> >();
  this->Lp1 = new std::vector<SFM_point<T> >();
  this->Lp2 = new std::vector<SFM_point<T> >();
  this->Sz = new std::vector<SFM_point<T> >();
  this->Sn1 = new std::vector<SFM_point<T> >();
  this->Sn2 = new std::vector<SFM_point<T> >();
  this->Sp1 = new std::vector<SFM_point<T> >();
  this->Sp2 = new std::vector<SFM_point<T> >();

};
  /**
   *Constructor with cv::Mat as input initialization mask
   *@param Mask the input mask used for intialization
   */
  inline SFM<T>(cv::Mat_<T> const &Mask){

  this->Init = new cv::Mat_<T>(Mask.size());
  *this->Init = Mask;
  this->Lz = new std::vector<SFM_point<T> >();
  this->Ln1 = new std::vector<SFM_point<T> >();
  this->Ln2 = new std::vector<SFM_point<T> >();
  this->Lp1 = new std::vector<SFM_point<T> >();
  this->Lp2 = new std::vector<SFM_point<T> >();
  this->Sz = new std::vector<SFM_point<T> >();
  this->Sn1 = new std::vector<SFM_point<T> >();
  this->Sn2 = new std::vector<SFM_point<T> >();
  this->Sp1 = new std::vector<SFM_point<T> >();
  this->Sp2 = new std::vector<SFM_point<T> >();

};
  /**
   *Constructor with cv::Mat as input initialization mask and a pointer to a functor that computes the force
   *@param Mask the input mask used for intialization
   *@param Force pointer to functor used to compute the update force
   */
  inline SFM<T>(cv::Mat_<T> const &Mask, functor<T> *Force){
  this->Init =  new cv::Mat_<T>(Mask.size());
  *this->Init = Mask;
  this->Force = Force;
  this->Lz = new std::vector<SFM_point<T> >();
  this->Ln1 = new std::vector<SFM_point<T> >();
  this->Ln2 = new std::vector<SFM_point<T> >();
  this->Lp1 = new std::vector<SFM_point<T> >();
  this->Lp2 = new std::vector<SFM_point<T> >();
  this->Sz = new std::vector<SFM_point<T> >();
  this->Sn1 = new std::vector<SFM_point<T> >();
  this->Sn2 = new std::vector<SFM_point<T> >();
  this->Sp1 = new std::vector<SFM_point<T> >();
  this->Sp2 = new std::vector<SFM_point<T> >();
  
};

  /**
   *Method used to obtain a copy of the Lz linked list
   */
  std::vector<SFM_point<T> > getLz(){ return *(this->Lz);};
  /**
   *Method used to obtain a copy of Phi as an arma::Mat<T> file
   */
  cv::Mat_<T> getPhi(){ return *(this->Phi);};
  /**
   *Method used to obtain a copy of Label as an arma::Mat<T> file
   */
  cv::Mat_<int> getLabel(){return *(this->Label);};
  /**
   *Method used to initialize Phi, Label, and L linked lists
   */
  void Initialize();
  /**
   *This method is used to compute the update of Phi by adding F to Phi(Lz)
   *This method requires to have initialized a functor that describes how to compute F 
   */
  void Update();
  /**
   *Method used to run the update until the stopping condition described in Force
   */
  void Run();
  /**
   *Virtual Method used to compute Phi(Lz) = Phi(Lz) + F(Lz) (Only override if you know what you are doing)
   *Computes the update Phi(p) = Force(p) + Phi(p) for each point p in Lz by using the Compute() method of the object Force. 
   *The Force object is a functor. The functor class has Compute  as a absolute virtual method. You must override this method 
   *and specify in that method how Phi should be updated. There are more instructions in the functor class.SFM also calls on Normalize() 
   *so your Compute() method should not normalize Phi. 
   *Finally the pointer to Phi will be released since we don't want Force to change Phi outside of the SFM class.
   */
  virtual void Compute();
  /**
   *Method used to set the Init mask
   */
  void setInit(cv::Mat_<T> const &Mask){
  if(this->Init == NULL)
    this->Init = new cv::Mat_<T>();
  *this->Init = Mask;
};
  /**
   *Method used to set the Force
   */
  void setForce(functor<T> *Force){this->Force = Force;};
  /**
   *Method used to print out the size of each list
   */
  void showSizes(){
    std::cout<<"The size of Sz is "<<(int) this->Sz->size()<<std::endl;
    std::cout<<"The size of Sp1 is "<<(int) this->Sp1->size()<<std::endl;
    std::cout<<"The size of Sp2 is "<<(int) this->Sp2->size()<<std::endl;
    std::cout<<"The size of Sn1 is "<<(int) this->Sn1->size()<<std::endl;
    std::cout<<"The size of Sn2 is "<<(int) this->Sn2->size()<<std::endl<<std::endl;
    std::cout<<"The size of Lz is "<<(int) this->Lz->size()<<std::endl;
    std::cout<<"The size of Lp1 is "<<(int) this->Lp1->size()<<std::endl;
    std::cout<<"The size of Lp2 is "<<(int) this->Lp2->size()<<std::endl;
    std::cout<<"The size of Ln1 is "<<(int) this->Ln1->size()<<std::endl;
    std::cout<<"The size of Ln2 is "<<(int) this->Ln2->size()<<std::endl<<std::endl;
  }

private:
  /**
   *Sign distance fuction
   */
  cv::Mat_<T> *Phi;
  /**
   *Initial condition mask
   */
  cv::Mat_<T> *Init;
  /**
   *Label of each point
   */
  cv::Mat_<int> *Label;
  /**
   *This is a functor class used to compute the force at each iteration
   *@see functor<T>
   */
  functor<T> *Force;
  /**
   *Linked list containing the zero level set
   */ 
  std::vector<SFM_point<T> > *Lz;
  /**
   *Linked list containing the 2 level set
   */
  std::vector<SFM_point<T> > *Lp1;
  /**
   *Linked list containing the 2 level set
   */
  std::vector<SFM_point<T> > *Lp2;
  /**
   *Linked list containing the -1 level set;
   */
  std::vector<SFM_point<T> > *Ln1;
  /**
   *Linked list containing the -2 level set;
   */
  std::vector<SFM_point<T> > *Ln2;
  /**
   *linked list containing the points moving to Lz
   */
  std::vector<SFM_point<T> > *Sz;
  /**
   *Linked list containing the points moving to Lp1
   */
  std::vector<SFM_point<T> > *Sp1;
  /**
   *Linked list containing the points moving to Lp2
   */
  std::vector<SFM_point<T> > *Sp2;
  /**
   *Linked list containing the points moving to Ln1
   */
  std::vector<SFM_point<T> > *Sn1;
  /**
   *Linked list containing the points moving to Ln2
   */
  std::vector<SFM_point<T> > *Sn2;
  /**
   *Checks to see if test has no neighbours q such that label(q) == x. If test has such neighbours, this function returns false.
   *@param test Point top be tested
   *@param x int to compare to
   */
  inline bool CheckNeighboursLabel(SFM_point<T> const & test, int const & x){
  
    if(test.checkForward()){
      if((*this->Label)(test[0]+1,test[1]) == x)
	return false;}
    if(test.checkBackward()){
      if((*this->Label)(test[0]-1,test[1]) == x)
	return false;}
    if(test.checkUp()){
      if((*this->Label)(test[0],test[1]+1) == x)
	return false;}
    if(test.checkDown()){
      if((*this->Label)(test[0],test[1]-1) == x)
	return false;}
    else  
      return true;
  };  
  /**
   *Find the Max of Phi(p) for p in neighbours of point that also satisfices label(point) >= x
   *@param point point to check neighbours of
   *@param x the int used to compare the label(point) to
   */
  inline T MaxNeighboursPhi(SFM_point<T> point, int x){

    T Max = -3;
    if(point.checkForward()){
      if((*this->Label)(point[0]+1,point[1]) >= x && (*this->Phi)(point[0]+1,point[1]) > Max) 
	Max = (*this->Phi)(point[0]+1,point[1]);}
    if(point.checkBackward()){
      if((*this->Label)(point[0]-1,point[1]) >= x && (*this->Phi)(point[0]-1,point[1]) > Max) 
	Max = (*this->Phi)(point[0]-1,point[1]);}
    if(point.checkUp()){
      if((*this->Label)(point[0],point[1]+1) >= x && (*this->Phi)(point[0],point[1]+1) > Max) 
	Max = (*this->Phi)(point[0],point[1]+1);}
    if(point.checkDown()){
      if((*this->Label)(point[0],point[1]-1) >= x && (*this->Phi)(point[0],point[1]-1) > Max) 
	Max = (*this->Phi)(point[0],point[1]-1);}
    return Max;   
};
  /**
   *Find the Min of Phi(p) for p in neighbours of point that also satisfices label(point) <= x
   *@param point point to check neighbours of
   *@param x the int used to compare the label(point) to
   */
  inline T minNeighboursPhi(SFM_point<T> point, int x){

    T min = 3;
    if(point.checkForward()){
      if((*this->Label)(point[0]+1,point[1]) <= x && (*this->Phi)(point[0]+1,point[1]) < min) 
	min = (*this->Phi)(point[0]+1,point[1]);}
    if(point.checkBackward()){
      if((*this->Label)(point[0]-1,point[1]) <= x && (*this->Phi)(point[0]-1,point[1]) < min) 
	min = (*this->Phi)(point[0]-1,point[1]);}
    if(point.checkUp()){
      if((*this->Label)(point[0],point[1]+1) <= x && (*this->Phi)(point[0],point[1]+1) < min) 
	min = (*this->Phi)(point[0],point[1]+1);}
    if(point.checkDown()){
      if((*this->Label)(point[0],point[1]-1) <= x && (*this->Phi)(point[0],point[1]-1) < min) 
	min = (*this->Phi)(point[0],point[1]-1);}
    return min;
  };
  int count;
};


#endif

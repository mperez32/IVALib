#ifndef _FUNCTOR_HPP
#define _FUNCTOR_HPP

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Header file containing the functor class in IVALib 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


#include<armadillo_bits/config.hpp>
#include<armadillo>
#include"opencv2/core/core.hpp"
#include"IVALib/ActiveSurfaces/SFM_point.hpp"


/**
 *FUNCTOR CLASS
 *This class contains is an abstract class used to compute the force F in the SFM class. One should override the Compute method which will
 *pass all the relevant information such as the const refrence to the
 *point list containing the -2,-1,0,1, and 2 level set. The class also contains the member Phi which is a pointer to the current Phi 
 *arma::Mat<T>. The second method one should override is the method Stop used to compute the stopping conditions.
 */
template<class T>
class functor {

public:
  /**
   *Constructor
   */
  functor<T>(){
    this->F = new std::vector<SFM_point<T> >();
    threeD = false;
  }
  /**
   *Destrucor
   */
  ~functor<T>(){
  delete F;
  }
  /**
   *Set the phi pointer
   *@param Phi the pointer to phi
   */
  void setPhi(cv::Mat_<T> *Phi){
    this->Phi = Phi;
  }
  /**
   *Set the phi pointer
   *@param Phi the pointer to phi
   */
  void setPhi3D(arma::Cube<T> *Phi){
    this->Phi3D = Phi;
  }

  /**
   *Set 3D to true
   */
  inline void set3D(){
    if(!threeD)
      threeD = true;
  }
  /**
   *Computes the stoping condition and returns true if it should stop and false otherwise. This is a pure virtual member.
   *@param Phi Signed Distance Function representing the contour
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   *@count int value used to keep count of number of iterations
   */
  virtual bool Stop(std::vector<SFM_point<T> > const &Lz, std::vector<SFM_point<T> > const &Lp1, std::vector<SFM_point<T> > const &Lp2, std::vector<SFM_point<T> > const &Ln1,std::vector<SFM_point<T> > const &Ln2, int count)=0;
  /**
   *Computes F, this is a pure virtual method and must be overriden.
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   */
  virtual void Compute(std::vector<SFM_point<T> > const &Lz, std::vector<SFM_point<T> > const &Lp1, std::vector<SFM_point<T> > const &Lp2, std::vector<SFM_point<T> > const &Ln1,std::vector<SFM_point<T> > const &Ln2)=0;
  /**
   *Normalizes F so that each value in F is at most 0.5
   */ 
  void Normalize(){
    T max = 0;
    //find the max of abs(Phi(p)) for p in Lz
    for(typename std::vector<SFM_point<T> >::iterator i = this->F->begin(); i < this->F->end();i++){
      if(std::abs(i->getValue()) >max){
	max = std::abs(i->getValue());
      }
    }
    
    //normalize Force so that the max(Force(Lz)) = 0.5;
    if(max > 0.5)
      for(typename std::vector<SFM_point<T> >::iterator i = this->F->begin(); i < this->F->end();i++)
	i->setValue(0.5/max * i->getValue());
	
  };
  /**
   *Updates the Phi
   *Compute Phi(p) = Phi(p) + Force(p) for each p in F
   */
  void Update(){ 
    //Compute Phi(p) = Phi(p) + Force(p) for each p in F
    if(threeD){
      for(typename std::vector<SFM_point<T> >::iterator i = this->F->begin(); i<this->F->end();i++){ 
	(*this->Phi3D)((*i)[0],(*i)[1],(*i)[2]) = (*this->Phi3D)((*i)[0],(*i)[1],(*i)[2]) + i->getValue(); 
	i = this->F->erase(i);
      }
    }else{
      for(typename std::vector<SFM_point<T> >::iterator i = this->F->begin(); i<this->F->end();i++){ 
	(*this->Phi)((*i)[0],(*i)[1]) = (*this->Phi)((*i)[0],(*i)[1]) + i->getValue(); 
	i = this->F->erase(i);
      }
    }
    
  };
  /**
   *Releases the Phi pointer after all computations are done
   */
  void Release(){
    if(threeD)
      this->Phi3D = NULL;
    else
      this->Phi = NULL;
  };
protected:
  /**
   *Member pointing to a point list where the force values will be set
   */
  std::vector<SFM_point<T> > *F;
  /*
   *Member pointing to the updated Phi
   */
  cv::Mat_<T> *Phi;
  /**
   *Member pointing to the update Phi in 3D
   */
  arma::Cube<T> *Phi3D;
  /**
   *Member boolean that represents if used in 3D
   */
  bool threeD;
  /**
   *Set the F value at a given point
   *@param point the point you are setting;
   *@param value the value you are setting at that point
   */
  void setPointF(SFM_point<T> point, T value){
    point.setValue(value);
    this->F->push_back(point);
  };
};



/**Template specification
 *This is a template specification for the functor class for the char type since there is no max for char.
 *@see functor<>
 */
template<>
class functor <char>{

public:
  //Constructor
  functor(){
    this->F = new std::vector<SFM_point<char> >();
    threeD =false;
  }

  //Set the phi pointer
  void setPhi(cv::Mat_<char> *Phi){
    this->Phi = Phi;
  }
  /**
   *Set the phi pointer
   *@param Phi the pointer to phi
   */
  void setPhi3D(arma::Cube<char> *Phi){
    this->Phi3D = Phi;
  }

  /**
   *Set 3D to true
   */
  inline void set3D(){
    if(!threeD)
      threeD = true;
  }

  //Computes the stoping condition and returns true if it should stop and false otherwise 
  virtual bool Stop( std::vector<SFM_point<char> > const &Lz, std::vector<SFM_point<char> > const &Lp1, std::vector<SFM_point<char> > const &Lp2, std::vector<SFM_point<char> > const &Ln1,std::vector<SFM_point<char> > const &Ln2, int count)=0;

  
  //Computes F, this is an abstract method and must be overriden
  virtual void Compute(std::vector<SFM_point<char> > const &Lz, std::vector<SFM_point<char> > const &Lp1, std::vector<SFM_point<char> > const &Lp2, std::vector<SFM_point<char> > const &Ln1,std::vector<SFM_point<char> > const &Ln2)=0;

  //Normalizes Phi 
  void Normalize(){
    char max = 0;
    //find the max of abs(Phi(p)) for p in Lz
    if(threeD){
      for(typename std::vector<SFM_point<char> >::iterator i = this->F->begin(); i < this->F->end();i++){
	if( (*this->Phi)((*i)[0],(*i)[1],(*i)[2]) >max)
	  max = (*this->Phi)((*i)[0],(*i)[1],(*i)[2]);
      }
    }else{
      for(typename std::vector<SFM_point<char> >::iterator i = this->F->begin(); i < this->F->end();i++){
	if( (*this->Phi)((*i)[0],(*i)[1]) >max)
	  max = (*this->Phi)((*i)[0],(*i)[1]);
      }
    }
    //normalize Force so that the max(Force(Lz)) = 0.5; 
    for(typename std::vector<SFM_point<char> >::iterator i = this->F->begin(); i < this->F->end();i++){
      i->setValue(0.5/max * i->getValue());
    }
   
};

  //Updates the Phi
  void Update(){ 
  //Compute Phi(p) = Phi(p) + Force(p) for each p in F
  for(typename std::vector<SFM_point<char> >::iterator i = this->F->begin(); i<this->F->end();i++) 
    (*this->Phi)((*i)[0],(*i)[1]) = (*this->Phi)((*i)[0],(*i)[1]) + i->getValue(); 
  };

  
  //Releases the Phi pointer
  void Release(){
    this->Phi = NULL;
    this->F->clear();
  };

protected:
  std::vector<SFM_point<char> > *F;//Member pointing to a point list where the force values will be set
  cv::Mat_<char> *Phi;//Member pointing to the updated Phi
  arma::Cube<char> *Phi3D;
  bool threeD;
  //Set the F value at a given point
  void setPointF(SFM_point<char> point, char value){
    point.setValue(value);
    this->F->push_back(point);
  };


   
};
#endif





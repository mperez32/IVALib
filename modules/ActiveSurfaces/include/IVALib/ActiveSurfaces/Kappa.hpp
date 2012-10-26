#ifndef _KAPPA_H_
#define _KAPPA_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Header file containing the SFM class in IVALib 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/

#include "IVALib/ActiveSurfaces/functor.hpp"



/**
 *Kappa 
 *This class inherits the abstract class functor. It is used to specify how to compute the force that will be used to evolve the 
 *level set. 
 */
class Kappa: public functor<double>{
  
public:
  /**
   *Default constructor
   */
  Kappa():functor<double>(){};
  /**
   *Stop method used to compute when to stop the run methor in SMF class
   *@see SFM<template class>::Run()
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   *@count int value used to keep count of number of iterations
   */
  bool Stop( std::vector<SFM_point<double> > const &Lz, std::vector<SFM_point<double> > const &Lp1, std::vector<SFM_point<double> > const &Lp2, std::vector<SFM_point<double> > const &Ln1,std::vector<SFM_point<double> > const &Ln2, int count){
    if(count<6)return false;
    else return true;
  };
  /**
   *Computes the curvature at each point
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   */
  void Compute(std::vector<SFM_point<double> > const &Lz, std::vector<SFM_point<double> > const &Lp1, std::vector<SFM_point<double> > const &Lp2, std::vector<SFM_point<double> > const &Ln1,std::vector<SFM_point<double> > const &Ln2);
};

#endif

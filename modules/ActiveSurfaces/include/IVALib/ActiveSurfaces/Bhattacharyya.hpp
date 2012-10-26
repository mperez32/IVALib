#ifndef _BHATTACHARYYA_H_
#define _BHATTACHARYYA_H_
#include "IVALib/ActiveSurfaces/functor.hpp"
#include<stdio.h>
#include<iostream>
#include"IVALib/Plotter/PlotterWindow.hpp"
#include <stdexcept> 

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Header file containing the Bhattacharyya class in IVALib 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/


/**
 *Bhattacharyya
 *This class inherits the abstract class functor. It is used to specify how to compute the force that will be used to evolve the 
 *level set. This class uses the Bhattacharyya energy functional which tries to minimize the distance  
 */
class Bhattacharyya: public functor<double>{
  
public:
  /**
   *A constructor
   *Default constructor
   */
  Bhattacharyya():functor<double>(){
    this->P_in = new arma::vec();
    this->P_out = new arma::vec();
    this->Img = new cv::Mat_<uchar>();
    this->size = new cv::Size;
    plot = NULL;
    bins = 256;
  };
  /**
   *A constructor
   *Constructor that takes in a cv:Mat as an image.
   *@param Img Image that is used to evolve the active contour
   *@param bins The number of bins in the histogram, by default 256
   *@param min The minnimum number in the range used to compute the histogram by default 0
   *@param max The maximum value in the range used to compute the histogram, by defaul 255
   */
  Bhattacharyya(cv::Mat const &Img, int ibins = 256,double imin = 0, double imax = 255):functor<double>(){
    cv::Mat dst;
    std::cout<<"this is imin :"<<imin<<std::endl;
    this->bins = ibins;
    this->min = imin;
    this->max = imax;
    this->step = (imax+1-imin)/ibins;
    this->channels = Img.channels();
    this->normal = new arma::vec(bins);
    this->H_in = new cv::Mat();
    this->H_out = new cv::Mat();
    this->computeNormal();
    //std::cout<<"The number of channels are : "<<this->channels<<std::endl;
    
    if(this->channels == 1){
      this->P_in = new arma::vec(bins);
      this->P_out = new arma::vec(bins);
      this->V_in = new arma::vec(bins);
      this->V_out = new arma::vec(bins);
      this->Img = new cv::Mat;
      *this->Img = Img;
    }
    else if(this->channels == 2){
      this->P_in = new arma::vec[2];
      this->P_out = new arma::vec[2];
      this->V_in = new arma::vec[2];
      this->V_out = new arma::vec[2];
      this->Img = new cv::Mat;
      *this->Img = Img;
    }
    else if(channels == 3){
      this->P_in = new arma::vec[3];
      this->P_out = new arma::vec[3];
      this->V_in = new arma::vec[3];
      this->V_out = new arma::vec[3];
      this->Img = new cv::Mat;
      *this->Img = Img;
    }
    else{
      throw std::runtime_error("sorry but the Bhattacharyya doesn't accept images with more then 3 channels");
      exit(0);}
    this->size = new cv::Size(Img.size());
    plot = NULL;
  };
  /**
   *Destructor
   */
  ~Bhattacharyya(){
    delete normal;
    delete H_in;
    delete H_out;
    if(channels == 1){
      delete P_in;
      delete P_out;
      delete V_in;
      delete V_out;
      delete Img;
      if(plot != NULL)
	delete plot;
    }
    else{
      delete[] P_in;
      delete[] P_out;
      delete[] V_in;
      delete[] V_out;
      delete Img;
      if(plot != NULL)
	delete[] plot;
    }
  }
  /**
   *Stop method inherited from functor class. This method returns true when the stopping condition is reached. This method is used by 
   *the SFM class as a stopping condition when running Run method.
   *@param Phi Signed Distance Function representing the contour
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   *@count int value used to keep count of number of iterations
   */
  bool Stop(std::vector<SFM_point<double> > const &Lz, std::vector<SFM_point<double> > const &Lp1, std::vector<SFM_point<double> > const &Lp2, std::vector<SFM_point<double> > const &Ln1,std::vector<SFM_point<double> > const &Ln2, int count){
    if(count<6)return false;
    else return true;
  };
  /**
   *Compute method inherited from functor class. This method is used to compute the gradient of the Bhattacharyya distance with 
   *respect to Phi. 
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   */
  void Compute(std::vector<SFM_point<double> > const &Lz, std::vector<SFM_point<double> > const &Lp1, std::vector<SFM_point<double> > const &Lp2, std::vector<SFM_point<double> > const &Ln1,std::vector<SFM_point<double> > const &Ln2);
  /**
   *Computes the normal distribution
   */
  inline void computeNormal(){
    *normal = arma::linspace(-127,128,256);
    *normal = exp(-(square(*normal)/5.5));
    *normal = *normal/arma::sum(normal[0]);
  }
  /**
   *Plot method plots the distribution of the pixels inside the contour and outside the contours
   */
  void Plot(){
    if(channels == 1){
      if(plot == NULL)
	plot = new PlotterWindow;
      plot[0].add_Curve(P_in[0],"red",PlotMM::CURVE_LINES,PlotMM::SYMBOL_NONE);
      plot[0].add_Curve(P_out[0]);
      plot[0].show();}
      else if(channels == 3){
	if(plot == NULL)
	  plot = new PlotterWindow[3];
	plot[0].add_Curve(P_in[0],"red",PlotMM::CURVE_LINES,PlotMM::SYMBOL_NONE);
	plot[0].add_Curve(P_out[0]);
	plot[0].set_Title("Green");
	plot[0].show();
	plot[1].add_Curve(P_in[1],"red",PlotMM::CURVE_LINES,PlotMM::SYMBOL_NONE);
	plot[1].add_Curve(P_out[1]);
	plot[1].set_Title("Blue");
	plot[1].show();
	plot[2].add_Curve(P_in[2],"red",PlotMM::CURVE_LINES,PlotMM::SYMBOL_NONE);
	plot[2].add_Curve(P_out[2]);
	plot[2].set_Title("Red");
	plot[2].show();
      }
  }
  /**
   *Method to obtain B constant at each iteration.
   */
  inline double &get_B(){ return B;};
private:
  /**
   *A pointer to the cv::Mat image
   */
  cv::Mat *Img;
  /**
   *Pointer of the distributions inside the contour
   */
  arma::vec *P_in;
  /**
   *Pointer of the distribution outside the contour
   */
  arma::vec *P_out;
  /**
   *Normal distribution used as a kernel for the KDE of P_in and P_out
   */
  arma::vec *normal;
  /**
   *Pointer to the ratio of P_out over P_in
   */
  arma::vec *V_in;
  /**
   *Pointer to the ratio of P_in over P_out
   */
  arma::vec *V_out;
  /**
   *Pointer to the mask that represents the pixels inside the contour
   */
  cv::Mat *H_in;
  /**
   *Pointer to the mask that represents the pixels outside the contour
   */
  cv::Mat *H_out;
  /**
   *Pointer to the size of the Img
   */
  cv::Size *size;
  /**
   *Int representing the number of channels the image has
   */
  int channels;
  /**
   *Function used to compute the Histogram
   */
  template<class T>
  inline void Hist(){
    const char* t;
    if(this->channels == 1){
      this->P_out->zeros(bins);
      this->P_in->zeros(bins);    
      int ent;
      T *p;
      for(int i=0;i <this->Img->rows;i++){
	p = this->Img->ptr<T>(i);
	t = H_out->ptr<char>(i);
	for(int j = 0;j <this->Img->cols;j++){
	  ent = (int) (p[j]-min)/step;
	  if(t[j] != 0)	  
	    this->P_out[0].at(ent)++;
	  else
	    this->P_in[0].at(ent)++;
	}
      }
    }else if(this->channels == 2){
      this->P_out[0].zeros(bins);
      this->P_out[1].zeros(bins);
      this->P_in[0].zeros(bins);
      this->P_in[1].zeros(bins);
      cv::Vec<T, 2>* p;
      int ent[2];
      for(int i = 0;i<this->Img->rows;i++){
	p = this->Img->ptr<cv::Vec<T,2> >(i); 
	t = H_out->ptr<char>(i);      
	for(int j=0;j<this->Img->cols;j++){
	  ent[0] = (int) (p[j][0]-min)/step;
	  ent[1] = (int) (p[j][1]-min)/step;
	  if(t[j] != 0){
	    this->P_out[0].at(ent[0])++;
	    this->P_out[1].at(ent[1])++;	  
	  }else{
	    this->P_in[0].at(ent[0])++;
	    this->P_in[1].at(ent[1])++;	  
	  }
	}
      }     
    }else if(this->channels == 3){
      this->P_out[0].zeros(bins);
      this->P_out[1].zeros(bins);
      this->P_out[2].zeros(bins);
      this->P_in[0].zeros(bins);
      this->P_in[1].zeros(bins);
      this->P_in[2].zeros(bins);
      cv::Vec<T, 3>* p;
      int ent[3];
      for(int i = 0;i<this->Img->rows;i++){
	p = this->Img->ptr<cv::Vec<T, 3> >(i);
	t = H_out->ptr<char>(i);
	for(int j = 0;j<this->Img->cols;j++){
	  ent[0] = (int) (p[j][0]-min)/step;
	  ent[1] = (int) (p[j][1]-min)/step;
	  ent[2] = (int) (p[j][2]-min)/step;
	  if(ent[0] <0 || ent[0] >=bins|| ent[0] <0 || ent[0] >=bins || ent[0] <0 || ent[0] >=bins ){
	    std::stringstream error;
	    error <<"Error in Hist function. The value of the pixel is out of range. The value of the pixel is ["<<ent[0]<<","<<ent[1]<<","<<ent[2]<<"]"<<std::endl;
	    throw std::runtime_error(error.str());
	    exit(0);
	  }
	  if(t[j] != 0){
	    this->P_out[0].at(ent[0])++;
	    this->P_out[1].at(ent[1])++;
	    this->P_out[2].at(ent[2])++;
	  }else{
	    this->P_in[0].at(ent[0])++;
	    this->P_in[1].at(ent[1])++;
	    this->P_in[2].at(ent[2])++;
	  }
	}
      }
    } 
  };
  /**
   *Pointer to PlotterWindows used to plot P_in and P_out
   */
  PlotterWindow *plot;
  /**
   *Double used to store the value of the B constant in the Bhattacharyya distance
   */
  double B;
  /**
   *Int representing how many bins the histogram has
   */
  int bins;
  /**
   *Double representing the minnimum of the range to perform the histogram on
   */
  double min;
  /**
   *Double representing the maximum of the range to perform the histogram on
   */
  double max;
  /**
   *Double representing the step in each histogram
   */
  double step;
  
};

#endif

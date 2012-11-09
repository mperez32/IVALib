#ifndef CHANVESE_H_
#define CHANVESE_H_

#include "IVALib/ActiveSurfaces/functor.hpp"
#include<stdio.h>
#include<iostream>



/**
 *ChanVese 
 *This class inherits the abstract class functor. It is used to specify how to compute the force that will be used to evolve the 
 *level set. This class uses the Chan Vese energy functional. 
 */
class ChanVese: public functor<double>{
  
public:
  /**
   *Default Constructor
   */
  ChanVese():functor<double>(){
    this->Img = new cv::Mat;
    this->size = new cv::Size;
    H_in =  cv::Mat(*this->size, CV_8U);
    H_out = cv::Mat(*this->size, CV_8U);

  };
  /**
   *Contructor
   *This constructor uses an input image in format of cv::Mat
   *@param Img cvMat used to evolve the contour on
   */
  ChanVese(cv::Mat const &Img):functor<double>(){
    this->channels = Img.channels();
    std::cout<<"The number of channels are : "<<this->channels<<std::endl;
    this->Img = new cv::Mat;
    *this->Img = Img;
    this->size = new cv::Size(Img.size());
    H_in =  cv::Mat(*this->size, CV_8U);
    H_out = cv::Mat(*this->size, CV_8U);

  }
  /**
   *Destructor
   */
  ~ChanVese(){
    delete Img;
    delete size;
  }
  /**
   *Stop is a method used as a stopping condition. Returns true if the estimated energy is reached and false otherwise
   *@param Phi Signed Distance Function representing the contour
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
   *Computes the flow F at each step.
   *@Lz vector of points used to represent the contour
   *@Lp1 vector of points used to represent the +1 list
   *@Lp2 vector of points used to represent the +2 list
   *@Ln1 vector of points used to represent the -1 list
   *@Ln2 vector of points used to represent the -2 list
   */
  void Compute(std::vector<SFM_point<double> > const &Lz, std::vector<SFM_point<double> > const &Lp1, std::vector<SFM_point<double> > const &Lp2, std::vector<SFM_point<double> > const &Ln1,std::vector<SFM_point<double> > const &Ln2);

private:
  /**
   *pointer to the image
   */
  cv::Mat *Img;
  /**
   *pointer to the size of the image
   */
  cv::Size *size;
  /**
   *number of channels
   */
  int channels;
  /**
   *Double representing the size inside the contour
   */
  double size_in;
  /**
   *Double representing the size outside the contour
   */
  double size_out;
  /**
   *cv::Mat that has value 1 inside the contour and 0 outside
   */
  cv::Mat H_in;
  /**
   *cv::Mat that has value 1 outside the contour and 0 inside
   */
  cv::Mat H_out;
  /**
   *cv::Vec3d that represents of the features inside the contour
   */
  cv::Vec3d means_in;
  /**
   *cv::Vec3d that represents of the features outside the contour
   */
  cv::Vec3d means_out;
  

  template<class T>
  inline void computeEner(std::vector<SFM_point<double> > const &Lz){

    using namespace std;
    typedef vector<SFM_point<double> > plist;

    //Compute the means of the features in and out of the contour
    H_in = ((*this->Phi) <= 0)/255;
    H_out = ((*this->Phi) > 0)/255;
    size_in = countNonZero(H_in);
    size_out = countNonZero(H_out);
    means_in = cv::Vec3d(0, 0, 0);
    means_out = cv::Vec3d(0, 0, 0);
    
    
    if(channels == 1){
      for(int i = 0; i< Img->rows;++i){
	T *Img_p = Img->ptr<T>(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0;j< Img->cols;j++){
	  if(H_in_p[j] == 1)
	    means_in[0] = means_in[0] + Img_p[j];
	  else
	    means_out[0] = means_out[0] + Img_p[j];
	}	 
      }
    }else if(channels == 2){
      for(int i = 0; i < (this->Img->rows); ++i){
	cv::Vec<T,2> *Img_p = this->Img->ptr<cv::Vec<T,2> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< this->Img->cols;j++){
	  if(H_in_p[j] == 1){
	    means_in[0] = means_in[0] + Img_p[j][0];
	    means_in[1] = means_in[1] + Img_p[j][1];
	  }else{
	    means_out[0] = means_out[0] + Img_p[j][0];
	    means_out[1] = means_out[1] + Img_p[j][1];
	  }
	}
      }         
   }else if(channels == 3){
      for(int i = 0; i < (this->Img->rows); ++i){
	cv::Vec<T, 3> *Img_p = this->Img->ptr<cv::Vec<T, 3> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< this->Img->cols;j++){
	  if(H_in_p[j] == 1){
	    means_in[0] = means_in[0] + Img_p[j][0];
	    means_in[1] = means_in[1] + Img_p[j][1];
	    means_in[2] = means_in[2] + Img_p[j][2];
	  }else{
	    means_out[0] = means_out[0] + Img_p[j][0];
	    means_out[1] = means_out[1] + Img_p[j][1];
	    means_out[2] = means_out[2] + Img_p[j][2];
	  }
	}
      }    
    }
    means_in = means_in/size_in;
    means_out = means_out/size_out;
    
    //Declare the variables that will be used
    double dx, dy, dxx, dyy, dx2, dy2, dxy, kappa, ener_chanve;
    bool Xok, Yok;
    
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
      
      //Compute curvuture at each point
      kappa = (dxx*dy2 + dyy*dx2 -2*dx*dy*dxy)/(dx2 + dy2+ 0.0000001);
      
      //Compute chanvese energy at each point
      if(this->channels == 1)
	ener_chanve = kappa + (pow(Img->ptr<T>((*i)[0])[(*i)[1]]-means_in[0],2) - pow(Img->ptr<T>((*i)[0])[(*i)[1]]-means_out[0],2));
      else if(this->channels == 2){
	ener_chanve = 0.8*kappa + 1.0/2*(pow((this->Img->at<cv::Vec<T,2> >((*i)[0],(*i)[1])[0]-means_in[0]),2) + pow((this->Img->at<cv::Vec<T,2> >((*i)[0],(*i)[1])[1]-means_in[1]),2) + pow((this->Img->at<cv::Vec<T,2> >((*i)[0],(*i)[1])[2]-means_in[2]),2) - (pow((this->Img->at<cv::Vec<T, 3> >((*i)[0],(*i)[1])[0]-means_out[0]),2) + pow((this->Img->at<cv::Vec<T,2> >((*i)[0],(*i)[1])[1]-means_out[1]),2) + pow((this->Img->at<cv::Vec<T,2> >((*i)[0],(*i)[1])[2]-means_out[2]),2)));      
      }else if(this->channels == 3){
	ener_chanve = 0.8*kappa + 1.0/3*(pow((this->Img->at<cv::Vec<T,3> >((*i)[0],(*i)[1])[0]-means_in[0]),2) + pow((this->Img->at<cv::Vec<T,3> >((*i)[0],(*i)[1])[1]-means_in[1]),2) + pow((this->Img->at<cv::Vec<T,3> >((*i)[0],(*i)[1])[2]-means_in[2]),2) - (pow((this->Img->at<cv::Vec<T, 3> >((*i)[0],(*i)[1])[0]-means_out[0]),2) + pow((this->Img->at<cv::Vec<T,3> >((*i)[0],(*i)[1])[1]-means_out[1]),2) + pow((this->Img->at<cv::Vec<T,3> >((*i)[0],(*i)[1])[2]-means_out[2]),2)));
      }

      this->setPointF(*i,ener_chanve);
          
    }//end for loop that iterates through LZ 


  };
  
};
#endif

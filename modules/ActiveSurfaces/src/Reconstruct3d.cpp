/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * cpp file containing most methods for the Reconstruct3d class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 ******************************************************************************/
#include "IVALib/ActiveSurfaces/Reconstruct3d.hpp"
using namespace cv;
using namespace std;



 void Reconstruct3d::Compute(vector<SFM_point<double> > const &Lz, vector<SFM_point<double> > const &Lp1, vector<SFM_point<double> > const &Lp2, vector<SFM_point<double> > const &Ln1,vector<SFM_point<double> > const &Ln2){
   
     //Update the label map and Lz of each camera
     double *labelPtr = label->memptr();
     double *PhiPtr = Phi3D->memptr();
     for(int i = 0;i<label->n_elem;i++){
	 if(PhiPtr[i] <= 0)
	     labelPtr[i] = 1;
	 else 
	     labelPtr[i] = 0;
     }
     *this->Lz = Lz;
     this->energy.zeros();

     //compute the energy at each point
     for(clist::iterator cam = Cameras.begin(); cam<Cameras.end(); cam++){
	 cam->computeOcc();
	 if(cam->getImg().depth() == CV_8U)
	     this->computeEnergy<uchar>(*cam);
	 else if(cam->getImg().depth() == CV_8S)
	     this->computeEnergy<uchar>(*cam);
	 else if(cam->getImg().depth() == CV_16U)
	     this->computeEnergy<ushort>(*cam);
	 else if(cam->getImg().depth() == CV_16S)
	     this->computeEnergy<short>(*cam);
	 else if(cam->getImg().depth() == CV_32S)
	     this->computeEnergy<int>(*cam);
	 else if(cam->getImg().depth() == CV_32F)
	     this->computeEnergy<float>(*cam);
	 else if(cam->getImg().depth() == CV_64F)
	     this->computeEnergy<double>(*cam);
	 else{
	     throw runtime_error("Your image has an invalid depth.");
	     exit(0);}
     }
     
  
    //Declare the variables that will be used
    double dx, dy, dz, dzz, dxx, dyy, dx2, dy2, dz2, dxz, dyz, dxy, R_ener;
    bool Xok, Yok, Zok;
    for(plist::const_iterator i = Lz.begin();i < Lz.end();i++){
      
      dx = dy = dz = dzz = dxx = dyy = dxy = dxz = dyz = 0;
      Xok = Yok = Zok = false;
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
      
      //Compute curvuture at each point and sum it to the Energy cube and set it to F
      R_ener =  this->energy((*i)[0],(*i)[1],(*i)[2]) + (dxx*(dy2+dz2)+dyy*(dx2+dz2)+dzz*(dx2+dy2)-2*dx*dy*dxy-2*dx*dz*dxz-2*dy*dz*dyz)/(dx2+dy2+dz2+.00000001);

      this->setPointF(*i,R_ener);
      
    }//end for loop that iteratos through LZ 


 }; 

template<class T>
inline void Reconstruct3d::computeEnergy(Camera &cam){
    
    typedef vector<SFM_point<double> > plist;
    double size_in,size_out;
    Mat Img = cam.getImg();
    //Compute the means of the features in and out of the contour of each projected image
    Mat H_in = cam.getSurfImg();
    Mat H_out = (cam.getSurfImg() - 1)*(-1);
    size_in = countNonZero(H_in);
    size_out = countNonZero(H_out);
    Vec3d means_in = Vec3d(0, 0, 0);
    Vec3d means_out = Vec3d(0, 0, 0);

    
    if(Img.channels() == 1){
      for(int i = 0; i< Img.rows;++i){
	T *Img_p = Img.ptr<T>(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0;j< Img.cols;j++){
	  if(H_in_p[j] == 1)
	    means_in[0] = means_in[0] + Img_p[j];
	  else
	    means_out[0] = means_out[0] + Img_p[j];
	}	 
      }
    }else if(Img.channels() == 2){
      for(int i = 0; i < Img.rows; ++i){
	cv::Vec<T,2> *Img_p = Img.ptr<cv::Vec<T,2> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< Img.cols;j++){
	  if(H_in_p[j] == 1){
	    means_in[0] = means_in[0] + Img_p[j][0];
	    means_in[1] = means_in[1] + Img_p[j][1];
	  }else{
	    means_out[0] = means_out[0] + Img_p[j][0];
	    means_out[1] = means_out[1] + Img_p[j][1];
	  }
	}
      }         
    }else if(Img.channels() == 3){
      for(int i = 0; i < Img.rows; ++i){
	cv::Vec<T, 3> *Img_p = Img.ptr<cv::Vec<T, 3> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< Img.cols;j++){
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
    double ener, S;
    vector<Vec2i > list;
    Vec3d Wpoint;
    //Compute the Force values for each point on the Occ curve at each camera
    for(vlist::iterator point = cam.getOcc().begin(); point < cam.getOcc().end(); point++){
      	
      ener = 0;
      list = cam.piPoint(*point);
      vector<Vec2i >::iterator point2d = list.begin();
      if(Img.channels() == 1){
	for(point2d; point2d < list.end();point2d++)
	    ener = ener - (means_in[0]- means_out[0])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])); 
      }else if(Img.channels() == 2){
	for(point2d; point2d < list.end();point2d++)
	    ener = ener - 1.0/2*((means_in[0]- means_out[0])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])) + (means_in[1]- means_out[1])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1])));
      }else if(Img.channels() == 3){
	for(point2d; point2d < list.end();point2d++)
	    ener = ener - 1.0/3*((means_in[0]- means_out[0])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])) + (means_in[1]- means_out[1])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1])) + (means_in[2]- means_out[2])*((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[2]) +(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[2])));
      }else{
	throw runtime_error("Sorry the Reconstruction functional does not take more than three channel images");
      }
      //normalized the ener
      ener = ener/list.size();
      //get Wpoint and compute energy
      cam.getSurface().getPoint(*point,Wpoint);
      S = norm(Wpoint);
      this->energy((*point)[0],(*point)[1],(*point)[2]) = this->energy((*point)[0],(*point)[1],(*point)[2]) + (S*S)/(Wpoint[2]*Wpoint[2]*Wpoint[2])*ener;
    }
  }

void Reconstruct3d::updateCamera(arma::cube const &Phi3D,std::vector<SFM_point<double> > const &Lz){


  //update the label map and Lz for all cameras
  double *labelPtr = label->memptr();
  const double *PhiPtr = Phi3D.memptr();
  for(int i = 0;i<label->n_elem;i++){
    if(PhiPtr[i] <= 0)
      labelPtr[i] = 1;
    else 
      labelPtr[i] = 0;
  }
  *this->Lz = Lz;
  this->energy.zeros();

  for(clist::iterator cam = Cameras.begin(); cam<Cameras.end(); cam++){
    cam->computeOcc();
    if(cam->getImg().depth() == CV_8U)
	this->computeCamEnergy<uchar>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_8S)
	this->computeCamEnergy<uchar>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_16U)
	this->computeCamEnergy<ushort>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_16S)
	this->computeCamEnergy<short>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_32S)
	this->computeCamEnergy<int>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_32F)
	this->computeCamEnergy<float>(*cam,Phi3D);
    else if(cam->getImg().depth() == CV_64F)
	this->computeCamEnergy<double>(*cam,Phi3D);
    else{
      throw runtime_error("Your image has an invalid depth.");
      exit(0);}
  }  
}

template<class T>
inline void Reconstruct3d::computeCamEnergy(Camera &cam, arma::cube const &Phi3D){

    typedef vector<SFM_point<double> > plist;
    double size_in,size_out;
    Mat Img = cam.getImg();
    Matx<double,3,3> R = cam.getSurface().getRot();
    dT = Vec3d(0,0,0);
    dg = Vec3d(0,0,0);
    
    //Compute the means of the features in and out of the contour of each projected image
    Mat H_in = cam.getSurfImg();
    Mat H_out = (cam.getSurfImg() - 1)*(-1);
    size_in = countNonZero(H_in);
    size_out = countNonZero(H_out);
    Vec3d means_in = Vec3d(0, 0, 0);
    Vec3d means_out = Vec3d(0, 0, 0);

    
    if(Img.channels() == 1){
      for(int i = 0; i< Img.rows;++i){
	T *Img_p = Img.ptr<T>(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0;j< Img.cols;j++){
	  if(H_in_p[j] == 1)
	    means_in[0] = means_in[0] + Img_p[j];
	  else
	    means_out[0] = means_out[0] + Img_p[j];
	}	 
      }
    }else if(Img.channels() == 2){
      for(int i = 0; i < Img.rows; ++i){
	cv::Vec<T,2> *Img_p = Img.ptr<cv::Vec<T,2> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< Img.cols;j++){
	  if(H_in_p[j] == 1){
	    means_in[0] = means_in[0] + Img_p[j][0];
	    means_in[1] = means_in[1] + Img_p[j][1];
	  }else{
	    means_out[0] = means_out[0] + Img_p[j][0];
	    means_out[1] = means_out[1] + Img_p[j][1];
	  }
	}
      }         
    }else if(Img.channels() == 3){
      for(int i = 0; i < Img.rows; ++i){
	cv::Vec<T, 3> *Img_p = Img.ptr<cv::Vec<T, 3> >(i);
	char *H_in_p = H_in.ptr<char>(i);
	char *H_out_p = H_out.ptr<char>(i);
	for(int j = 0; j< Img.cols;j++){
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
    double p_ener;
    vector<Vec2i > list;
    Vec3d p_normal;
    Vec3d X_0;
    Vec3d wpoint;
    //Compute the Force values for each point on the Occ curve at each camera
    for(vlist::iterator point = cam.getOcc().begin(); point < cam.getOcc().end(); point++){
      //Compute normal, obtain X_0, and set p_ener to 0
      p_normal = Vec3d(0,0,0);
      if((*point)[0]+1 <Phi3D.n_rows)
	p_normal[1] = (Phi3D((*point)[0]+1,(*point)[1],(*point)[2]) - Phi3D((*point)[0],(*point)[1],(*point)[2]));
      if((*point)[1]+1 <Phi3D.n_cols)
	p_normal[0] = (Phi3D((*point)[0],(*point)[1]+1,(*point)[2]) - Phi3D((*point)[0],(*point)[1],(*point)[2]));
      if((*point)[2]+1 <Phi3D.n_slices)
	p_normal[2] = (Phi3D((*point)[0],(*point)[1],(*point)[2]+1) - Phi3D((*point)[0],(*point)[1],(*point)[2]));
      p_normal = cam.getSurface().getRot()*p_normal/(norm(p_normal)+0.00000000000000001);
      
      cam.getSurface().getXo(*point,X_0);
      
      p_ener = 0;
      //Obtain the list of points projected onto the image and compute the mean of the force on those points
      list = cam.piPoint(*point);
      vector<Vec2i >::iterator point2d = list.begin();
      if(Img.channels() == 1){
	for(point2d; point2d < list.end();point2d++)
	  p_ener = p_ener + ((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])); 
      }else if(Img.channels() == 2){
	for(point2d; point2d < list.end();point2d++)
	    p_ener = p_ener + 1.0/2*(((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])) + ((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1])));
      }else if(Img.channels() == 3){
	for(point2d; point2d < list.end();point2d++)
	    p_ener = p_ener + 1.0/3*(((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[0]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[0])) + ((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[1]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[1]))+ ((Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[2])*(Img.at<uchar>((*point2d)[0],(*point2d)[1])- means_in[2]) - (Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[2])*(Img.at<uchar>((*point2d)[0],(*point2d)[1]) - means_out[2]))); 
      }else{
	throw runtime_error("Sorry the Reconstruction functional does not take more than three channel images");
      }      
      p_ener = p_ener/(((double) list.size())+0.00000000000000001);
      
      cam.getSurface().getPoint(*point,wpoint);
      //Compute the value of the force for each component of the local representation of d_lambda in SE3
      dT[0] = dT[0] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*p_normal[0];
      dT[1] = dT[1] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*p_normal[1];
      dT[2] = dT[2] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*p_normal[2];

      
      dg[0] = dg[0] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*((R*Matx<double,3,3>(0,0,0,0,0,-1,0,1,0)*X_0).dot(p_normal));
      dg[1] = dg[1] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*((R*Matx<double,3,3>(0,0,1,0,0,0,-1,0,0)*X_0).dot(p_normal));
      dg[0] = dg[2] + p_ener*norm(wpoint)/(wpoint[2]*wpoint[2]*wpoint[2]+0.00000000000000001)*((R*Matx<double,3,3>(0,-1,0,1,0,0,0,0,0)*X_0).dot(p_normal));
    }

    //Move the camera according to the gradient descent scheme
    dT = dT/(norm(dT)+1.0e-24);
    dg = dg/(norm(dg)+1.0e-24);
    Matx<double,3,3> G = Matx<double,3,3>::eye() + sin(-dt*(CV_PI/80))*Matx<double,3,3>(0,-dg[2],dg[1],dg[2],0,-dg[0],-dg[1],dg[0],0) + (1-cos(-dt*(CV_PI/80)))*(dg*dg.t() - Matx<double,3,3>::eye());

    cam.moveCamera((R*G).t(),(R*G).t()*((dt*dT) -cam.getSurface().getCenter()));
}








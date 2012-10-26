#include"IVALib/ActiveSurfaces/SFM_point.hpp"
#include<iostream>

//This file contains all the methods from the class SFM_point
 
using namespace std;
/*setNeighbors
 *This method sets the neighbors 
 */
template<class T>
vector<SFM_point<T> > SFM_point<T>::get_Neighbors(){
  //Check to see if dimensions are set
  if(this->rows == 0 || this->columns == 0){
    cout<<"The dimensions has not been established."<<endl;
    exit(0);
  }
  

  vector<SFM_point<T> > result;
  int row = (*this)[0];
  int column = (*this)[1];
  int rows = this->rows;
  int columns = this->columns;

  int temp;

  //Set the neighbor
  if(row +1 < rows){
    //point = SFM_point<T>(rows,columns,row+1,column);
    result.push_back(SFM_point<T>(rows,columns,row+1,column));
  }
  if(row-1 >= 0){
    //point = SFM_point<T>(rows,columns,row-1,column);
    result.push_back(SFM_point<T>(rows,columns,row-1,column));
  }
  if(column +1 < columns){
    //point = SFM_point<T>(rows,columns,row,column+1);
    result.push_back(SFM_point<T>(rows,columns,row,column+1));
  }
  if(column-1 >= 0){
    //point = SFM_point<T>(rows,columns,row,column-1);
    result.push_back(SFM_point<T>(rows,columns,row,column-1));
  }
  return result;
}

template class SFM_point<int>;
template class SFM_point<float>;
template class SFM_point<double>;
template class SFM_point<char>;

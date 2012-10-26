#ifndef _SFM_POINT_HPP
#define _SFM_POINT_HPP

#include"opencv2/core/core.hpp"
#include<stdio.h>
#include<vector>
#include<sstream>
#include<string>
#include <stdexcept> 


/**
 *SFM_point
 *Class used to store the points in the Linked lists described in narrow band algorithms. Derived from openCV Vec3i class.
 */

template<class T>
class SFM_point : public cv::Vec3i{
public:
  /**
   *Default Constructor
   */
  inline SFM_point<T>(){};
  /**
   *Contructor for a 2D point with intial values set blank
   *@param rows number of rows in the image the point represents
   *@param columns number of columns in the image the point represents
   */
  inline SFM_point<T>(int const &rows, int const &columns):
  slices(1)
  {
    this->rows = rows;
    this->columns = columns;
    value = 0;
  };
  /**
   *Contructor for a 3D point with intial values set blank
   *@param rows number of rows in the image the point represents
   *@param columns number of columns in the image the point represents
   */
  inline SFM_point<T>(int const &rows, int const &columns, int const &slices)
  {
    this->rows = rows;
    this->columns = columns;
    this->slices = slices;
    value = 0;
  };

  /**
   *Contructor for a 2D point with intial values set blank
   *@param rows number of rows in the image the point represents
   *@param columns number of columns in the image the point represents
   *@param row the row value of the point
   *@param column the column value of the point
   */
  inline SFM_point<T>(int const &rows, int const &columns, int const &row, int const &column):
  rows(rows),
  columns(columns),
  slices(1),
  value(0){
    (*this)[0] = row;
    (*this)[1] = column;
  };
  /**
   *Contructor for a 3D point with intial values set blank
   *@param rows number of rows in the image the point represents
   *@param columns number of columns in the image the point represents
   *@param row the row value of the point
   *@param column the column value of the point
   */
  inline SFM_point<T>(int const &rows, int const &columns, int const &slices, int const &row, int const &column, int const &slice):
  rows(rows),
  columns(columns),
  slices(slices),
  value(0){
    (*this)[0] = row;
    (*this)[1] = column;
    (*this)[2] = slice;
  };
  /**
   *Constructor from another object
   *@param obj the object that will be copied into this
   */
  /*inline SFM_point<T>(const SFM_point<T> &obj){
    memcpy(this,*obj,sizeof(SFM_point));
    }*/
  /**
  /method that retunrs list of neighbors
  */
  std::vector<SFM_point<T> > get_Neighbors();
  /**
   *getRows returns the number of rows the image the point represents have.
   */ 
  inline int getRows(){return rows;};
  /**
   *getColumns returns the number of columns the image the point represents have.
   */ 
  inline int getColumns(){return columns;};
  /**
   *getRows returns the number of rows the image the point represents have.
   */ 
  inline int getRows() const {return rows;};
  /**
   *getColumns returns the number of columns the image the point represents have.
   */ 
  inline int getColumns() const {return columns;};
  /** 
   *getSlices returns the number of slices
   */
  inline int getSlices(){return slices;};
  /** 
   *getSlices returns the number of slices
   */
  inline int getSlices() const {return slices;};
  /**
   *set
   *This method sets the value of row and column 
   *@param row value of the row of the point
   *@column value of the column of the point
   */
  inline void set(int &row,int &column){
    if(row < rows && row >=0 && column < columns && column >= 0){
      (*this)[0] = row;
      (*this)[1] = column;
  }
  else{
    std::stringstream error;
    error << "The point ("<< row <<","<< column <<") is out of bounds";
    throw std::runtime_error(error.str());
    exit(0);
  }
}
  /**
   *set
   *This method sets the value of row, column, and slice 
   *@param row value of the row of the point
   *@param column value of the column of the point
   *@param slices value of the slice of the point
   */
  inline void set(int &row,int &column, int &slice){
    if(row < rows && row >=0 && column < columns && column >= 0 && slice >= 0 && slice < slices){
      (*this)[0] = row;
      (*this)[1] = column;
      (*this)[2] = slice;
    }
    else{
      std::stringstream error;
      error <<"The point ("<<row<<","<<column<<","<<slice<<") is out of bounds";
      throw std::runtime_error(error.str());
      exit(0);
    }
  }

  /**
   *Set value of the point
   *@param value the value to be set
   */
  inline void setValue(T value){this->value = value;};
  /**
   *Gets the value of the point
   */
  inline T getValue(){return this->value;};
  /**
   *Gets the value of the point
   */
  inline T getValue() const {return this->value;};
  /**
   *methods to check foward returns true if the point foward exists
   */
  inline bool checkForward(){
    if((*this)[0] + 1 < rows)
      return true;
    else 
      return false;};
  /**
   *methods to check backward returns true if the point backward exists
   */
  inline bool checkBackward(){
    if((*this)[0] - 1 >= 0)
      return true;
    else 
      return false;};
  /**
   *methods to check up returns true if the point up exists
   */
  inline bool checkUp(){
    if((*this)[1] + 1 < columns)
      return true;
    else 
      return false;};
  /**
   *methods to check down returns true if the point down exists
   */
  inline bool checkDown(){
    if((*this)[1] - 1 >= 0)
      return true;
    else 
      return false;};
  /**
   *methods to check the foward slice
   */
  inline bool checkFSlice(){
    if((*this)[2] +1 < slices)
      return true;
    else
      return false;};
  /**
   *method to check the back slice
   */
  inline bool checkBSlice(){
    if((*this)[2] -1 >= 0)
      return true;
    else 
      return false;};
  /**
   *methods to check foward returns true if the point foward exists
   */
  inline bool checkForward() const{
    if((*this)[0] + 1 < rows)
      return true;
    else 
      return false;};
  /**
   *methods to check backward returns true if the point backward exists
   */
  inline bool checkBackward() const{
    if((*this)[0] - 1 >= 0)
      return true;
    else 
      return false;};
  /**
   *methods to check up returns true if the point up exists
   */
  inline bool checkUp() const{
    if((*this)[1] + 1 < columns)
      return true;
    else 
      return false;};
  /**
   *methods to check down returns true if the point down exists
   */
  inline bool checkDown() const{
    if((*this)[1] - 1 >= 0)
      return true;
    else 
      return false;};
  /**
   *methods to check the foward slice
   */
  inline bool checkFSlice() const{
    if((*this)[2] +1 < slices)
      return true;
    else
      return false;};
  /**
   *method to check the back slice
   */
  inline bool checkBSlice() const{
    if((*this)[2] -1 >= 0)
      return true;
    else 
      return false;};

  
private: 
  /**
   *count of points
   */
  int count;
  /**
   *number of rows
   */
  int rows;
  /**
   *number of elements
   */
  int columns;
  /**
   *number of slices
   */
  int slices;
  /**
   *value stored in point
   */
  T value;
};

#endif 

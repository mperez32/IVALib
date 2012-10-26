#include "IVALib/Plotter/SurfaceWindow.hpp"

using namespace std;
using namespace cv;


SurfaceWindow::SurfaceWindow(arma::cube data){
  SP(vtkImageData) tempImageData = SP(vtkImageData)::New();
  ArmaCube2VTKImageData(data,tempImageData);
  ImageData = image2ushort( tempImageData );
  resampler = SP(vtkImageResample)::New();
  resampler->SetInputData( ImageData );
  resampler->SetAxisMagnificationFactor(0,1.0);
  resampler->SetAxisMagnificationFactor(1,1.0);
  resampler->SetAxisMagnificationFactor(2,1.0);
  resampler->Update();
  ImageData = resampler->GetOutput();
  
  double imgRange[2];
  ImageData->GetScalarRange( imgRange );
  
  opacityTranferFunction = vtkPiecewiseFunction::New();
  
  
  opacityTranferFunction->AddPoint( imgRange[1],     0.8);
  opacityTranferFunction->AddPoint( 2,0.8);
  opacityTranferFunction->AddPoint( 1,0.0);
  opacityTranferFunction->AddPoint( 0,     0.0);
  SP(vtkColorTransferFunction) colorTransferFunction = SP(vtkColorTransferFunction)::New();
  std::vector<double> v(2);
  std::vector<int> rgb(3);
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] + (1.0 - k/double(v.size())) * imgRange[0];
    cout << "added colorpoint ... " << v[k] << endl;
    get_good_color(k,rgb);
    colorTransferFunction->AddRGBPoint(   v[k], rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0 );
  }

  //
  volumeProperty = SP(vtkVolumeProperty)::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTranferFunction);
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  
  //
  SP(vtkVolumeRayCastCompositeFunction) compositeFunction = SP( vtkVolumeRayCastCompositeFunction)::New();
  SP(vtkVolumeRayCastMapper) volumeMapper = SP(vtkVolumeRayCastMapper)::New();
  volumeMapper->SetVolumeRayCastFunction(compositeFunction);    
  volumeMapper->SetInputData( ImageData );
  volumeMapper->Update( );
  
  //
  volume = vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  
  ren1 = vtkRenderer::New();
  renWin = vtkRenderWindow::New();
  ren1->SetBackground(1.0,1.0,1.0);
  renWin->AddRenderer(ren1);
  renderWindowInteractor = vtkRenderWindowInteractor ::New();
  renderWindowInteractor->SetRenderWindow(renWin);
  
};


SP(vtkImageData) SurfaceWindow::image2ushort( vtkImageData* imageData )
  {
    SP(vtkImageData) imgvol = SP(vtkImageData)::New( );
    SP(vtkImageCast) cast = SP(vtkImageCast)::New( );
    cast->SetInputData(imageData);
    cast->SetOutputScalarTypeToUnsignedShort();
    cast->Update();
    imgvol = cast->GetOutput();
    return imgvol;
  };


std::vector<double> SurfaceWindow::get_good_color_0to7( int idx ){
    double rgb[3];
    switch( idx )
      {
      case 0:
	rgb[0]=0; rgb[1]=117; rgb[2]=220;  // ? 
	break;
      case 1:
	rgb[0]=255; rgb[1]=0; rgb[2]=16; // red
	break;
      case 2:
	//    double rgb[3] = {43,206,72}; // green
	rgb[0]=43; rgb[1]=206; rgb[2]=72; 
	break;
      case 3:
	//double rgb[3] = {224,255,102}; // uranium
	rgb[0]=224; rgb[1]=255; rgb[2]=102; 
	break;
      case 4:
	//    double rgb[3] = {194,0,136}; // mallow
	rgb[0]=194; rgb[1]=0; rgb[2]=136; 
	break;
      case 5:
	//  double rgb[3] = {255,80,5}; // zinnia
	rgb[0]=255; rgb[1]=80; rgb[2]=5; 
	break;
      case 6:
	// double rgb[3] = {220, 163, 255}; // amethyst
	rgb[0]=220; rgb[1]=163; rgb[2]=255; 
	break;
      case 7:
	//    double rgb[3] = {0,153,143}; // turquoise
	rgb[0]=0; rgb[1]=153; rgb[2]=143; 
	break;
      default:
	//    double rgb[3] = {200,200,200}; // default, grayish
	rgb[0]=200; rgb[1]=200; rgb[2]=200; 
	break;
      }
    rgb[0] /= 255.0;
    rgb[1] /= 255.0;
    rgb[2] /= 255.0;
    return std::vector<double>(rgb,rgb+3);
  };

#ifndef SURFACEWINDOW_H
#define SURFACEWINDOW_H
#include "IVALib/Utils/Utils.hpp"
#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPointsReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageResample.h"
#include "vtkImageMedian3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "opencv2/core/core.hpp"
#include <vtkMath.h>
#include <cmath>
#include <vector>


#define SP( X )  vtkSmartPointer<X> 


class SurfaceWindow {
public:

  SurfaceWindow(arma::cube);

  void get_good_color( int idx, std::vector<int>& rgb_out ){
    std::vector<double> rgb_f64 = get_good_color_0to7( idx );
    rgb_out = std::vector<int>(3);
    for( int k=0; k<3; k++ ) { rgb_out[k] = 255 * rgb_f64[k]; }
  };
 
  std::vector<double> get_good_color_0to7( int idx );

  SP(vtkImageData) image2ushort( vtkImageData* imageData );
 
  void run(){
    ren1->AddVolume(volume);
    ren1->Render();
    renderWindowInteractor->Start();
  };
  
private:
  SP(vtkImageData) ImageData;
  SP(vtkImageResample) resampler;
  SP(vtkVolumeProperty) volumeProperty;
  vtkVolume *volume;
  vtkRenderer *ren1;
  vtkRenderWindow *renWin;
  vtkRenderWindowInteractor *renderWindowInteractor;
  vtkPiecewiseFunction *opacityTranferFunction;
};




/*struct LabelVolumeMetaInfo {
  string filename;
  string volume_string;
};
*/

/*SP(vtkImageData) image2ushort( vtkImageData* imageData )
{
  SP(vtkImageData) imgvol = SP(vtkImageData)::New( );
  int dims[3];
  imageData->GetDimensions( dims );
  double spacing_in[3];
  imageData->GetSpacing(spacing_in);
  cout << "spacing:" << Mat( vector<double>(spacing_in,spacing_in+3) ) << endl;
  
  imgvol->SetDimensions( dims[0],dims[1],dims[2] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spacing_in );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );
  
  // Values stored 'linearly', slightly unsure about the orientation though.
  unsigned short*  outputPtr = (unsigned short *) imgvol->GetScalarPointer();
  short *inputPtr = static_cast<short*>( imageData->GetScalarPointer() );
  int numel                 = dims[0]*dims[1]*dims[2];
  for( int i=0; i<numel; i++ )
  {
    short invalue            =  inputPtr[i];
    unsigned short nextvalue = (unsigned short ) invalue ;
    *outputPtr= nextvalue;
    *outputPtr++;
  }
  
  return imgvol;
}


int main( int argc, char **argv)
{
  SP(vtkMetaImageReader) imgReader =  SP(vtkMetaImageReader)::New();

  std::vector< SP(vtkImageData) > multiLabels(argc-1);
  std::vector<LabelVolumeMetaInfo>    volume_info(argc-1);

  if( argc >= 2 ) {
    cout << "loading label files. warning: they should be the same size "
         << " e.g. from the same image file! " << endl;
    for( int k = 1; k < argc; k++ ) {
      cout << "attempting to load " << argv[k] << endl;
      imgReader->SetFileName( argv[k] );
      imgReader->Update();
      volume_info[k-1].filename = argv[k];

      // read it from disk, fails if file name is wacked
      SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
      imageDataTmp = vrcl::removeImageOstrava( imgReader->GetOutput(),3,5 );

      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New(); 
      img    = image2ushort( imageDataTmp );
      multiLabels[k-1] = img;
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.mha dva.mha tri.mha ... blyat.mha " << endl;
    exit(1);
  }
  
  SP(vtkImageData) imageData = mergeLabelMaps( multiLabels, volume_info);

  cout << "successfully processed labels. printing their meta info: " << endl;
  for( int k = 0; k < (int) volume_info.size(); k++ ) {
    cout << volume_info[k].filename << ", " << volume_info[k].volume_string << endl;
  }

  SP(vtkImageResample) resampler = SP(vtkImageResample)::New();
  resampler->SetInput( imageData );
  resampler->SetAxisMagnificationFactor(0,1.0);
  resampler->SetAxisMagnificationFactor(1,1.0);
  resampler->SetAxisMagnificationFactor(2,1.0);
  resampler->Update();
  imageData = resampler->GetOutput();
    
  double imgRange[2];
  imageData->GetScalarRange( imgRange );
  
  vtkPiecewiseFunction *opacityTranferFunction = vtkPiecewiseFunction::New();
  
  opacityTranferFunction->AddPoint( imgRange[1],     0.8);
  opacityTranferFunction->AddPoint( 2,0.8);
  opacityTranferFunction->AddPoint( 1,0.0);
  opacityTranferFunction->AddPoint( 0,     0.0);
  SP(vtkColorTransferFunction) colorTransferFunction = SP(vtkColorTransferFunction)::New();
  std::vector<double> v(2*multiLabels.size());
  std::vector<int> rgb(3);
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] + (1.0 - k/double(v.size())) * imgRange[0];
    cout << "added colorpoint ... " << v[k] << endl;
    get_good_color(k,rgb);
    colorTransferFunction->AddRGBPoint(   v[k], rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0 );
  }
  
  //
  SP(vtkVolumeProperty) volumeProperty = SP(vtkVolumeProperty)::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTranferFunction);
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  
  //
  SP(vtkVolumeRayCastCompositeFunction) compositeFunction = 
      SP( vtkVolumeRayCastCompositeFunction)::New();
  SP(vtkVolumeRayCastMapper) volumeMapper = SP(vtkVolumeRayCastMapper)::New();
  volumeMapper->SetVolumeRayCastFunction(compositeFunction);
  
  volumeMapper->SetInput( imageData );
  
  volumeMapper->Update( );
  
  //
  vtkVolume *volume = vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  ren1->SetBackground(1.0,1.0,1.0);
  renWin->AddRenderer(ren1);
  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor ::New();
  renderWindowInteractor->SetRenderWindow(renWin);
  
  ren1->AddVolume(volume);
  ren1->Render();
  renderWindowInteractor->Start();
  
  return 0;
}
*/
#endif

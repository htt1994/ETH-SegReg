#include "Log.h"

#include <stdio.h>
#include <iostream>
#include "argstream.h"
#include "ImageUtils.h"
#include "TransformationUtils.h"
#include <itkWarpImageFilter.h>



using namespace std;
using namespace itk;




int main(int argc, char ** argv)
{
    LOG<<CLOCKS_PER_SEC<<endl;

	feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
    typedef short PixelType;
    const unsigned int D=3;
    typedef Image<PixelType,D> ImageType;
    typedef ImageType::Pointer ImagePointerType;
    typedef ImageType::ConstPointer ImageConstPointerType;
    typedef float Displacement;
    typedef Vector<Displacement,D> LabelType;
    typedef Image<LabelType,D> LabelImageType;
    typedef LabelImageType::Pointer LabelImagePointerType;
    typedef ImageType::IndexType IndexType;

    argstream * as=new argstream(argc,argv);
    string moving,target="",def,output;
    bool NN=false;
    (*as) >> parameter ("moving", moving, " filename of moving image", true);
    (*as) >> parameter ("target", target, " filename of target image", false);
    (*as) >> parameter ("def", def, " filename of deformation", true);
    (*as) >> parameter ("out", output, " output filename", true);
    (*as) >> option ("NN", NN," use NN interpolation");
    (*as) >> help();
    as->defaultErrorHandling();
    ImagePointerType image = ImageUtils<ImageType>::readImage(moving);
    LabelImagePointerType deformation = ImageUtils<LabelImageType>::readImage(def);

    if (target != ""){
        ImageConstPointerType ref =(ImageConstPointerType) ImageUtils<ImageType>::readImage(target);
        deformation=TransfUtils<ImageType>::bSplineInterpolateDeformationField(deformation,ref);
    }
  

    if (NN){
        LOG<<"Performing NN interpolation"<<endl;
        ImageUtils<ImageType>::writeImage(output, (TransfUtils<ImageType,Displacement>::warpImage(image,deformation,true) ));
    }
    else{
        LOG<<"Performing linear interpolation"<<endl;
        ImageUtils<ImageType>::writeImage(output,  TransfUtils<ImageType,Displacement>::warpImage(image,deformation) );
    }

	return 1;
}

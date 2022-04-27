// TCLAP includes
#include <tclap/ValueArg.h>
#include <tclap/ArgException.h>
#include <tclap/CmdLine.h>

//ITK includes
#include <itkEllipseSpatialObject.h>
#include <itkSpatialObjectToImageFilter.h>
#include <itkImageFileWriter.h>

// STD includes
#include <cstdlib>
#include <string>

// ===========================================================================
// Entry point
// ===========================================================================
int main(int argc, char **argv)
{
  // =========================================================================
  // Command-line variables
  // =========================================================================
  float radius;
  float spacing;
  unsigned int size;
  std::string output;

  // =========================================================================
  // Parse arguments
  // =========================================================================
  try
    {
    TCLAP::CmdLine cmd("itkSphereImageGenerator");

    TCLAP::ValueArg<float> radiusArgument("r", "radius", "Radius of the sphere to be generated", false, 1.0f, "float");
    TCLAP::ValueArg<float> spacingArgument("s", "spacing", "Spacing betwen voxels (per axis)", false, 1.0f, "float");
    TCLAP::ValueArg<unsigned int> sizeArgument("z", "size", "Image size in voxels (per axis)", false, 100, "unsigned int");
    TCLAP::ValueArg<std::string> outputArgument("o", "output", "Output file", true, "None", "string");

    cmd.add(radiusArgument);
    cmd.add(spacingArgument);
    cmd.add(sizeArgument);
    cmd.add(outputArgument);

    cmd.parse(argc,argv);

    radius = radiusArgument.getValue();
    spacing = spacingArgument.getValue();
    size = sizeArgument.getValue();
    output = outputArgument.getValue();

    if(radius < 0.0f)
      {
      std::cerr << "Error: radius value should be greater than 0." << std::endl;
      return EXIT_FAILURE;
      }

    if (spacing < 0.0f)
      {
      std::cerr << "Error: spacing value should be greater than 0." << std::endl;
      return EXIT_FAILURE;
      }
    }
  catch(TCLAP::ArgException &e)
    {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

  // =========================================================================
  // ITK definitions
  // =========================================================================
  using ImageType = itk::Image<unsigned short,3>;
  using EllipseType = itk::EllipseSpatialObject<3>;
  using SpatialObjectToImageFilterType = itk::SpatialObjectToImageFilter<EllipseType, ImageType>;
  using ImageWriterType = itk::ImageFileWriter<ImageType>;

  ImageType::SizeType imageSize({size,size,size});
  ImageType::SpacingType imageSpacing({spacing,spacing,spacing});

  // =========================================================================
  // Ellipse generation
  // =========================================================================
  auto ellipse = EllipseType::New();
  ellipse->SetRadiusInObjectSpace(radius);
  ellipse->SetDefaultInsideValue(1);
  ellipse->Update();

  // =========================================================================
  // Ellipse mapping
  // =========================================================================
  auto imageFilter = SpatialObjectToImageFilterType::New();
  imageFilter->SetSize(imageSize);
  imageFilter->SetSpacing(imageSpacing);
  imageFilter->SetInput(ellipse);
  imageFilter->SetUseObjectValue(true);
  imageFilter->SetOutsideValue(0);

  // =========================================================================
  // Output writing
  // =========================================================================
  auto writer = ImageWriterType::New();
  writer->SetFileName(output);
  writer->SetInput(imageFilter->GetOutput());

  try
  {
    imageFilter->Update();
    writer->Update();
  }
  catch (const itk::ExceptionObject & excp)
  {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

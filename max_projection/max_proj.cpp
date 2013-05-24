#define __MAX_PROJ_CPP__
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <cmath>

// dependencies inside the hypergeom package
#include "helpers.hpp"
#include "image.hpp"
#include "utilities.hpp"

//boost 
#include "boost/date_time/posix_time/posix_time.hpp"



int main (int argc, char** argv){

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // reading the command line input
  cl_args_reader my_reader(argc,argv);
  bool			gaveHelp	= my_reader.gaveHelp_;
  const integer_t 	numframes	= my_reader.numframes_;
  const integer_t 	width		= my_reader.width_;
  const integer_t 	height		= my_reader.height_;
  bool			verbose		= my_reader.verbose_     ;
  const std::string	output_location	= my_reader.output_location_;

  if(gaveHelp)
    return 1;
 
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // setting up the image stack
  std::vector<onpro::gray16_image> image_stack(numframes);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //filling images with random values
  std::cout << "1/3: generating images" << std::endl;
  const onpro::gray16_image::payload_pixel_t max_intensity = std::numeric_limits<onpro::gray16_image::payload_pixel_t>::max();
  size_t reverse = numframes;
  const onpro::gray16_image::payload_pixel_t intensity_step = max_intensity/numframes;
  const float  radius_step = 1./float(numframes);
  const size_t num_pixels = width*height;
  
  for(size_t index=0;index<numframes;++index,--reverse){
    //initialize images
    image_stack[index].recreate(width,height);
    //fill with numbers that should mimick a body similar to a cell to be sliced by the stack
    if(index<(numframes/2))
      fill_flat_around_index(image_stack[index],
			     index*(0.5*intensity_step), //assuming index*.5 will never be larger than  max_intensity
			     (num_pixels*.5)-index,
			     (num_pixels*.5)+index
			     );
    else
      fill_flat_around_index(image_stack[index],
			     index*(0.5*intensity_step), //assuming index*.5 will never be larger than  max_intensity
			     (num_pixels*.5)-reverse,
			     (num_pixels*.5)+reverse
			     );
  }  

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //projecting the maximal pixel value along the stack index onto one image
  std::cout << "2/3: conducting projection" << std::endl;
  onpro::gray16_image projection;
  projection.recreate(width,height);
  const size_t max_PixelIndex = projection.getSize();
  onpro::gray16_image::payload_pixel_t maximumIntensityAlongStack =0;
  onpro::gray16_image::payload_pixel_t pixelIntensity =0;

  boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
  for(size_t pixIndex = 0;pixIndex<max_PixelIndex;++pixIndex){
    maximumIntensityAlongStack = 0;
    for(size_t frameIndex=0;frameIndex<numframes;++frameIndex){
      pixelIntensity = image_stack[frameIndex].data_[pixIndex];
      if(pixelIntensity>maximumIntensityAlongStack)
	maximumIntensityAlongStack = pixelIntensity;
    }
    
    projection.view_[pixIndex] = maximumIntensityAlongStack;
  }
  
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //finding maximum intensity in the projection
  onpro::gray16_image::payload_view_t::iterator max_pixel = std::max_element(projection.view_.begin(),projection.view_.end());
  size_t max_pixel_index = max_pixel - projection.view_.begin();


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //print results
  if(verbose)
    {
      std::cout << ">>\n" << image_stack[0] << "\n";
      std::cout << "projection of maxima\n"  << projection << "\n" 
		<< "max intensity: " << *max_pixel << " at index " << max_pixel_index << "\n";
      std::cout << "(begin - max it: " << projection.view_.begin() - max_pixel << ")\n";
    }

  boost::posix_time::time_duration dur = boost::posix_time::microsec_clock::local_time() - start;
  float duration_microsec = dur.total_microseconds();
  float fps = 1000.*1000.*float(numframes)/(duration_microsec);
  std::cout << "time needed:\t" << dur << "\n"
	    << "fps:\t\t" << fps << std::endl;
  
  //////////////////////////////////////////////////////////////////////////////////////////
  // writing everything to a file if required, not implemented yet
  if(!output_location.empty()){
    std::cerr << "writing to file not implemented yet!\n" << std::endl;
  }
  
  std::cout << "3/3: Done.\n";
  return 0;
}





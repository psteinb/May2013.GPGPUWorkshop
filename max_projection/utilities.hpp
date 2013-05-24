#ifndef __ONPRO_UTILITIES_HPP__
#define __ONPRO_UTILITIES_HPP__
#include <sstream>
#include <typeinfo>
#include "boost/random.hpp"

namespace onpro {

  
  template < typename ImageT >
  void fill_random(ImageT& _image, 
		   const size_t& _seed = 0,
		   const long& _lower = 0,
		   const long& _upper = std::numeric_limits<typename ImageT::payload_pixel_t>::max()){
    boost::random::mt11213b rng(_seed);
    boost::random::uniform_int_distribution<> uniform_unsigned(_lower,_upper);
    size_t imageSize = _image.getSize();
    for(size_t index = 0;index<imageSize;++index)
	_image.view_[index]=uniform_unsigned(rng);
    
    

  }

  template < typename ImageT >
  void fill_flat(ImageT& _image, 
		 const size_t& _value = 0){
    size_t imageSize = _image.getSize();
    for(size_t index = 0;index<imageSize;++index)
      _image.view_[index]=_value;
    
    

  }

  template < typename ImageT >
  void fill_flat_around_index(ImageT& _image, 
			      const size_t& _value = 0,
			      const size_t& _lower = 0,
			      const size_t& _upper = 0
			      ){

    const size_t imageSize = _image.getSize();
        
    for(size_t index = 0;index<imageSize;++index)
      if(index>_lower && index<_upper)
	_image.view_[index]=_value;
      else
	_image.view_[index]=0;
  }
  
}
#endif

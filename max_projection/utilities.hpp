#ifndef __ONPRO_UTILITIES_HPP__
#define __ONPRO_UTILITIES_HPP__
#include <sstream>
#include <typeinfo>
#include "boost/static_assert.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/random.hpp"
//#include "snappy.h"

namespace onpro {

  template <typename T>
  T stringify(const std::string& _str){
    std::istringstream instream;
    
  }
  
  
  template <typename ImageT>
  bool is_image_packed(const ImageT& _image){
    size_t sizeOfImage = _image.getSize();
    size_t widthTimesHeight = _image.height()*_image.width();
    return sizeOfImage!=widthTimesHeight;
  }
  
  template < typename ImageT >
  void fill_random(ImageT& _image){
    boost::random::mt11213b rng;
    boost::random::uniform_int_distribution<> uniform_unsigned(0,std::numeric_limits<typename ImageT::payload_pixel_t>::max());
    boost::random::uniform_int_distribution<> uniform_char(0,std::numeric_limits<char>::max());
    size_t imageSize = _image.getSize();
    for(size_t index = 0;index<imageSize;++index)
	_image.view_[index]=uniform_unsigned(rng);
    
    
    std::ostringstream str_container("");
    for(short index = 0;index < 16;index++)
      str_container << char(uniform_char(rng));
    
    _image.path_ = str_container.str();
  }


  template < typename ImageT >
  void serialize(const ImageT& _image, std::string& _string){
    //layout: fields are separated by ;
    //pixel_type 	.. 1 x char (adopted from typeid)
    //timestamp  	.. 1 x unsigned long
    //path       	.. 1 x string
    //height     	.. 1 x size_t 
    //width      	.. 1 x size_t 
    //size       	.. 1 x size_t 
    //payload		.. size x pixel_type
    
    _string.clear();
    
    boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    const unsigned long timestamp = (_image.timestamp_ - epoch).ticks();
    const size_t image_size = _image.getSize();
    
    std::ostringstream outstream;
    outstream << typeid(ImageT::payload_pixel_t).name() << ";";
    outstream << timestamp << ";";
    outstream << _image.path_ << ";";
    outstream << _image.height_ << ";";
    outstream << _image.width_ << ";";

    outstream << image_size << ";";
    
    for(size_t index = 0;index<image_size;++index){
	outstream << _image.data_[index];
	if(index!=(image_size-1))
	  outstream << " ";
    }
    
    _string = outstream.str();
  }

  template < typename ImageT >
  void deserialize(ImageT& _image, const std::string& _string){
    //for more information on split, see http://www.boost.org/doc/libs/1_51_0/doc/html/string_algo/usage.html
    std::vector<std::string> split_result; 
    boost::split( split_result, _string, boost::is_any_of(";") ); 
    
    boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
    _dest.timestamp_ = epoch + boost::posix_time::milliseconds(_src.timestamp_());

  }
  

}
#endif

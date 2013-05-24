#ifndef __ONPRO_IMAGE_HPP__
#define __ONPRO_IMAGE_HPP__
#include <typeinfo>
#include <algorithm>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/time_serialize.hpp"
#include <boost/serialization/vector.hpp>
#include "boost/gil/gil_all.hpp"
#include <boost/serialization/array.hpp>
#include <boost/static_assert.hpp>

namespace onpro {
  
   
  enum SourceType {
    RawImage = 1, //use size that is computed from image dimensions
    CompressedImage = 2 //use the size that is set by the method
  };

  //this injects size
  template <typename Daughter, SourceType SourceT = RawImage >
  struct with_size_method {

    with_size_method() : size_(0) {};

    size_t getSize() const {
      //make this depent on T
      size_t value = static_cast<const Daughter&>(*this).width_*static_cast<const Daughter&>(*this).height_;
    
      return value;
    }
  
    void setSize(const size_t& _size){
      return;
    }
    
    size_t  size_;
  };

  //this injects size
  template <typename Daughter>
  struct with_size_method<Daughter, CompressedImage> {

    with_size_method() : size_(0) {};

    size_t getSize() const {
      return size_;
    }
  
    void setSize(const size_t& _size){
      size_ = _size;
      return;
    }

    size_t  size_;
  };


  template <typename PayloadPolicy, SourceType ImageType = RawImage>
  struct image : public PayloadPolicy, public with_size_method<image<PayloadPolicy,ImageType>, ImageType> {
  
    typedef typename boost::gil::channel_type<typename PayloadPolicy::value_type>::type payload_pixel_t;
    typedef typename PayloadPolicy::view_t payload_view_t;
    typedef typename PayloadPolicy::const_view_t payload_const_view_t;
    typedef typename PayloadPolicy::value_type payload_value_t;

    boost::posix_time::ptime		timestamp_;
    std::string 			path_;
    size_t				height_;
    size_t				width_;
    payload_view_t			view_;
    payload_pixel_t*			data_;
    static const SourceType		src_type_ = ImageType;

    //in order not to double memory consumption
    //http://boost.2283326.n4.nabble.com/serialization-Serializing-array-of-POD-or-custom-type-td4631974.html
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
      ar & timestamp_ & path_ & height_ & width_;
      size_t temp_size = this->getSize();
      ar & temp_size;
      
      for(size_t index = 0;index<this->getSize();++index){
	//std::cout << "saving .. " << index << "/" << this->getSize() << ": " << data_[index] << "("<< view_[index] <<")\n";
	ar & data_[index];}
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
      ar & timestamp_ & path_ & height_ & width_;
      size_t temp_size = 0;
      ar & temp_size;
      this->setSize(temp_size);
      if(data_){
	delete[] data_;
	data_ = 0;
      }
      payload_pixel_t temp_data[temp_size];
      for(size_t index = 0;index<temp_size;++index){
	ar & temp_data[index];
      }

      create_image_from_buffer(*this,&temp_data[0],height_,width_);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    image():
    PayloadPolicy(),
		     timestamp_(boost::posix_time::microsec_clock::local_time()),
		     path_(),
		     height_(0),
		     width_(0),
		     view_(),
		     data_(0)
    {
      
    }

    image(const PayloadPolicy& _data, const std::string& _path="") :
      PayloadPolicy(_data),
      timestamp_(boost::posix_time::microsec_clock::local_time()),
      path_(_path),
      height_(_data.height()),
      width_(_data.width()),
      view_(),
      data_()
    {
      this->setSize(height_*width_);//default size, can be changed afterwards
      view_ = view(*this);
      data_ = interleaved_view_get_raw_data(view_);
    };

    image(const image& _rhs) :
      PayloadPolicy(_rhs),
      timestamp_(_rhs.timestamp_),
      path_(_rhs.path_),
      height_(_rhs.height()),
      width_(_rhs.width()),
      view_(),
      data_(0)
    {
      this->setSize(height_*width_);//default size, can be changed afterwards
      view_ = view(*this);
      data_ = interleaved_view_get_raw_data(view_);
    }
    
    image& operator=(const image& _rhs){
      //this should do for now, but I'd rather like to have some copy-and-swap inplace
      PayloadPolicy::operator=(static_cast<PayloadPolicy const&>(_rhs));
      this->setSize(_rhs.getSize());
      timestamp_ = _rhs.timestamp_;
      path_= _rhs.path_;
      height_= _rhs.height_;
      width_= _rhs.width_;
      view_= view(*this);
      data_= interleaved_view_get_raw_data(view_);
      return *this;
    }

    ~image(){
    }

    void recreate(const size_t& _width, const size_t& _height){
      PayloadPolicy::recreate(_width,_height);
      this->width_ = _width;
      this->height_ = _height;
      this->view_ = view(*this);
      this->data_ = interleaved_view_get_raw_data(this->view_);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //the following methods don't belong here, but are inside the class declaration 
    //to ease coding

    friend std::ostream& operator<<(std::ostream& _stream, const image& _image){
      std::cout << "|image : " << _image.path_ << " - " << _image.timestamp_ << "\n";
      std::cout << "|w.h   : " << _image.width_ << "x" << _image.height_ << ", payload length: " << _image.getSize() << "\n";
      std::cout << "|pixels:\n|\t";
      for(int i = 0;i<_image.getSize();i++)
	std::cout << _image.view_[i] << (((i+1) % (_image.width_) == 0 && i>0) ? "\n|\t" : " ");
      
      std::cout << std::endl;
      
      return _stream;
    };

    template <typename BufferT>
    friend void create_image_from_buffer(image& _image, BufferT* _buffer, const size_t& _height, const size_t& _width ){
    
      BOOST_STATIC_ASSERT(sizeof(BufferT)==sizeof(payload_value_t));
      
      size_t bytes_per_row = _width*sizeof(BufferT);
      payload_view_t temp_view = boost::gil::interleaved_view(_width, _height, 
                                                                               (payload_value_t*)_buffer, bytes_per_row);
      _image.recreate(_width, _height);
      _image.view_ = view(_image);
      copy_pixels(temp_view,_image.view_);
      _image.data_ = interleaved_view_get_raw_data(_image.view_);
      
    }

    friend inline size_t size_in_byte(const image& _frame){
      size_t value = _frame.path_.size()
	+(sizeof(payload_pixel_t)*_frame.height_*_frame.width_)
	+sizeof(_frame.timestamp_)
	+2*(sizeof(_frame.height_))
	+(sizeof(_frame.getSize()))
	;
      return value;
    }
	
  };

  typedef image<boost::gil::gray16_image_t> gray16_image;
  typedef image<boost::gil::gray8_image_t> gray8_image;

  typedef image<boost::gil::gray16_image_t,CompressedImage> gray16_compressed_image;
  typedef image<boost::gil::gray8_image_t,CompressedImage> gray8u_compressed_image;
  typedef image<boost::gil::gray8s_image_t,CompressedImage> gray8s_compressed_image;
  typedef image<boost::gil::gray8s_image_t,CompressedImage> compressed_image;

}

#endif

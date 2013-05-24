#ifndef __HYPERGEOM_HELPERS_HPP__
#define __HYPERGEOM_HELPERS_HPP__

#include <typeinfo>
#include <sstream>
#include <iostream>
#include "unistd.h"
#include <fstream>

typedef long integer_t;

template<typename T>
bool charPtr_to(char * _optarg, T& _result){
  std::istringstream instream;instream.str(_optarg);
  T meta;
  if( !(instream >> meta) ){
    std::cerr << __FILE__ << "\t unable to convert "<< _optarg << " to type " << typeid(_result).name() << std::endl;
    return false;
  }
  _result = meta;
  return true;
  
}



struct cl_args_reader {
  

  bool		gaveHelp_	;
  integer_t 	numframes_		;
  integer_t 	width_	;
  integer_t 	height_	;
  bool 		verbose_	;
  std::string   output_location_;

  cl_args_reader(int argc, char** argv) : 
    gaveHelp_       (false),
    numframes_          (0),
    width_        (0),
    height_        (0),
    verbose_   (false),
    output_location_("")
  {
    int opt = 0;
    try{
      while( (opt = getopt(argc, argv, "n:W:H:o:vh" ))!=-1 ){       
	switch(opt){
	case 'n':
	  charPtr_to(optarg,numframes_);
	  break;
	case 'W':
	  charPtr_to(optarg,width_);
	  break;
	case 'H':
	  charPtr_to(optarg,height_);
	  break;
	case 'o': 
	  output_location_ = optarg;
	  break; 
	case 'h': 
	  gaveHelp_ = true;
	  printHelp();
	  break; 
	case 'v': 
	  verbose_ = true;
	  break; 
	default:
	  gaveHelp_ = true;
	  printHelp();
	  break;
	} 
      }
    }
    catch(std::exception& m_exc){
      std::cerr << __FILE__ << ":" << __LINE__ << "\t unable to read command line arguments, " << m_exc.what() << std::endl;
      throw;
    }

  }

  void printHelp(){
  
    std::cout << "\t usage:\n"
	      << "\t executable <flags/args>\n"
	      << "\t -n <num>\t\t ... numframes \n"
	      << "\t -H <num>\t\t ... width \n"
	      << "\t -W <num>\t\t ... height \n"
	      << "\t -o <file_location>\t ... file to dump the results to (path must exist)\n"
	      << "\t -v\t\t\t ... verbose output \n"
	      << "\t -h\t\t\t ... print help message\n";
  }

  friend std::ostream& operator<<(std::ostream& _stream, const cl_args_reader& _config){
    std::cout << ">> num frames : " << _config.numframes_ << ", w x h: " << _config.width_ << "x" << _config.height_ << "\n"
	      << ">> output : "<< _config.output_location_ << "\n";
      
    };


};

void store_results(const std::vector<double>& _results, const std::string& _filename){
  std::ofstream outfile(_filename.c_str());
  outfile.precision(7);
  // std::cout.precision(7);
  // std::cout << "v P" << std::endl;
  
  outfile << "v P" << std::endl;

  for(size_t i = 0; i < _results.size(); i++){
    // std::cout <<  i << " " << _results.at(i) << std::endl;
    outfile <<  i << " " << _results.at(i) << std::endl;
  }
  outfile.close();
  std::cout << "results written to " << _filename << "\n";
}
#endif

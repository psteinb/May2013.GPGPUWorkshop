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
  // instream.str(optarg);
  // if( !(instream >> meta) ){
  // 	std::cerr << __FILE__ << "\t invalid argument format for [-n]" << std::endl;
  // 	total = 0;
  // }
  // else{
  // 	total = meta;
  // }
  std::istringstream instream;instream.str(_optarg);
  T meta;
  if( !(instream >> meta) ){
    std::cerr << __FILE__ << "\t unable to convert "<< _optarg << " to type " << typeid(_result).name() << std::endl;
    return false;
  }
  _result = meta;
  return true;
  
}


void printArgs(unsigned _n,unsigned _a,unsigned _b,unsigned _q){
  
  std::cout << "\t -n = " << _n
	    << "\t -a = " << _a
	    << "\t -b = " << _b
	    << "\t -q = " << _q
	    << "\n";
}

struct cl_args_reader {
  

  bool		gaveHelp_	;
  integer_t 	total_		;
  integer_t 	a_lower_	;
  integer_t 	b_upper_	;
  integer_t 	q_second_upper_	;
  bool 		progress_bar_	;
  bool 		dump_result_	;
  std::string   output_location_;

  cl_args_reader(int argc, char** argv) : 
    gaveHelp_       (false),
    total_          (0),
    a_lower_        (0),
    b_upper_        (0),
    q_second_upper_ (0),
    progress_bar_   (false),
    dump_result_    (false),
    output_location_("")
  {
    int opt = 0;
    try{
      while( (opt = getopt(argc, argv, "n:a:b:q:o:pdh" ))!=-1 ){       
	switch(opt){
	case 'n':
	  charPtr_to(optarg,total_);
	  break;
	case 'a':
	  charPtr_to(optarg,a_lower_);
	  break;
	case 'b':
	  charPtr_to(optarg,b_upper_);
	  break;
	case 'q':
	  charPtr_to(optarg,q_second_upper_);
	  break;
	case 'o': 
	  output_location_ = optarg;
	  break; 
	case 'h': 
	  gaveHelp_ = true;
	  printHelp();
	  break; 
	case 'd': 
	  dump_result_ = true;
	  break; 
	case 'p': 
	  progress_bar_ = true;
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
	      << "\t -n <num>\t\t ... total \n"
	      << "\t -a <num>\t\t ... a_lower \n"
	      << "\t -b <num>\t\t ... b_upper \n"
	      << "\t -q <num>\t\t ... q_second_upper \n"
	      << "\t -o <file_location>\t ... file to dump the results to (path must exist)\n"
	      << "\t -p\t\t\t ... print progress bar (default: false)\n"
	      << "\t -d\t\t\t ... dump results at end (default: false)\n"
	      << "\t -h\t\t\t ... print help message\n";
  }

};

template <typename NumType = long>
class increaseByOne {

  NumType current_;
  
public:
  
  increaseByOne(const NumType& _init_value):
    current_(_init_value){}


  NumType operator()() {
    return current_++;
  }
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

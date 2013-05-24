#define __HYPERGEOM_PDF_CPP__
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <cmath>

// dependencies inside the hypergeom package
#include "helpers.hpp"


using namespace std;

int main (int argc, char** argv){

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  // reading the command line input
  cl_args_reader my_reader(argc,argv);
  bool		gaveHelp	= my_reader.gaveHelp_;
  integer_t 	total		= my_reader.total_;
  integer_t 	a_lower 	= my_reader.a_lower_         ;
  integer_t 	b_upper 	= my_reader.b_upper_         ;
  integer_t 	q_second_upper	= my_reader.q_second_upper_;
  bool 		progress_bar	= my_reader.progress_bar_    ;
  bool 		dump_result	= my_reader.dump_result_     ;
  std::string output_location	= my_reader.output_location_;

  if(gaveHelp)
    return 1;

  std::cout << __FILE__ ;
  printArgs(total,a_lower,b_upper,q_second_upper);
  if(!(a_lower<total && b_upper<total && q_second_upper<=total)){
    std::cerr << "input args did not fulfill requirements:\n"
	      << "a,b < n and q <= n\n";
    return 1;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // defining some variables needed for inner-loop calculations

  integer_t firstIdx, secondIdx, thirdIdx, forthIdx,  mmax, mmin, lmax, lmin;
  double qs = 0.;
  float prog = 0.;
  integer_t alpha = std::min(a_lower,b_upper);
  vector<double> dist ((alpha+1),0.0);

  const integer_t zero = 0;
  const integer_t firstIdxBegin = std::max(a_lower+b_upper-(q_second_upper+total),zero);
  const integer_t firstIdxEnd = (alpha+1);
  const integer_t constant_1 = a_lower-total+q_second_upper;
  const integer_t constant_2 = total-q_second_upper;
  const integer_t constant_3 = total-q_second_upper-a_lower;
  const integer_t constant_4 = total+q_second_upper-a_lower;
  const integer_t constant_5 = total+q_second_upper-a_lower-b_upper;
  const double	  globalInvariant = lgamma(total-q_second_upper+1) 
    + lgamma(q_second_upper+1) 
    - lgamma(total+1) 
    + lgamma(total-a_lower +1) 
    + lgamma(a_lower+1) 
    - lgamma(total+q_second_upper+1) 
    + lgamma(total+q_second_upper-b_upper+1) 
    + lgamma(b_upper+1) 
    ;


  double first_looplevel_invariant  = 0.;
  double second_looplevel_invariant = 0.;
  double third_looplevel_invariant  = 0.;

  double sum = 0.;
  for(firstIdx = firstIdxBegin; firstIdx < firstIdxEnd; firstIdx++){

    if(progress_bar){
      prog = ((firstIdx+0.0)/alpha)*100;

      std::cout << setprecision(3) << "\r   Calculating probabilities... " << prog << "%";
      //      fflush(stdout);
      std::cout.flush();

    }

    qs = 0.;
    sum = 0.;
    mmin = max((constant_1-firstIdx),zero);
    mmax = min((a_lower-firstIdx),q_second_upper);
    
    first_looplevel_invariant = -1.*lgamma(b_upper-firstIdx+1) ;


    for(secondIdx = mmin; secondIdx <= mmax; secondIdx++){
      
      lmin = max((constant_1-secondIdx),zero);
      lmax = min(firstIdx,(q_second_upper-secondIdx));

      second_looplevel_invariant = -1.*lgamma(secondIdx+1) - lgamma(a_lower-firstIdx-secondIdx+1) ;


      for(thirdIdx = lmin; thirdIdx <= lmax; thirdIdx++){
	
	third_looplevel_invariant = lgamma(constant_2-firstIdx+thirdIdx+1)
	  - lgamma(firstIdx-thirdIdx+1)
	  - lgamma(q_second_upper-thirdIdx-secondIdx+1) 
	  - lgamma(constant_2-firstIdx+thirdIdx+1) 
	  - lgamma(constant_3+thirdIdx+secondIdx+1) 
	  ;

	for(forthIdx = 0; forthIdx <= thirdIdx; forthIdx++){

	  sum = globalInvariant + first_looplevel_invariant + second_looplevel_invariant + third_looplevel_invariant
	    - lgamma(thirdIdx-forthIdx+1) 
	    - lgamma(forthIdx+1) 
	    + lgamma(total+q_second_upper-a_lower-secondIdx-forthIdx+1) ;


	  if((constant_5-secondIdx-forthIdx+firstIdx+1)>0)
	    {
	    sum -= lgamma(constant_5-secondIdx-forthIdx+firstIdx+1) ;

	  }
	  
	  qs += exp( sum );
	  //qs = qs + exp( (lgamma(n-q+1) - lgamma(n-q-i+l+1) - lgamma(i-l+1) + lgamma(q+1) - lgamma(l-j+1) - lgamma(j+1) - lgamma(q-l-m+1) - lgamma(m+1) + lgamma(n-i-q+l+1) - lgamma(n-q+l-a+m+1) - lgamma(a-i-m+1) + lgamma(n+q-a-m-j+1) - lgamma(n+q-a-m-j-b+i+1) - lgamma(b-i+1) - lgamma(n+1) + lgamma(n-a+1) + lgamma(a+1) - lgamma(n+q+1) + lgamma(n+q-b+1) + lgamma(b+1)) );
	  
	}
      }
    }

    dist.at(firstIdx) = qs;
  }

  //////////////////////////////////////////////////////////////////////////////////////////
  // writing everything to a file if required
  if(dump_result){
    std::ofstream outfile(output_location.c_str());
    std::cout.precision(7);;
    outfile.precision(7);
    std::cout << ">> v P" << endl;
    outfile << "v P" << endl;

    for(size_t i = 0; i < dist.size(); i++){
      std::cout << ">> "<< i << " " << dist.at(i) << std::endl;
      outfile <<  i << " " << dist.at(i) << std::endl;
    }
    outfile.close();
  }
  
  std::cout << "Done.\n";
  return 0;
}





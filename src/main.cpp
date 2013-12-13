#include <sndfile.hh>

#include <iostream>
#include <vector>
#include <fstream>

#include "options.hpp"

#include "wav2json.hpp"

bool progress_callback(size_t percent)
{
    std::cerr << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bconverting: " << percent << "%";
    return true;
}


int main(int argc, char* argv[])
{
  Options options(argc, argv);

  using std::endl;
  using std::cout;
  using std::cerr;

  std::ofstream output;
  std::ostream ofs(std::cout.rdbuf());

  // If the output file name provided is "-", use stdout.  Otherwise open the filename provided.
  if (options.output_file_name != "-")
  {
    output.open(options.output_file_name.c_str());
    ofs.rdbuf(output.rdbuf());
  }


  //it appears, that javascript is fine with scientific notation
  //ofs << std::fixed; //explicitly use fixed notation
  //ofs << std::scientific;
  ofs.precision( options.precision );
  
  ofs << "{" << std::endl;

  /*
    Precision vs. file size benchmark:

    precision 6:
    -rw-r--r--   1 beschulz  staff    606 Aug 18 19:36 test1.json
    -rw-r--r--   1 beschulz  staff  43431 Aug 18 19:36 test2.json
    -rw-r--r--   1 beschulz  staff  39104 Aug 18 19:36 test3.json

    presision 2:
    -rw-r--r--   1 beschulz  staff    384 Aug 18 19:55 test1.json
    -rw-r--r--   1 beschulz  staff  15291 Aug 18 19:55 test2.json
    -rw-r--r--   1 beschulz  staff  19499 Aug 18 19:55 test3.json    
  */

  if (!options.no_header)
  {
    ofs << "  \"_generator\":\"wav2json version " << version::version << " on " << version::platform << " (http://goo.gl/af7wg)\"," << std::endl;
  }

  for(size_t i = 0; i != options.channels.size(); ++i)
  {
    // open sound file
    SndfileHandle wav(options.input_file_name.c_str());

	// output sound duration
	if (i == 0) {
		ofs << "  \"duration\":" << wav.frames()/wav.samplerate() << "," << std::endl;	  
	}
  
    // handle error
    if ( wav.error() )
    {
        cerr << "Error opening audio file '" << options.input_file_name << "'" << endl;
        cerr << "Error was: '" << wav.strError() << "'" << endl; 
        return 2;
    }

    Options::Channel channel = options.channels[i];

    ofs << "  \"" << channel << "\":";

    compute_waveform(
      wav,
      ofs,
      options.samples,
      channel,
      options.use_db_scale,
      options.db_min,
      options.db_max,
      progress_callback
    );

    if (i != options.channels.size()-1) //only write comma, if this is not the last entry
      ofs << "," << std::endl;
  }

  ofs << std::endl << "}" << std::endl;

  cerr << endl;

  return 0;
}

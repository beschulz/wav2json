#include <sndfile.hh>

#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>

#include "options.hpp"

#include "wav2json.hpp"

bool progress_callback(int percent)
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

  std::ofstream ofs(options.output_file_name.c_str());
  
  ofs << "{" << std::endl;

  for(size_t i = 0; i != options.channels.size(); ++i)
  {
    // open sound file
    SndfileHandle wav(options.input_file_name);

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

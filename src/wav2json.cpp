#include "wav2json.hpp"

#include <math.h>
//#include <cmath>
#include <iostream>
#include <vector>
#include <assert.h>
#include <sndfile.hh>

/*
  clamp x into range [min...max]
*/
template <typename T>
const T& clamp(const T& x, const T& min, const T& max)
{
  return std::max(min, std::min(max, x));
}

/*
  metaprogramming functions to get value range of sample format T.
*/
template <typename T> struct sample_scale {};

template <> struct sample_scale<short>
{
  static const unsigned short value = 1 << (sizeof(short)*8-1);
};

template <> struct sample_scale<float>
{
  static const int value = 1;
};

/*
  conversion from and to dB
*/
float float2db(float x)
{
  x = fabsf(x);

  if (x > 0.0f)
    return 20.0f * log10f(x);
  else
    return -9999.9f;
}

float db2float(float x)
{
  return powf(10.0f, x/20.f);
}

/*
  map value x in range [in_min...in_max] into range [out_min...out_max]
*/
float map2range(float x, float in_min, float in_max, float out_min, float out_max)
{
  return clamp<float>(
    out_min + (out_max-out_min)*(x-in_min)/(in_max-in_min),
    out_min,
    out_max
  );
}

template <typename T>
T compute_sample(const std::vector<T>& block, int i, int n_channels, Options::Channel channel)
{
  switch(channel)
  {
    case Options::LEFT : return block[i];
    case Options::RIGHT: assert( n_channels == 2 ); return block[i+1];
    case Options::MID  : assert( n_channels == 2 ); return (block[i] + block[i+1]) / T(2);
    case Options::SIDE : assert( n_channels == 2 ); return (block[i] - block[i+1]) / T(2);
    case Options::MIN  : assert( n_channels == 2 ); return std::min(block[i], block[i+1]);
    case Options::MAX  : assert( n_channels == 2 ); return std::max(block[i], block[i+1]);
    default : assert(false && "no such channel"); break;
  }
  return T(0);
}

/*
  compute the waveform of the supplied audio-file and store it into out_image.
*/
void compute_waveform(
  const SndfileHandle& wav,
  std::ostream& output_stream,
  size_t samples,
  Options::Channels channels,
  bool use_db_scale,
  float db_min,
  float db_max,
  progress_callback_t progress_callback
)
{
  using std::size_t;
  using std::cerr;
  using std::endl;

  // you can change it to float or short, short was much faster for me.
#ifdef USE_FLOAT
  typedef float sample_type;
#else
  typedef short sample_type;
#endif

  samples = std::min(wav.frames(), (sf_count_t)samples);

  int frames_per_pixel  = std::max<int>(1, wav.frames() / samples);
  int samples_per_pixel = wav.channels() * frames_per_pixel;
  std::size_t progress_divisor = std::max<std::size_t>(1, samples/100);

  // temp buffer for samples from audio file
  std::vector<sample_type> block(samples_per_pixel);

  // filter out channels, that require more channels than the wav file has
  channels.erase(
      std::remove_if(channels.begin(), channels.end(), [&wav](Options::Channel channel){
          if ((channel == Options::MID  ||
               channel == Options::SIDE ||
               channel == Options::RIGHT||
               channel == Options::MIN  ||
               channel == Options::MAX) &&
               wav.channels() == 1
          )
          {
            std::cerr << "Warning: your trying to generate output for channel '" << channel << "', but the input has only one channel. removing requested channel." << std::endl;
            return true;
          }
          return false;
      }),
      channels.end()
  );

  if (channels.empty())
  {
    std::cerr << "Warning: there are no channels left to process, aborting." << endl;
    return;
  }

  // create one vector of floats for each requested channel
  std::vector<std::vector<float> > output_values( channels.size() );

  // https://github.com/beschulz/wav2json/pull/7
  // http://www.mega-nerd.com/libsndfile/api.html#note2
  const_cast<SndfileHandle&>(wav).command(SFC_SET_SCALE_FLOAT_INT_READ, 0, SF_TRUE);

  /*
    the processing works like this:
    for each vertical pixel in the image (x), read frames_per_pixel frames from
    the audio file and find the min and max values.
  */
  for (size_t x = 0; x < samples; ++x)
  {
    // read frames
    sf_count_t n = const_cast<SndfileHandle&>(wav).readf(&block[0], frames_per_pixel) * wav.channels();
    assert(n <= (sf_count_t)block.size());

    // for each requested channel
    for(size_t channel_idx = 0; channel_idx != channels.size(); ++channel_idx)
    {
      // find min and max
      sample_type max(0);

      Options::Channel channel = channels[channel_idx];
      for (int i=0; i<n; i+=wav.channels()) // seek to next frame
      {
        sample_type sample = compute_sample(block, i, wav.channels(), channel);
        sample_type abs_sample = std::abs(sample);
        max = std::max( max, abs_sample );
      }

      float y = use_db_scale?
              map2range( float2db(max / (float)sample_scale<sample_type>::value ), db_min, db_max, 0, 1):
              map2range( max, 0, sample_scale<sample_type>::value, 0, 1);

      // print progress
      if ( x%(progress_divisor) == 0 )
      {
        if ( progress_callback && !progress_callback( 100*x/samples ) )
          return;
      }

      output_values[channel_idx].push_back(y);
    }
  }

  // finally output the collected values
  for(size_t channel_idx = 0; channel_idx != channels.size(); ++channel_idx)
  {
    Options::Channel channel = channels[channel_idx];

    output_stream << "  \"" << channel << "\": [";
    for (size_t i = 0; i != output_values[channel_idx].size(); ++i)
    {
      output_stream << output_values[channel_idx][i];
      if (i != output_values[channel_idx].size()-1)
        output_stream << ","; // only output comma, if not the last ellement
    }
    output_stream << "]," << endl;
  }

  // call the progress callback
  if ( progress_callback && !progress_callback( 100 ) )
    return;
}

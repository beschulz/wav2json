#include <sndfile.hh>
#include "options.hpp"

#ifdef __OBJC__
    typedef bool (^progress_callback_t)(size_t progress);
#else /* __OBJC__ */
    typedef bool (*progress_callback_t)(size_t progress);
#endif /* __OBJC__ */

void compute_waveform(
  const SndfileHandle& wav,
  std::ostream& output_stream,
  size_t samples,
  Options::Channels channels,
  bool use_db_scale,
  float db_min,
  float db_max,
  progress_callback_t progress_callback
);

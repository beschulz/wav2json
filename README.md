wav2json
========

Generate waveformjs.org compatible json data out of wav files.

Author: Benjamin Schulz

email: beschulz[the a with the circle]betabugs.de  

License: GPL

Note: the file examples/waveform.js is from http://waveformjs.org/

This application is based on [wav2png](https://github.com/beschulz/wav2png/) but does generate json data instead of pngs.

#Examples

	wav2json song.wav --channels left right -o song.json

might produce output like

	{
		"left":[0,0.499969,0.865997,0.999969,0.865997,0.499969,0,0.5,0.866028,0,0.866028,0.5],
		"right":[0,0.499969,0.865997,0.999969,0.865997,0.499969,0,0.5,0.866028,0,0.866028,0.5],
	}

Also take a look at some [example renderings](http://beschulz.github.com/wav2json/).

# Performance
Similar to wav2png (e.g. very good). But it might be a little slower, when generating output for multiple channels.
Since the input file is opened multiple times, I'd recommend to write it to a temporary file first, so you can take advantage of the operating systems file cache:

	sox ../song.mp3 -c 2 -t wav tmp.wav
	wav2json tmp.wav --channels left right mid side min max -o song.json
	rm tmp.wav

Also note, that you can dramatically reduce the file size, by reducing the precision of the output. In my tests, even a precision of 1 looked good.

# Installation

## On Linux (Ubuntu, Debian)

###
	if you're using a Linux distributing, that supports apt-get or you're on OSX and have homebrew installed, you
	might want to try:
	```
		cd build
		make install_dependencies
	```

### install dependencies
    apt-get install make g++ libsndfile1-dev libboost-program-options-dev

### Build
	cd build
    make all

## On Max OS

### install dependencies
* Get the Xcode command line tools
	* Starting with Xcode 4.3, Apple does not install command line tools by default anymore, so after Xcode installation, go to Preferences > Downloads > Components > Command Line Tools and click Install. You can also directly [download Command Line Tools](https://developer.apple.com/downloads) for Xcode without getting Xcode.
* [Install homebrew](https://github.com/mxcl/homebrew/wiki/installation)
* install libsndfile: in the shell: ```brew install libsndfile```
* install boost: in the shell: ```brew install boost```

### Build
* in the shell: ```cd build && make all```

### Add wav2json
Either

	mv ../bin/Darwin/wav2json /usr/local/bin

Or add this to your .bash_profile making sure to change the directory to where you cloned the repo

	# wav2json
	PATH=$PATH\:/Path/to/wav2json/src/wav2json/bin/Darwin ; export PATH

#Usage

	wav2json version 0.2
	written by Benjamin Schulz (beschulz[the a with the circle]betabugs.de)

	usage: wav2json [options] input_file_name
	example: wav2json my_file.wav

	Allowed options:

	Generic options:
	  -v [ --version ]      print version string
	  --help                produce help message

	Configuration:
	  -s [ --samples ] arg (=800)           number of samples to generate
	  --channels arg (=left right )         channels to compute: left, right, mid, 
	                                        side, min, max
	  -p [ --precision ] arg (=6)           precision of the floats, that are 
	                                        generated. [1..6], reduce for smaller 
	                                        sized files. Usually 2 should be 
	                                        sufficient!
	  -o [ --output ] arg                   name of output file, defaults to <name 
	                                        of inputfile>.json
	  -c [ --config-file ] arg (=wav2json.cfg)
	                                        config file to use
	  -d [ --db-scale ]                     use logarithmic (e.g. decibel) scale 
	                                        instead of linear scale
	  --db-min arg (=-48)                   minimum value of the signal in dB, that
	                                        will be visible in the waveform
	  --db-max arg (=0)                     maximum value of the signal in dB, that
	                                        will be visible in the waveform. 
	                                        Useful if you know that your signal 
	                                        peaks at a certain level.
	  -n [ --no-header ]                    Do not include the version info banner 
	                                        in the output


One thing, thats noteworthy is, that you can generate output for multiple cahannels:

* left is the left channel of the audio file.
* right is the right channel of the audio file
* mid is the mid channel of the audio file computed per sample as: mid = (left + right) / 2
* side is the side channel of the audio file computed per sample as: side = (left + right) / 2
* max is the maximum of the channels of the audio file computed per sample as: max = max(left, right)
* min is the minimum of the channels of the audio file computed per sample as: min = min(left, right)

Obviously outputting the right, mid, side, min, max channels does only make sense on stereo input. When the input is mono,
the applications falls back to outputing the left channel and prints a warning to stderr.

When there are more than two channels present, the output might be a little non-sensical, depending on what information is stored in the first and second channel.

Also note, that if there are less samples in the audio file, than defined by --samples, only as many samples as there are in the audio file will be written to the json file. The interpolation should be done by waveform.js

# Donations
If you find wav2png incredibly usefull nd use it a lot, feel free to make a small [donation via paypal](http://goo.gl/Ey2Bp).
While it is highly appreciated, it is absolutely not necessary to us the software.

If you find any issues, feel free to contact me.
and most important: enjoy and have fun :D


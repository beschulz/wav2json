#ifndef OPTIONS_HPP__
#define OPTIONS_HPP__

#include <boost/program_options.hpp>
#include <string>
#include "./version.hpp"

struct Options
{
	Options(int argc, char* argv[])
	{
		namespace po = boost::program_options;

		po::options_description generic("Generic options");
			generic.add_options()
    		("version,v", "print version string")
    		("help", "produce help message")    
    	;

    	Channels default_chanels;
    	default_chanels.push_back(LEFT);
    	default_chanels.push_back(RIGHT);

    	po::options_description config("Configuration");
		config.add_options()
    		("samples,s", po::value<unsigned>(&samples)->default_value(800), 
          		"number of samples to generate")
    		("channels", po::value<Channels>(&channels)->default_value(default_chanels)->multitoken(),
          		"channels to compute: left, right, mid, side, min, max")
    		("output,o", po::value<std::string>(&output_file_name)->default_value(""), 
          		"name of output file, defaults to <name of inputfile>.json")
    		("config-file,c", po::value<std::string>(&config_file_name)->default_value("wav2json.cfg"), 
          		"config file to use")
    		("db-scale,d", po::value(&use_db_scale)->zero_tokens()->default_value(false),
    			"use logarithmic (e.g. decibel) scale instead of linear scale")
    		("db-min", po::value(&db_min)->default_value(-48.0f),
    			"minimum value of the signal in dB, that will be visible in the waveform")
		    ("db-max", po::value(&db_max)->default_value(0.0f),
    			"maximum value of the signal in dB, that will be visible in the waveform. "
    			"Usefull, if you now, that your signal peaks at a certain level.")
    	;

    	po::options_description hidden("Hidden options");
			hidden.add_options()
    		("input-file", po::value< std::string >(&input_file_name), "input file")
    	;
    
		po::positional_options_description p;
		p.add("input-file", -1);


		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::variables_map vm;

		bool parse_error = false;
		try
		{
			po::store(po::command_line_parser(argc, argv).
		    	options(cmdline_options).positional(p).run(), vm);
			po::notify(vm);

			// try to read stuff from config file
			try
			{
				po::store(po::parse_config_file<char>(config_file_name.c_str(),config_file_options),vm);
				po::notify(vm);
			} catch(po::reading_file& e)
			{
				// notify of error, if the cfg is not the default one
				if ( config_file_name != "wav2json.cfg" )
				{
					std::cerr << "Error: " << e.what() << std::endl;
					parse_error = true;
				}
			}

			//but options from command line override the ones from the config file, so we just parse them again
			po::store(po::command_line_parser(argc, argv).
		    	options(cmdline_options).positional(p).run(), vm);
			po::notify(vm);
		} catch( std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
			parse_error = true;
		}

		if (input_file_name.empty())
		{
			std::cerr << "Error: no input file supplied." << std::endl;
			parse_error = true;
		}

		if (output_file_name.empty())
			output_file_name = input_file_name + ".png";

		if (samples == 0)
		{
			std::cerr << "Error: samples cannot be 0." << std::endl;
			parse_error = true;
		}

		if (parse_error || vm.count("help") )
		{
			std::cout << "wav2json version " << version::version << std::endl;
			std::cout << "written by Benjamin Schulz (beschulz[the a with the circle]betabugs.de)" << std::endl;
			std::cout << std::endl;
			std::cout << "usage: wav2json [options] input_file_name" << std::endl;
			std::cout << "example: wav2json my_file.wav" << std::endl;
			std::cout << std::endl;
			std::cout << visible << std::endl;
			exit(0);
		}

		if (vm.count("version"))
		{
			std::cout << "version: " << version::version << std::endl;
			std::cout << "build date: " << version::date << std::endl;
			exit(0);
		}
	}

	unsigned samples;

	enum Channel
	{
		LEFT,
		RIGHT,
		MID,
		SIDE,
		MIN,
		MAX
	};

	typedef std::vector<Channel> Channels;
	Channels channels;

	std::string output_file_name;
	std::string input_file_name;
	std::string config_file_name;

	bool use_db_scale;
	float db_min;
	float db_max;
};

inline std::istream& operator>>(std::istream& in, Options::Channel& channel)
{
    std::string token;
    in >> token;
         if (token == "left") channel = Options::LEFT;
    else if (token == "right") channel = Options::RIGHT;
    else if (token == "mid") channel = Options::MID;
    else if (token == "side") channel = Options::SIDE;
    else if (token == "min") channel = Options::MIN;
    else if (token == "max") channel = Options::MAX;
    //else throw boost::program_options::validation_error("Invalid channel");

    return in;
}

inline std::ostream& operator<<(std::ostream& out, const Options::Channel& channel)
{
	switch(channel)
	{
		case Options::LEFT : out << "left"; break;
		case Options::RIGHT: out << "right"; break;
		case Options::MID  : out << "mid"; break;
		case Options::SIDE : out << "side"; break;
		case Options::MIN  : out << "min"; break;
		case Options::MAX  : out << "max"; break;
		default : out << "unknown"; break;
	}

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const Options::Channels& channels)
{
	for(Options::Channels::const_iterator i = channels.begin(); i != channels.end(); ++i)
	{
		out << *i << " ";
	}

    return out;
}


#endif /* OPTIONS_HPP__ */

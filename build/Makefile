######

UNAME := $(shell uname)
BINARY=../bin/$(UNAME)/wav2json
SRC=../src

ifeq ($(UNAME), Linux)
LD_PLATFORM_FLAGS=-lboost_program_options
CC=g++
endif
ifeq ($(UNAME), Darwin)
CC=clang++
LD_PLATFORM_FLAGS=\
	/usr/local/lib/libboost_program_options-mt.a \
	/usr/local/lib/libsndfile.a \
	/usr/local/lib/libogg.a \
	/usr/local/lib/libvorbis.a \
	/usr/local/lib/libvorbisenc.a \
	/usr/local/lib/libFLAC.a

INCLUDES=\
	-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/X11/include/
endif

all: $(BINARY)

$(SRC)/version.hpp: Makefile version.txt
	echo "#ifndef VERSION_HPP__" > $(SRC)/version.hpp
	echo "#define VERSION_HPP__" >> $(SRC)/version.hpp
	echo "namespace {" >> $(SRC)/version.hpp
	echo "	namespace version {" >> $(SRC)/version.hpp
	echo "		static const std::string date=\"`date`\";" >> $(SRC)/version.hpp
	echo "		static const std::string platform=\"$(UNAME)\";" >> $(SRC)/version.hpp
	echo "		static const std::string version=\"`cat version.txt`\";" >> $(SRC)/version.hpp
	echo "	}; /* namespace version */" >> $(SRC)/version.hpp
	echo "} /* anonymous namespace */" >> $(SRC)/version.hpp
	echo "#endif /* VERSION_HPP__ */" >> $(SRC)/version.hpp

$(BINARY): $(SRC)/*.cpp $(SRC)/*.hpp $(SRC)/version.hpp
	mkdir -p `dirname $(BINARY)`
	$(CC) -O3 -Wall -Werror $(SRC)/*.cpp $(INCLUDES) $(LD_PLATFORM_FLAGS) -o $(BINARY)

../examples/%.json : ../example_data/%.wav
	$(BINARY) $+ --precision 2 --channels left right mid side min max -o $@

TEST_JSONS=$(patsubst ../example_data/%.wav,../examples/%.json,$(wildcard ../example_data/*.wav))

tests: $(BINARY) $(TEST_JSONS)

clean:
	rm -f $(BINARY)
	rm -f gmon.out
	rm -f $(SRC)/version.hpp
	rm -f ../examples/*.json

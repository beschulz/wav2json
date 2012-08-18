#!/bin/sh

#this is required, because the json files cannot be loaded via file://
cd ../examples/
python -m SimpleHTTPServer
cd -

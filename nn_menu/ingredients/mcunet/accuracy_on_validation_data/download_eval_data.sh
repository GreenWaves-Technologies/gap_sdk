#!/bin/sh

echo "Downloading data ..."
curl --progress-bar https://gwt-website-files.s3.eu-central-1.amazonaws.com/nn_menu/imagenet_gap.zip --output imagenet_gap.zip
echo "Done!"
unzip imagenet_gap.zip
rm imagenet_gap.zip
#!/bin/sh

echo "Downloading data ..."
curl --progress-bar https://gwt-website-files.s3.eu-central-1.amazonaws.com/nn_menu/mcunet_quant_data_ppm.zip --output mcunet_quant_data_ppm.zip
echo "Done!"
unzip mcunet_quant_data_ppm.zip
rm mcunet_quant_data_ppm.zip
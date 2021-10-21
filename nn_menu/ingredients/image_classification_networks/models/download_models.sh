####################################### MOBILENET V1 ##############################################

######### Models 1.0 ########
wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz
tar -xvf mobilenet_v1_1.0_224_quant.tgz
rm mobilenet_v1_1.0_224_quant.ckpt.*
rm mobilenet_v1_1.0_224_quant_*
rm mobilenet_v1_1.0_224_quant.tgz
mv mobilenet_v1_1.0_224_quant.tflite models/tflite_models/mobilenet_v1_1_0_224_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_192_quant.tgz
tar -xvf mobilenet_v1_1.0_192_quant.tgz
rm mobilenet_v1_1.0_192_quant.ckpt.*
rm mobilenet_v1_1.0_192_quant_*
rm mobilenet_v1_1.0_192_quant.tgz
mv mobilenet_v1_1.0_192_quant.tflite models/tflite_models/mobilenet_v1_1_0_192_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_160_quant.tgz
tar -xvf mobilenet_v1_1.0_160_quant.tgz
rm mobilenet_v1_1.0_160_quant.ckpt.*
rm mobilenet_v1_1.0_160_quant_*
rm mobilenet_v1_1.0_160_quant.tgz
mv mobilenet_v1_1.0_160_quant.tflite models/tflite_models/mobilenet_v1_1_0_160_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_128_quant.tgz
tar -xvf mobilenet_v1_1.0_128_quant.tgz
rm mobilenet_v1_1.0_128_quant.ckpt.*
rm mobilenet_v1_1.0_128_quant_*
rm mobilenet_v1_1.0_128_quant.tgz
mv mobilenet_v1_1.0_128_quant.tflite models/tflite_models/mobilenet_v1_1_0_128_quant.tflite

######### Models 0.75 ########
wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_224_quant.tgz
tar -xvf mobilenet_v1_0.75_224_quant.tgz
rm mobilenet_v1_0.75_224_quant.ckpt.*
rm mobilenet_v1_0.75_224_quant_*
rm mobilenet_v1_0.75_224_quant.tgz
mv mobilenet_v1_0.75_224_quant.tflite models/tflite_models/mobilenet_v1_0_75_224_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_192_quant.tgz
tar -xvf mobilenet_v1_0.75_192_quant.tgz
rm mobilenet_v1_0.75_192_quant.ckpt.*
rm mobilenet_v1_0.75_192_quant_*
rm mobilenet_v1_0.75_192_quant.tgz
mv mobilenet_v1_0.75_192_quant.tflite models/tflite_models/mobilenet_v1_0_75_192_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_160_quant.tgz
tar -xvf mobilenet_v1_0.75_160_quant.tgz
rm mobilenet_v1_0.75_160_quant.ckpt.*
rm mobilenet_v1_0.75_160_quant_*
rm mobilenet_v1_0.75_160_quant.tgz
mv mobilenet_v1_0.75_160_quant.tflite models/tflite_models/mobilenet_v1_0_75_160_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.75_128_quant.tgz
tar -xvf mobilenet_v1_0.75_128_quant.tgz
rm mobilenet_v1_0.75_128_quant.ckpt.*
rm mobilenet_v1_0.75_128_quant_*
rm mobilenet_v1_0.75_128_quant.tgz
mv mobilenet_v1_0.75_128_quant.tflite models/tflite_models/mobilenet_v1_0_75_128_quant.tflite

######### Models 0.5 ########
wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_224_quant.tgz
tar -xvf mobilenet_v1_0.5_224_quant.tgz
rm mobilenet_v1_0.5_224_quant.ckpt.*
rm mobilenet_v1_0.5_224_quant_*
rm mobilenet_v1_0.5_224_quant.tgz
mv mobilenet_v1_0.5_224_quant.tflite models/tflite_models/mobilenet_v1_0_5_224_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_192_quant.tgz
tar -xvf mobilenet_v1_0.5_192_quant.tgz
rm mobilenet_v1_0.5_192_quant.ckpt.*
rm mobilenet_v1_0.5_192_quant_*
rm mobilenet_v1_0.5_192_quant.tgz
mv mobilenet_v1_0.5_192_quant.tflite models/tflite_models/mobilenet_v1_0_5_192_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_160_quant.tgz
tar -xvf mobilenet_v1_0.5_160_quant.tgz
rm mobilenet_v1_0.5_160_quant.ckpt.*
rm mobilenet_v1_0.5_160_quant_*
rm mobilenet_v1_0.5_160_quant.tgz
mv mobilenet_v1_0.5_160_quant.tflite models/tflite_models/mobilenet_v1_0_5_160_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.5_128_quant.tgz
tar -xvf mobilenet_v1_0.5_128_quant.tgz
rm mobilenet_v1_0.5_128_quant.ckpt.*
rm mobilenet_v1_0.5_128_quant_*
rm mobilenet_v1_0.5_128_quant.tgz
mv mobilenet_v1_0.5_128_quant.tflite models/tflite_models/mobilenet_v1_0_5_128_quant.tflite

######### Models 0.25 ########
wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_224_quant.tgz
tar -xvf mobilenet_v1_0.25_224_quant.tgz
rm mobilenet_v1_0.25_224_quant.ckpt.*
rm mobilenet_v1_0.25_224_quant_*
rm mobilenet_v1_0.25_224_quant.tgz
mv mobilenet_v1_0.25_224_quant.tflite models/tflite_models/mobilenet_v1_0_25_224_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_192_quant.tgz
tar -xvf mobilenet_v1_0.25_192_quant.tgz
rm mobilenet_v1_0.25_192_quant.ckpt.*
rm mobilenet_v1_0.25_192_quant_*
rm mobilenet_v1_0.25_192_quant.tgz
mv mobilenet_v1_0.25_192_quant.tflite models/tflite_models/mobilenet_v1_0_25_192_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_160_quant.tgz
tar -xvf mobilenet_v1_0.25_160_quant.tgz
rm mobilenet_v1_0.25_160_quant.ckpt.*
rm mobilenet_v1_0.25_160_quant_*
rm mobilenet_v1_0.25_160_quant.tgz
mv mobilenet_v1_0.25_160_quant.tflite models/tflite_models/mobilenet_v1_0_25_160_quant.tflite

wget http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_0.25_128_quant.tgz
tar -xvf mobilenet_v1_0.25_128_quant.tgz
rm mobilenet_v1_0.25_128_quant.ckpt.*
rm mobilenet_v1_0.25_128_quant_*
rm mobilenet_v1_0.25_128_quant.tgz
mv mobilenet_v1_0.25_128_quant.tflite models/tflite_models/mobilenet_v1_0_25_128_quant.tflite

####################################### MOBILENET V2 ##############################################

####  float_v2_1.4_224   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_140.tgz
tar -xvf quantized_v2_224_140.tgz
mv v2_224_140/model.tflite models/tflite_models/mobilenet_v2_1_4_224_quant.tflite
rm -rf v2_224_140/
rm -rf quantized_v2_224_140.tgz

####  float_v2_1.0_224   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_100.tgz
tar -xvf quantized_v2_224_100.tgz
mv v2_224_100/model.tflite models/tflite_models/mobilenet_v2_1_0_224_quant.tflite
rm -rf v2_224_100/
rm -rf quantized_v2_224_100.tgz

####  float_v2_1.0_192   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_100.tgz
tar -xvf quantized_v2_192_100.tgz
mv v2_192_100/model.tflite models/tflite_models/mobilenet_v2_1_0_192_quant.tflite
rm -rf v2_192_100/
rm -rf quantized_v2_192_100.tgz

####  float_v2_1.0_160   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_100.tgz
tar -xvf quantized_v2_160_100.tgz
mv v2_160_100/model.tflite models/tflite_models/mobilenet_v2_1_0_160_quant.tflite
rm -rf v2_160_100/
rm -rf quantized_v2_160_100.tgz

####  float_v2_1.0_128   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_100.tgz
tar -xvf quantized_v2_128_100.tgz
mv v2_128_100/model.tflite models/tflite_models/mobilenet_v2_1_0_128_quant.tflite
rm -rf v2_128_100/
rm -rf quantized_v2_128_100.tgz

####  float_v2_1.0_96 	##
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_100.tgz
tar -xvf quantized_v2_96_100.tgz
mv v2_96_100/model.tflite models/tflite_models/mobilenet_v2_1_0_96_quant.tflite
rm -rf v2_96_100/
rm -rf quantized_v2_96_100.tgz
​
####  float_v2_0.75_224  ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_75.tgz
tar -xvf quantized_v2_224_75.tgz
mv v2_224_75/model.tflite models/tflite_models/mobilenet_v2_0_75_224_quant.tflite
rm -rf v2_224_75/
rm -rf quantized_v2_224_75.tgz

####  float_v2_0.75_192  ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_75.tgz
tar -xvf quantized_v2_192_75.tgz
mv v2_192_75/model.tflite models/tflite_models/mobilenet_v2_0_75_192_quant.tflite
rm -rf v2_192_75/
rm -rf quantized_v2_192_75.tgz

####  float_v2_0.75_160  ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_75.tgz
tar -xvf quantized_v2_160_75.tgz
mv v2_160_75/model.tflite models/tflite_models/mobilenet_v2_0_75_160_quant.tflite
rm -rf v2_160_75/
rm -rf quantized_v2_160_75.tgz

####  float_v2_0.75_128  ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_75.tgz
tar -xvf quantized_v2_128_75.tgz
mv v2_128_75/model.tflite models/tflite_models/mobilenet_v2_0_75_128_quant.tflite
rm -rf v2_128_75/
rm -rf quantized_v2_128_75.tgz

####  float_v2_0.75_96   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_75.tgz
tar -xvf quantized_v2_96_75.tgz
mv v2_96_75/model.tflite models/tflite_models/mobilenet_v2_0_75_96_quant.tflite
rm -rf v2_96_75/
rm -rf quantized_v2_96_75.tgz
​
####  float_v2_0.5_224   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_224_50.tgz
tar -xvf quantized_v2_224_50.tgz
mv v2_224_50/model.tflite models/tflite_models/mobilenet_v2_0_5_224_quant.tflite
rm -rf v2_224_50/
rm -rf quantized_v2_224_50.tgz

####  float_v2_0.5_192   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_192_50.tgz
tar -xvf quantized_v2_192_50.tgz
mv v2_192_50/model.tflite models/tflite_models/mobilenet_v2_0_5_192_quant.tflite
rm -rf v2_192_50/
rm -rf quantized_v2_192_50.tgz

####  float_v2_0.5_160   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_160_50.tgz
tar -xvf quantized_v2_160_50.tgz
mv v2_160_50/model.tflite models/tflite_models/mobilenet_v2_0_5_160_quant.tflite
rm -rf v2_160_50/
rm -rf quantized_v2_160_50.tgz

####  float_v2_0.5_128   ##	
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_128_50.tgz
tar -xvf quantized_v2_128_50.tgz
mv v2_128_50/model.tflite models/tflite_models/mobilenet_v2_0_5_128_quant.tflite
rm -rf v2_128_50/
rm -rf quantized_v2_128_50.tgz

####  float_v2_0.5_96 	##
wget https://storage.googleapis.com/mobilenet_v2/checkpoints/quantized_v2_96_50.tgz
tar -xvf quantized_v2_96_50.tgz
mv v2_96_50/model.tflite models/tflite_models/mobilenet_v2_0_5_96_quant.tflite
rm -rf v2_96_50/
rm -rf quantized_v2_96_50.tgz
​
################################### MOBILENET V3 minimalistic ########################################

wget https://storage.googleapis.com/mobilenet_v3/checkpoints/v3-large-minimalistic_224_1.0_uint8.tgz
tar -xvf v3-large-minimalistic_224_1.0_uint8.tgz
mv v3-large-minimalistic_224_1.0_uint8/v3-large-minimalistic_224_1.0_uint8.tflite models/tflite_models/mobilenet_v3_large_minimalistic_1_0_224_quant.tflite
rm -rf v3-large-minimalistic_224_1.0_uint8/
rm -rf v3-large-minimalistic_224_1.0_uint8.tgz

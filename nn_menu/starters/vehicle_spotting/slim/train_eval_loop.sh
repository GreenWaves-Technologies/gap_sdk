PYT=python3
CUDA="2"
NUM_CLONES=1

while getopts w:m:e:b:l:q:c:s:g:i: option
do
	case "${option}"
	in
	w) WIDTH=${OPTARG};;
	m) MODEL=${OPTARG};;
	b) BATCH_SIZE=${OPTARG};;
	e) NUM_EPOCHS=${OPTARG};;
	l) LR=${OPTARG};;
	q) QUANT_DELAY=${OPTARG};;
	c) FROM_CKPT=${OPTARG};;
	s) START_EPOCH=${OPTARG};;
	g) USE_GRAYSCALE=" --use_grayscale";;
	i) IMAGE_SIZE=${OPTARG};;
	esac
done

#MODEL
MODEL=${MODEL:="mobilenet_v1"}
WIDTH=${WIDTH:=}
if [ -z ${WIDTH} ]
then
	WIDTH=1
else
	MODEL+="_${WIDTH}"
fi

#IMAGE_SIZE
IMAGE_SIZE=${IMAGE_SIZE:=224}

#DATASET
DATASET="visualwakewords"
DATASET_DIR="visualwakewords_vehicle"
VAL_SPLIT="val"

DATASET_FLAGS="--dataset_name=${DATASET} --dataset_dir=${DATASET_DIR}"

TRAIN_DIR="${MODEL}/${DATASET}/train_dir"

EVAL_DIR="${TRAIN_DIR}/eval"

#TRAIN HYPERPARAMETERS
NUM_EPOCHS=${NUM_EPOCHS:=100}
START_EPOCH=${START_EPOCH:=1}
BATCH_SIZE=${BATCH_SIZE:=96}
LR=${LR:=0.045}
DS_TRAIN_DIM=82783
#DS_TRAIN_DIM=1200000
TRAIN_FLAGS="--learning_rate=${LR} --batch_size=${BATCH_SIZE} --num_clones=${NUM_CLONES}"

#QUANTIZATION
QUANT_DELAY=${QUANT_DELAY:=-1}
QUANT=""
if [ ${QUANT_DELAY} -gt -1 ]
then
	QUANT=" --quantize"
	TRAIN_DIR+="_quant_sym"
fi

RED='\033[0;31m'
NC='\033[0m'
NUM_STEPS=0
for ((i=${START_EPOCH}; i<=${NUM_EPOCHS}; i++))
do
	let NUM_STEPS=$((i*${DS_TRAIN_DIM}/${BATCH_SIZE}))
	#echo ${TRAIN_FLAGS}
	CUDA_VISIBLE_DEVICES=${CUDA} ${PYT} -W ignore train_image_classifier.py --train_dir=${TRAIN_DIR} ${TRAIN_FLAGS} --train_image_size=${IMAGE_SIZE} --max_number_of_steps=${NUM_STEPS} ${DATASET_FLAGS} --dataset_split_name="train"  --model_name=${MODEL} ${USE_GRAYSCALE} --preprocessing_name=mobilenet_v1 --quantize_delay=${QUANT_DELAY} ${FROM_CKPT}
	#echo -e "${RED}${FROM_CKPT}${NC}"
	CUDA_VISIBLE_DEVICES=${CUDA} ${PYT} -W ignore eval_image_classifier.py --eval_dir=${EVAL_DIR} --eval_image_size=${IMAGE_SIZE} --checkpoint_path=${TRAIN_DIR} ${DATASET_FLAGS} --dataset_split_name=${VAL_SPLIT} --model_name=${MODEL} ${USE_GRAYSCALE} --preprocessing_name=mobilenet_v1 ${QUANT}
done

#${PYT} export_inference_graph.py --model_name=${MODEL} --output_file="${MODEL}/${MODEL}_inference.pb" --input_size=224 --use_grayscale --dataset_name=${DATASET}

#freeze_graph --input_graph="${MODEL}/${MODEL}_inference.pb" --output_graph="${MODEL}/${DATASET_DIR}/frozen_graph.pb" --input_binary --input_checkpoint=${TRAIN_DIR} --output_node_names="MobilenetV1/Predictions/Reshape_1"

#tflite_convert --graph_def_file="${MODEL}/${DATASET_DIR}/frozen_graph.pb" --output_file="${MODEL}/${DATASET_DIR}/${MODEL}_grayscale_vwwvehicle.tflite" --input_arrays="input" --output_arrays="MobilenetV1/Predictions/Reshape_1"

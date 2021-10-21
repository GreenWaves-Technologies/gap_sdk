path_to_coco="/home/fariselli/models/research/slim/coco/"


git clone git@github.com:Mxbonn/visualwakewords.git
cd visualwakewords
mkdir annotations
TRAIN_ANNOTATIONS_FILE="annotations/instances_train2014.json"
VAL_ANNOTATIONS_FILE="annotations/instances_val2014.json"
DIR="annotations/"
python scripts/create_coco_train_minival_split.py \
  --train_annotations_file="${path_to_coco}${TRAIN_ANNOTATIONS_FILE}" \
  --val_annotations_file="${path_to_coco}${VAL_ANNOTATIONS_FILE}" \
  --output_dir="${DIR}"\

MAXITRAIN_ANNOTATIONS_FILE="annotations/instances_maxitrain.json"
MINIVAL_ANNOTATIONS_FILE="annotations/instances_minival.json"
VWW_OUTPUT_DIR="annotations/"
python scripts/create_visualwakewords_annotations.py \
  --train_annotations_file="${MAXITRAIN_ANNOTATIONS_FILE}" \
  --val_annotations_file="${MINIVAL_ANNOTATIONS_FILE}" \
  --output_dir="${VWW_OUTPUT_DIR}" \
  --threshold=0.005 \
  --foreground_class='person'

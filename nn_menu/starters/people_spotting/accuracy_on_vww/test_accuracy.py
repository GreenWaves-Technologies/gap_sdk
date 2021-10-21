import sys
from PIL import Image
import os
import tensorflow as tf
import numpy as np
import argparse

sys.path.append('./visualwakewords')
import pyvww
from pyvww.utils import VisualWakeWords


parser = argparse.ArgumentParser()
parser.add_argument('--tflite_model', type=str, default='../model/visual_wake_quant.tflite', 
                    help='Model to use for VWW.')
parser.add_argument('--GAP', action='store_true', dest='GAP')

parser.add_argument('--coco_dataset_path', type=str, default='', 
                    help='Path to COCO dataset')
parser.add_argument('--vww_ann_file', type=str, default='visualwakewords/annotations/instances_val.json', 
                    help='Path to the VWW annotation file')

args, unparsed = parser.parse_known_args()

print(args.GAP)
if args.coco_dataset_path == '':
    print('Error! Missing path to COCO')
    exit()
    
#print(args.coco_dataset_path, args.vww_ann_file, args.tflite_model)


# Configurable Parameters
ANNOTATION_FILE_PATH=args.vww_ann_file
COCO_PATH=args.coco_dataset_path
TFLITE_MODEL_PATH =args.tflite_model

# Preprocessing Function
def preprocess_image(image, scale_factor=0.875, central_crop=True, W_out=224, H_out=224):
    
    # this simply scale to the output dims
    if scale_factor is None and central_crop is False:
        image = image.resize((W_out, H_out),Image.BILINEAR)
        return image

    # compute resize dimensions based on scaling factor, then crop the center
    new_W = int(W_out / scale_factor)
    new_H = int(H_out / scale_factor)
            
    (orig_W, orig_H) = image.size
    if orig_W < orig_H:
        new_H = int(new_W*orig_H/orig_W)
    else:
        new_W = int(new_H*orig_W/orig_H)  
    
    #rezize
    image = image.resize((new_W, new_H),Image.BILINEAR)

    #if scale_factor is not None:
    # Crop the center of the image
    left = (new_W - W_out)/2
    top = (new_H - H_out)/2
    right = (new_W + W_out)/2
    bottom = (new_H + H_out)/2
    image = image.crop((left, top, right, bottom))
    
    return image

# Read VWW Dataset
vww = VisualWakeWords(ANNOTATION_FILE_PATH)
ids = list(sorted(vww.imgs.keys()))
DATASET_LEN = len(ids)
print(DATASET_LEN)

# Configura the TF Interpreter
interpreter = tf.lite.Interpreter(model_path=TFLITE_MODEL_PATH)
interpreter.allocate_tensors()

# Get input and output tensors of TFLite
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()
print('Input details: ', input_details)
print('Output details: ', output_details)
scale, zero_point = input_details[0]['quantization']
input_shape = input_details[0]['shape']
[nb,h_tflite,w_tflite,c_tflite] = input_shape
#print('The input parameters: ', scale, zero_point, nb,h_tflite,w_tflite,c_tflite)

pred0 = 0
pred1 = 0
class0 = 0
class1 = 0
print('Going to run validation over {} samples'.format(DATASET_LEN))
executable = 'vww_emul'
for index in range(DATASET_LEN):#DATASET_LEN
            
    img_id = ids[index]
    ann_ids = vww.getAnnIds(imgIds=img_id)
    if ann_ids:
        target = vww.loadAnns(ann_ids)[0]['category_id']
    else:
        target = 0

    #read image
    path = vww.loadImgs(img_id)[0]['file_name']
    #print(path)
    img = Image.open(os.path.join(COCO_PATH, path))
    if img.mode == 'L':
        img=img.convert('RGB')
    
    # preprocess
    img = preprocess_image(img, scale_factor=None, central_crop=False, W_out=w_tflite, H_out=h_tflite)
    
    #inference
    if args.GAP == False:
        img = np.array(img)
        #print(img.min(),img.max(),img.shape)
        interpreter.set_tensor(input_details[0]['index'], img.reshape(input_details[0]['shape']))
        interpreter.invoke()
        output = interpreter.get_tensor(output_details[0]['index'])
        predicted_class = np.argmax(output)
    #print(predicted_class)
    else:
        predicted_class = -1
        #print(img.s)
        img = img.transpose(method=Image.FLIP_LEFT_RIGHT)#FLIP_TOP_BOTTOM)
        img.save("test.ppm")
        ex_stream = os.popen("./{} test.ppm".format(executable))
        emul_log = ex_stream.readlines()
        emul_prediction = None
        for line in emul_log:
#            print(line)
            if 'no person seen' in line:
                predicted_class = 0
                break
            elif 'person seen' in line:
                predicted_class = 1
                break               
        
#    print('Prediction:', predicted_class, 'target: ', target)
    
    if target==0:
        class0 += 1
        if predicted_class == 0:
            pred0+=1
    elif target==1:
        class1 += 1
        if predicted_class == 1:
            pred1+=1
    else:
        print('Not recognized')

    accuracy_class0 = pred0/class0 if class0>0 else 0
    accuracy_class1 = pred1/class1 if class1>0 else 0
    avg_accuracy=(pred0+pred1)/(class0+class1)
    if (index > 0) and (index % 100 == 0):
        print('Processed {} of {} samples: test accuracy is {}%'.format(index,DATASET_LEN, avg_accuracy))

# Print final results
print('Class0:{}% ({}/{}) - Class1 {}% ({}/{}) '.format(100*accuracy_class0, pred0,class0,100*accuracy_class1,pred1,class1))
print("The average accuracy is: ", 100*(accuracy_class0+accuracy_class1)/2, '% or' , 100*avg_accuracy, '%' ) 
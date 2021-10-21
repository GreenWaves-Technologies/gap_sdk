import numpy as np
import sys

print("The arguments are: ", str(sys.argv))

if len(sys.argv) > 1:
	NAME_OUT_TENSORS = sys.argv[1]
else:
	NAME_OUT_TENSORS = "nntool"

last_layer_idx = -1 if NAME_OUT_TENSORS == 'nntool' else -3

out_tensor = np.squeeze(tensors[NAME_OUT_TENSORS][last_layer_idx][0])
print(out_tensor.shape)
out_tensor = out_tensor.transpose((1, 0))
out_char_codes = [np.argmax(out_tensor[i]) for i in range(out_tensor.shape[0])]

char_dict = {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9, '<Anhui>': 10, '<Beijing>': 11, '<Chongqing>': 12, '<Fujian>': 13, '<Gansu>': 14, '<Guangdong>': 15, '<Guangxi>': 16, '<Guizhou>': 17, '<Hainan>': 18, '<Hebei>': 19, '<Heilongjiang>': 20, '<Henan>': 21, '<HongKong>': 22, '<Hubei>': 23, '<Hunan>': 24, '<InnerMongolia>': 25, '<Jiangsu>': 26, '<Jiangxi>': 27, '<Jilin>': 28, '<Liaoning>': 29, '<Macau>': 30, '<Ningxia>': 31, '<Qinghai>': 32, '<Shaanxi>': 33, '<Shandong>': 34, '<Shanghai>': 35, '<Shanxi>': 36, '<Sichuan>': 37, '<Tianjin>': 38, '<Tibet>': 39, '<Xinjiang>': 40, '<Yunnan>': 41, '<Zhejiang>': 42, '<police>': 43, 'A': 44, 'B': 45, 'C': 46, 'D': 47, 'E': 48, 'F': 49, 'G': 50, 'H': 51, 'I': 52, 'J': 53, 'K': 54, 'L': 55, 'M': 56, 'N': 57, 'O': 58, 'P': 59, 'Q': 60, 'R': 61, 'S': 62, 'T': 63, 'U': 64, 'V': 65, 'W': 66, 'X': 67, 'Y': 68, 'Z': 69, '_': 70}

out_char = []
for i, char_code in enumerate(out_char_codes):
    if char_code == 70:
        continue
    for k, v in char_dict.items():
        if char_code == v:
            out_char.append(k)
            continue
print(len(out_char_codes))
print(out_char)
print(out_char_codes)

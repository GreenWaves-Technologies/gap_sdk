#!/usr/bin/env python3
import os
import sys
from PIL import Image

def main():
	path = sys.argv[1]
	for file in os.listdir(path):
		filename, ext = os.path.splitext(file)
		if ext not in ['.bmp', '.pgm']:
			continue
		img = Image.open(os.path.join(path, file)).convert('L')
		print(os.path.join(path, filename) + '.ppm')
		img.save(os.path.join(path, filename) + '.ppm')

if __name__ == "__main__":
	main()

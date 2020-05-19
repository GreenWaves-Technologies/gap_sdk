# Copyright 2019 GreenWaves Technologies, SAS
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
'''
import sys
import subprocess
import shlex
import os

INCLUDES = "-I. -I"+TILER_INC+" -I"+TILER_EMU_INC+" -I"+TILER_CNN_GENERATOR_PATH
CC = "gcc"

def compile_and_run_AT_model(model, output_exe="Gentile", output_dir="./", extra_flags=""):
	if not os.path.exists(output_dir):
		os.mkdir(output_dir)
	#compile
	CFLAGS = "-g -o"+ output_dir+"/"+output_exe
	SRCS = TILER_CNN_GENERATOR_PATH+"/CNN_Generators.c " + TILER_LIB
	args = CC + " " + CFLAGS + " " + INCLUDES + " " + model + " " + SRCS
	args = shlex.split(args)
	subprocess.call(args)
	#run
	subprocess.call([output_dir+"/"+output_exe, "-o"+output_dir, "-c"+output_dir, extra_flags], stdin = sys.stdin)

def compile_and_run_test_main(model_name, mainfile, output_exe="emul", output_dir="./", extra_flags=""):
	if not os.path.exists(output_dir):
		os.mkdir(output_dir)

	CC = "gcc "
	CFLAGS = "-g -O0 "
	MODEL_TENSORS = output_dir + "/" + model_name + "_L3_Flash_Const.dat "
	INCLUDES = "-I. -I"+TILER_INC+" -I"+TILER_EMU_INC+" -I"+TILER_CNN_GENERATOR_PATH + " -I" + output_dir + " "
	MODEL_SRCS = output_dir + "/" + mainfile + " "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_BiasReLULinear_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_Conv_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_Conv_DP_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_Conv_DW_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_Conv_DW_DP_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_Pooling_BasicKernels.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_MatAlgebra.c "
	MODEL_SRCS += TILER_CNN_KERNEL_PATH + "/CNN_SoftMax.c "
	MODEL_COMMON_SRC_FILES = "ImgIO.c" 
	#assert os.path.exists(MODEL_COMMON_SRC_FILES), "This test code uses the ImgIO.c functionalities, \
	#												provide the file in the project dir" 
	SRCS = MODEL_SRCS + MODEL_COMMON_SRC_FILES


	BUILD_DIR = output_dir + '/BUILD_EMUL'
	if not os.path.exists(BUILD_DIR):
		os.mkdir(BUILD_DIR)
	OBJS = " "
	for string in SRCS.split(" "):
		OBJS += output_dir + "/BUILD_EMUL/" + string.replace(".c", ".o ")

#	for file in shlex.split(MODEL_SRCS):
#		print(os.path.dirname(output_dir+file))
#		subprocess.call(["mkdir", "-p", os.path.dirname(output_dir + file)])
	args = shlex.split(CC + CFLAGS + "-c " + mainfile + " " + INCLUDES + "-MD -MF" + mainfile.replace(".c", ".d") + " -o" + mainfile.replace(".c", ".o"))
	subprocess.call(args)
	args = shlex.split(CC + CFLAGS + "-MMD -MP " + CFLAGS + INCLUDES + "-o main_test" + MODEL_SRCS + MODEL_COMMON_SRC_FILES)
	#compile
	#subprocess.call(args)

'''


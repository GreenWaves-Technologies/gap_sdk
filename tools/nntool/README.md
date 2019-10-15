# NNTOOL

NNTOOL helps to port NN graphs from various NN training packages to GAP8. It helps with:

- Post training graph quantization
- Graph evaluation for activation and parameter sizing
- AutoTiler CNN model generation

NNTOOL helps in the development phase to select appropriate quantization settings and in the project build phase to
automate the build process using any software build system (such as Make, CMake, etc.)

The tool has a command interpreter mode where it provides an interface to:

- Visualize the graph
- Calculate parameter and activation dynamic statistics
- Test quantization settings
- Transform the graph to fit AutoTiler generators by fusing operators together
- Equalize cross channel variations in weights to improve post quantization accuracy
- Generate AutoTiler model file
- Save a state file with all the transformations done to the graph

The tool also has a command line mode which takes a saved state file and directly genenerates the model and
saves a parameters file; the two elements necessary for a GAP project build.

To set up the tool install the packages in the requirements file

python -m pip install -r requirements.txt

if python3 is your default interpreter or

python3 -m pip install -r requirements.txt

if it is not and type make

make

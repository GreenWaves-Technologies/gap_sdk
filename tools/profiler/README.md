## Requirements
 - GAP SDK : see https://github.com/GreenWaves-Technologies/gap_sdk
 - Qt >= 5.13: see https://doc.qt.io/qt-5/gettingstarted.html

### Requirements on Ubuntu 18
```
sudo apt install qt5-default libqt5charts5-dev
```

## How to build
Simply run `make`

## How to run
To  run the profiler, you first need to run the profiler/init.sh script. <br>
Then, you need to go to your example directory ($HOME/gap-sdk/autotiler/Cifar10 for example) and run the following command:
```
$ make all profiler platform=gvsoc
```
The profiler window opens and you can first see a View Menu in the toolbar. This menu allows the user to select the different Dock Windows he/she wishes to display. Except the Timeline window, which is fixed, all the dock windows can be moved by clicking the mouse on their Title and placed around the timeline window. 

There are  also four buttons in the tool bar to control the Gvsoc process: 
 - Open : starts the Gvsoc process 
 - Close : closes the Gvsoc Process
 - Run : runs the Gvsoc process on your example
 - Pause : pauses the Gvsoc process. It can be restarted by pushing on the "Run" button again
  
**Make sure:**
 - your Makefile compiles your code with the -g option (debugging symbols), in order for the profiler to be able to provide as much information as possible.
 - you sourced the appropriate files that are needed to build your project (as the gap_sdk/sourceme.sh)

## Documentation
You can access to HTML documentation with the following commands:
```
$ make docs
$ <your favourite internet browser> docs/html/index.html &
```

## Known problems
Have a look at the gitlab issue page to get an up-to-date list of known problems.
Please report all the problems you find (or the improvements you'd like to see) there.

## Implementation details
Roughly speaking, this program collects traces from gvsoc, extracts performance relevant data from those, and displays it to the user.
 - The *backend* part is responsible for the communication with gvsoc (using a fifo) and for the extraction of relevant data. It is compiled as a static library.
 - The *frontend* part queries the backend, and is responsible for the display.

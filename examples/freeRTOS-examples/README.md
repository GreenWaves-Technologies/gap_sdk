FreeRTOS
========

FreeRTOS examples :
-----------------

- test_os : FreeRTOS API examples
- test_driver : Drivers examples
- test_features : GAP8 Feature_Cluster examples
- test_autotiler : Autotiler examples

To test on gapuino :
```bash
user@computer:demos/gwt/gap8/common/application_code$ cd PATH/TO/TEST
user@computer:demos/gwt/gap8/common/application_code/PATH/TO/TEST$ make [clean] all run
```
For Autotiler tests :
```bash
user@computer:demos/gwt/gap8/common/application_code/test_autotiler/Cifar10$ make clean all run
```

Jenkins tests :
```bash
user@computer:demos/gwt/gap8/common/application_code$ make clean gapuino_compile_and_run_test
```

API Rference
------------
https://www.freertos.org/a00106.html
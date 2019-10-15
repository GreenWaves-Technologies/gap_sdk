# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

class Variable():
    pass

class LocalVariable(Variable):
    pass

class GlobalVariable(Variable):
    pass

class InputVariable(GlobalVariable):
    pass

class OutputVariable(GlobalVariable):
    pass

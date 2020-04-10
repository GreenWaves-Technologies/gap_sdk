# pylint: disable=line-too-long, pointless-string-statement, unused-variable

from .templet import stringfunction
from .code_generator import CodeGenerator
from .naming_convension import DefaultNamingConvension

@stringfunction
# pylint: disable=unused-argument
def generator_template(G, gen):
    '''
#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"
${gen.extra_includes_generator(indent=0)}

void ${gen.project_name}Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    KernelOper_T Cop = KOP_CONV;

    // SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);
    SetSymbolDynamics();

    SetUsedFilesNames(0, 2, "CNN_BasicKernels.h", "${gen.project_name}.h");
    SetGeneratedFilesNames("${gen.project_name}Kernels.c", "${gen.project_name}Kernels.h");
${gen.options_generator(indent=1)}

${gen.memory_device_generator(indent=1)}

    LoadCNNLibrary();

${gen.kernel_generator(indent=1)}

#define GRAPH
#ifdef GRAPH
    CreateGraph("${gen.project_name}CNN",
        /* Arguments either passed or globals */
${gen.global_generator(indent=2)},
        /* Locals, allocated dynamically */
${gen.local_generator(indent=2)}
    );

${gen.stack_generator(indent=1)}

${gen.binding_generator(indent=1)}
    CloseGraph();
#endif
}

int main(int argc, char **argv)

{
    if (TilerParseOptions(argc, argv)) {
            printf("Failed to initialize or incorrect output arguments directory.\\n"); return 1;
    }
    ${gen.project_name}Model(52000, 300*1024, 8*1024*1024, 20*1024*1024);
    GenerateTilingCode();
    return 0;
}

'''

@stringfunction
# pylint: disable=unused-argument
def generator_template_v3(G, gen):
    '''
#include <stdint.h>
#include <stdio.h>
#include "AutoTilerLib.h"
#include "CNN_Generators.h"
${gen.extra_includes_generator(indent=0)}

void ${gen.project_name}Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    KernelOper_T Cop = KOP_CONV;

    // SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);
    SetSymbolDynamics();

    SetUsedFilesNames(0, 2, "CNN_BasicKernels.h", "${gen.project_name}.h");
    SetGeneratedFilesNames("${gen.project_name}Kernels.c", "${gen.project_name}Kernels.h");
${gen.options_generator(indent=1)}

${gen.memory_device_generator(indent=1)}

    LoadCNNLibrary();

${gen.kernel_generator(indent=1)}

#define GRAPH
#ifdef GRAPH
    CreateGraph("${gen.project_name}CNN",
        /* Arguments either passed or globals */
${gen.global_generator(indent=2)},
        /* Locals, allocated dynamically */
${gen.local_generator(indent=2)}
    );

    /* Stacked tensors - Concats */
${gen.stack_generator(indent=1)}

${gen.binding_generator(indent=1)}
    CloseGraph();
#endif
}

int main(int argc, char **argv)

{
    if (TilerParseOptions(argc, argv)) {
            printf("Failed to initialize or incorrect output arguments directory.\\n"); return 1;
    }
    ${gen.project_name}Model(52000, 300*1024, 8*1024*1024, 20*1024*1024);
    GenerateTilingCode();
    return 0;
}

'''

def execute_template(template_function, G, naming_convension=None, code_generator=None):
    if code_generator is None:
        if naming_convension is None:
            naming_convension = DefaultNamingConvension(G)
        code_generator = CodeGenerator(G, naming_convension)
    return template_function(G, code_generator)

def default_template(G, naming_convension=None, code_generator=None):
    if code_generator.opts['at_ver'] < 3:
        return execute_template(generator_template, G, naming_convension, code_generator)

    return execute_template(generator_template_v3, G, naming_convension, code_generator)

def dynamic_template(template_file):
    with open(template_file) as fp:
        template = fp.read()

# pylint: disable=unused-argument
    def func(G, gen):
        pass
    # set the function doc string dynamically and call the decorator function directly
    func.__doc__ = template

    def template_function(G, naming_convension=None, code_generator=None):
        return execute_template(stringfunction(func), G, naming_convension, code_generator)

    return template_function

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
#include ${gen.cnn_generators()}
${gen.extra_includes_generator(indent=0)}

void ${gen.project_name}Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    KernelOper_T Cop = KOP_CONV;

    // SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);
    SetSymbolDynamics();

    SetUsedFilesNames(0, 3, ${gen.extra_includes_kernels(indent=0)}, ${gen.cnn_kernels()}, "${gen.project_name}.h");
    SetGeneratedFilesNames("${gen.project_name}Kernels.c", "${gen.project_name}Kernels.h");
${gen.options_generator(indent=1)}

${gen.memory_device_generator(indent=1)}

${gen.load_basic_kernel_library(indent=1)}

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
    ${gen.project_name}Model(${gen.opts['l1_size']}, ${gen.opts['l2_size']}, ${gen.opts['l3_size']}, 20*1024*1024);
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
${gen.cnn_includes_generator()}
${gen.extra_includes_generator(indent=0)}

${gen.expressions_kernel_load_generator(indent=0)}

${gen.expressions_user_kernel_source_generator(indent=0)}

void ${gen.project_name}Model(unsigned int L1Memory, unsigned int L2Memory, unsigned int L3Memory, unsigned int L3Flash)
{
    KernelOper_T Cop = KOP_CONV;

    // SetKernelOpts(KER_OPT_NONE, KER_OPT_BUFFER_PROMOTE);
    SetSymbolDynamics();

${gen.used_filenames(indent=1)}
    SetGeneratedFilesNames("${gen.project_name}Kernels.c", "${gen.project_name}Kernels.h");
${gen.options_generator(indent=1)}

${gen.memory_device_generator(indent=1)}

${gen.load_basic_kernel_library(indent=1)}
${gen.load_expressions_library(indent=1)}

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
    ${gen.project_name}Model(${gen.opts['l1_size']}, ${gen.opts['l2_size']}, ${gen.opts['l3_size']}, ${gen.opts['l3_flash_mb']}*1024*1024);
    GenerateTilingCode();
    return 0;
}

'''

@stringfunction
# pylint: disable=unused-argument
def generator_template_header(G, gen):
    '''
#ifndef ${gen.project_name.upper()}_GRAPHINFO_H
#define ${gen.project_name.upper()}_GRAPHINFO_H
// Quantized scales can be used round_norm(val * QSCALE, QNORM) giving the real value in Q8

${gen.header_generator(indent=0)}
#endif //${gen.project_name.upper()}_GRAPHINFO_H
'''

@stringfunction
def generator_expressions_basic_kernel_header(G, gen):
    '''
#ifndef ${gen.project_name.upper()}_BASIC_KERNELS_H
#define ${gen.project_name.upper()}_BASIC_KERNELS_H
#include "Gap.h"
#include "math_funcs.h"
${gen.expressions_kernel_includes_generator()}

${gen.expressions_kernel_types_generator()}

${gen.expressions_kernel_prototypes_generator()}

#endif // ${gen.project_name.upper()}_BASIC_KERNELS_H
'''

@stringfunction
def generator_expressions_basic_kernel_source(G, gen):
    '''
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
${gen.expressions_kernel_header_includes_generator(indent=0)}

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

#ifndef AT_NORM
#define AT_NORM(x, n)   gap_roundnorm_reg((x), (n))
#endif
#define ATLShift(x, n)  ((x) << (n))

${gen.expressions_kernel_source_generator()}

#pragma GCC diagnostic pop
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

def header_template(G, naming_convension=None, code_generator=None):
    return execute_template(generator_template_header, G, naming_convension, code_generator)

def basic_kernel_source_template(G, naming_convension=None, code_generator=None):
    return execute_template(generator_expressions_basic_kernel_source, G, naming_convension, code_generator)

def basic_kernel_header_template(G, naming_convension=None, code_generator=None):
    return execute_template(generator_expressions_basic_kernel_header, G, naming_convension, code_generator)


def dynamic_template_string(template):
# pylint: disable=unused-argument
    def func(G, gen):
        pass
    # set the function doc string dynamically and call the decorator function directly
    func.__doc__ = template

    def template_function(G, naming_convension=None, code_generator=None):
        return execute_template(stringfunction(func), G, naming_convension, code_generator)

    return template_function

def dynamic_template(template_file):
    with open(template_file) as fp:
        template = fp.read()
    return dynamic_template_string(template)

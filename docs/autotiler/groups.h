/** @defgroup groupAutoTiler AutoTiler

        This is the API documentation of the GAP8 AutoTiler

        GAP8 AutoTiler is a tool that let you focus on writing basic kernels for tasks executed on the GAP8 cluster
        assuming that there are no memory constraints.

        These basic kernels are then used by a high level model where the following information is captured:

        + The structure of a multi dimensional iteration space
        + The exact nature of each argument (direction, location, ...)
        + The exact dimensions of each argument
        + Which subset of the overall iteration space a given argument traverses
        + Different types of constraints

        Taking into account all these constraints the GAP8 AutoTiler generates a tiled structure that fits into the memory budget it has
        been given in the shared L1 memory. This tiled structure is then used to bring data in and out from/to L2 or external memory
        in a pipelined way in order to keep cores busy. AutoTiler inserts calls to basic kernels on the tiled arguments at the requested
        locations in the iteration.

        The AutoTiler helps you to compose basic kernels and organize memory transfers across memory hierarchy in an optimal way without
        having to manage complex double/triple buffering. The GAP8 AutoTiler helps you write application code generators where
        basic kernels are put together and sized to the exact dimensions of the problem you want to solve.

        A number of interesting generators and kernels are included for CNN, FIR filters, FFT, HOG. Matrix Operations, etc
*/

/** @defgroup groupGenerators Generators

AutoTiler based generators
*/

/** @defgroup groupFFT2D FFT2DGenerator

@ingroup groupGenerators

AutoTiler based FFT2D generator
*/

/** @defgroup groupCNN CNNGenerator

@ingroup groupGenerators

AutoTiler based generators for CNN layers
*/

/** @defgroup groupIntegralImage IntegralImageGenerator

@ingroup groupGenerators

AutoTiler based generators for Integral Image
*/

/** @defgroup groupResize ResizeGenerator

@ingroup groupGenerators

AutoTiler based generators for BilinearResize
*/

/** @defgroup groupHOG HOGGenerator

@ingroup groupGenerators

An AutoTiler based Histogram of Gradients (HOG) generator
*/

/** @defgroup groupFIR FIRGenerator

@ingroup groupGenerators

An AutoTiler based Q15 FIR filter generator
*/



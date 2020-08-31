
import logging
from cmd2 import Cmd, Settable
from generation.autotiler_options import DEFAULT_GEN_OPTS, DEFAULT_GEN_OPTS_DESCRIPTIONS
from utils.data_importer import MODES
from .shell_utils import find_choice

LOG = logging.getLogger("nntool")

VALID_LOG_LEVELS = [
    "INFO",
    "DEBUG",
    "WARNING"
]

DEFAULT_OPT_DESCRIPTIONS = {
    'log_level': {'type': str, 'descr': 'set logging level', 'choices': VALID_LOG_LEVELS},
    'load_quantization': {'type': bool, 'descr': 'load TFLITE quantization information', 'choices': [True, False]},
    'load_dequantized': {'type': bool, 'descr': 'load the dequantized constant values from tflite quantized graph',
                         'choices': [True, False]},
    'fusions': {'type': bool, 'descr': 'run standard graph fusions on graph load', 'choices': [True, False]},
    'adjust_order': {'type': bool, 'descr': 'adjust activation and parameter dimension order\
         to match autotiler on graph load', 'choices': [True, False]},
    'weight_equalization': {'type': bool, 'descr': 'equalize weights on graph load', 'choices': [True, False]},
    'equalization_threshold': {'type': float, 'descr': 'threshold for weight equalization convergence'},
    'adjust_image': {'type': bool, 'descr': 'adjust image input size and channels', 'choices': [True, False]},
    'image_width': {'type': int, 'descr': 'input image width'},
    'image_height': {'type': int, 'descr': 'input image height'},
    'image_mode': {'type': str, 'descr': 'input image mode', 'choices': MODES.keys()},
    'input_divisor': {'type': float, 'descr': 'divide input tensor values by this value'},
    'input_offset': {'type': float, 'descr': 'add this value to input tensor values'},
    'input_norm_func': {'type': str, 'descr': 'lambda function in the form x: fn(x) where x is any input'},
    'graph_name': {'type': str, 'descr': 'name of the graph used for code generation'},
    'template_file': {'type': str, 'descr': 'template file used for code generation'},
}


class NNToolShellSettings(Cmd):
    '''
    This class have all the settings and properties that can be set up from the NNToolShell
    To see the Code Generation settings, please refer to generation/autotiler_options.py
    '''
    def __init__(self, *args, **kwargs):
        super(NNToolShellSettings, self).__init__(*args, **kwargs)
        for k, v in DEFAULT_OPT_DESCRIPTIONS.items():
            self.add_settable(Settable(k, v['type'], v['descr'],
                                       choices=v['choices'] if 'choices' in v.keys() else None))
        for k, v in DEFAULT_GEN_OPTS_DESCRIPTIONS.items():
            self.add_settable(Settable(k, v['type'], v['descr'],
                                       choices=v['choices'] if 'choices' in v.keys() else None))
        self.settings = {
            'load_quantization': False,
            'fusions': False,
            'adjust_order': False,
            'weight_equalization': False,
            'equalization_threshold': 0.1,
            'adjust_image': False,
            'image_width': -1,
            'image_height': -1,
            'image_mode': "",
            'image_transpose': False,
            'input_norm_func': "",
            'input_divisor': 1,
            'input_offset': 0,
            'input_shift': 0,
            'log_level': 'INFO',
            'graph_file': "",
            'tensor_file': "",
            'template_file': ""
        }
        self.settings.update(DEFAULT_GEN_OPTS)

    # LOG_LEVEL PROPERTY

    @property
    def log_level(self):
        return self.settings['log_level']

    @log_level.setter
    def log_level(self, val):
        try:
            val = int(val)
            self.settings['log_level'] = val
        except ValueError:
            val = val.upper()
            val = find_choice(VALID_LOG_LEVELS, val)

        self.settings['log_level'] = val
        LOG.setLevel(self.settings['log_level'])
        LOG.info("set log level to %s", val)

    def __getattr__(self, k):
        if k in DEFAULT_GEN_OPTS:
            return self.settings[k]

    def __setattr__(self, k, val):
        if k in DEFAULT_GEN_OPTS:
            self.settings[k] = val
        super(NNToolShellSettings, self).__setattr__(k, val)

    # load_quantization PROPERTY

    @property
    def load_quantization(self):
        return self.settings['load_quantization']

    @load_quantization.setter
    def load_quantization(self, val):
        self.settings['load_quantization'] = bool(val)

    # FUSIONS PROPERTY

    @property
    def fusions(self):
        return self.settings['fusions']

    @fusions.setter
    def fusions(self, val):
        self.settings['fusions'] = bool(val)

    # ADJUST_ORDER PROPERTY

    @property
    def adjust_order(self):
        return self.settings['adjust_order']

    @adjust_order.setter
    def adjust_order(self, val):
        self.settings['adjust_order'] = bool(val)

    # WEIGHT_EQUALIZATION PROPERTY

    @property
    def weight_equalization(self):
        return self.settings['weight_equalization']

    @weight_equalization.setter
    def weight_equalization(self, val):
        self.settings['weight_equalization'] = bool(val)

    # EQUALIZATION_THRESHOLD PROPERTY

    @property
    def equalization_threshold(self):
        return self.settings['equalization_threshold']

    @equalization_threshold.setter
    def equalization_threshold(self, val):
        self.settings['equalization_threshold'] = float(val)

    # IMAGE ADJUSTMENT SETTINGS

    # ADJUST_IMAGE PROPERTY

    @property
    def adjust_image(self):
        return self.settings['adjust_image']

    @adjust_image.setter
    def adjust_image(self, val):
        self.settings['adjust_image'] = bool(val)

    # IMAGE_WIDTH PROPERTY

    @property
    def image_width(self):
        return self.settings['image_width']

    @image_width.setter
    def image_width(self, val):
        try:
            val = int(val)
            if val <= 0:
                raise ValueError()
        except ValueError:
            raise ValueError("value should be positive integer")
        self.settings['image_width'] = bool(val)

    # IMAGE_HEIGHT PROPERTY

    @property
    def image_height(self):
        return self.settings['image_height']

    @image_height.setter
    def image_height(self, val):
        try:
            val = int(val)
            if val <= 0:
                raise ValueError()
        except ValueError:
            raise ValueError("value should be positive integer")
        self.settings['image_height'] = bool(val)

    # IMAGE_MODE PROPERTY

    @property
    def image_mode(self):
        return self.settings['image_mode']

    @image_mode.setter
    def image_mode(self, val):
        val = find_choice(MODES.keys(), val)
        self.settings['image_mode'] = str(val)

    # INPUT_DIVISOR PROPERTY

    @property
    def input_divisor(self):
        return self.settings['input_divisor']

    @input_divisor.setter
    def input_divisor(self, val):
        self.settings['input_divisor'] = int(val)

    # INPUT_NORM_FUNC PROPERTY

    @property
    def input_norm_func(self):
        return self.settings['input_norm_func']

    @input_norm_func.setter
    def input_norm_func(self, val):
        self.settings['input_norm_func'] = str(val)

    # INPUT_OFFSET PROPERTY

    @property
    def input_offset(self):
        return self.settings['input_offset']

    @input_offset.setter
    def input_offset(self, val):
        self.settings['input_offset'] = int(val)

    @property
    def template_file(self):
        return self.settings['template_file']

    @template_file.setter
    def template_file(self, val):
        self.settings['template_file'] = val

    def _get_input_args(self, args):
        res = {}
        if self.settings['adjust_image']:
            res['width'] = self.settings['image_width']
            res['height'] = self.settings['image_height']
            res['mode'] = self.settings['image_mode']
        else:
            res['width'] = res['height'] = -1
            res['mode'] = None
        if args:
            if args.width is not None:
                res['width'] = args.width
            if args.height is not None:
                res['height'] = args.height
            if args.mode is not None:
                res['mode'] = args.mode

#           res['shift'] = self.settings['input_shift'] if args.bit_shift
#           is None else args.bit_shift
            res['divisor'] = self.settings['input_divisor']\
                if args.divisor is None else args.divisor
            res['offset'] = self.settings['input_offset']\
                if args.offset is None else args.offset
            res['transpose'] = self.settings['image_transpose'] if args.transpose is None\
                else args.transpose
            res['norm_func'] = self.settings['input_norm_func'] if args.norm_func is None\
                else args.norm_func

            if args.rgb888_rgb565:
                res['rgb888_rgb565'] = True
        else:
            #            res['shift'] = self.settings['input_shift']
            res['divisor'] = self.settings['input_divisor']
            res['offset'] = self.settings['input_offset']
            res['transpose'] = self.settings['image_transpose']
            res['norm_func'] = self.settings['input_norm_func']

        if args.nptype:
            res['nptype'] = args.nptype

        return res

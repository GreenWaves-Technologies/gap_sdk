import os
import subprocess
import shlex
import pulp_config as plpconfig
import SCons.Util
import os.path

install_dir = os.environ.get('INSTALL_DIR')
target_install_dir = os.environ.get('TARGET_INSTALL_DIR')

if install_dir is None:
  install_dir = 'install'

if target_install_dir is None:
  target_install_dir = 'install'

files = [ 'hal/pulp.h', 'hal/utils.h', 'hal/pulp_io.h', 'hal/debug_bridge/debug_bridge.h' ]
files.append('hal/gvsoc/gvsoc.h')
files.append('hal/tb/tb.h')

try:
  for file in subprocess.check_output(shlex.split('plpfiles copy --item=hal_files')).decode('UTF-8').split():
    if os.path.exists('include/' + file):
      files.append(file)

except subprocess.CalledProcessError as e:
  print (e.output)
  raise

try:
  src_files = subprocess.check_output(shlex.split('plpfiles copy --item=hal_src_files')).decode('UTF-8').split()
except subprocess.CalledProcessError as e:
  print (e.output)
  raise


configs = plpconfig.get_configs_from_env()

def append_file(file):
  global files
  if not file in files and os.path.exists('include/' + file):
    files.append(file)


for config in configs:

  # The old system is storing the path to archi files in the json file
  # This has to be migrated so that only IP information is stored in the
  # json file and this build system is them copying the archi files according
  # to the IP information found in the json file.

  chip = config.get('**/chip/pulp_chip_family').get()


  append_file('hal/chips/%s/pulp.h' % chip)

  udma_hyper = config.get_child_int('**/udma/hyper/version')
  if udma_hyper is not None:
    append_file('hal/udma/hyper/udma_hyper_v%d.h' % udma_hyper)

  # RTC
  rtc = config.get('**/soc/rtc')
  if rtc is None:
    rtc = config.get('**/chip/rtc')
  if rtc is not None:
    rtc_version = config.get_child_int('**/rtc/version')
    if rtc_version == 1 or rtc_version is None:
      append_file('hal/vendors/dolphin/rtc.h')
    else:
      append_file('hal/rtc/rtc_v%d.h' % (rtc_version))

  # PMU
  pmu = config.get_child_int('**/soc/pmu/version')
  if pmu is None:
    pmu = config.get_child_int('**/chip/pmu/version')

  if pmu is not None:
    append_file('hal/maestro/pmu_v%d.h' % pmu)

  # ITC
  itc = config.get_child_int('**/soc/fc_itc/version')
  if itc is not None:
    append_file('hal/itc/itc_v%d.h' % itc)

  # GPIO
  gpio = config.get_child_int('**/gpio/version')
  if gpio is not None:
    append_file('hal/gpio/gpio_v%d.h' % gpio)


  # Chip specific files can be included here
  if chip == 'vega':
    append_file('hal/pwm/pwm_v1.h')
    append_file('hal/chips/vega/efuse.h')
    append_file('hal/chips/vega/pulp.h')
  elif chip == 'gap9':
    append_file('hal/pwm/pwm_v1.h')
    append_file('hal/chips/gap9/efuse.h')
    append_file('hal/chips/gap9/pulp.h')
  elif chip == 'gap':
    append_file('hal/pwm/pwm_v1.h')
  elif chip == 'wolfe':
    append_file('hal/pwm/pwm_v1.h')
  elif chip == 'vivosoc3':
    append_file('hal/chips/vivosoc3/fll.h')
    append_file('hal/chips/vivosoc3/freq.h')
  elif chip == 'vivosoc3_5':
    append_file('hal/chips/vivosoc3_5/fll.h')
    append_file('hal/chips/vivosoc3_5/freq.h')
  elif chip == 'vivosoc3_1':
    append_file('hal/chips/vivosoc3_1/fll.h')
    append_file('hal/chips/vivosoc3_1/freq.h')
  elif chip == 'vivosoc4':
    append_file('hal/chips/vivosoc4/fll.h')
    append_file('hal/chips/vivosoc4/freq.h')


targets = []

for file in files:
  file_path = os.path.join('include', file)
  targets.append(InstallAs(os.path.join(target_install_dir, file_path), file_path))

for file in src_files:
  file_path = os.path.join('src', file)
  targets.append(InstallAs(os.path.join(target_install_dir, file_path), file_path))


Default(targets)

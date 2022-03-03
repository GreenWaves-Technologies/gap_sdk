if [  -n "${ZSH_VERSION:-}" ]; then
	DIR="$(readlink -f -- "${(%):-%x}")"
	DIRNAME="$(dirname $DIR)"
	GAP_SDK_HOME=$(dirname $DIRNAME)
	export GAP_SDK_HOME
	#echo $(dirname "$(readlink -f ${(%):-%N})")
else
	export GAP_SDK_HOME="$(dirname $(dirname "$(readlink -f "${BASH_SOURCE[0]}")"))"
fi

export PATH=$GAP_SDK_HOME/install/workstation/openocd/bin:$PATH

# Path to openocd scripts
export OPENOCD_SCRIPTS=$GAP_SDK_HOME/utils/openocd_tools


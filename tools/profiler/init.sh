if [  -n "${ZSH_VERSION:-}" ]; then
	DIR="$(readlink -f -- "${(%):-%x}")"
	script_path="$(dirname $DIR)"
else
	script_path="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
fi

export PATH=$script_path/gui/build:$script_path/backend/src/scripts/:$PATH

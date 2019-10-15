Configuration
-------------

The virtual platform is simulating the architecture which is described by the specified system configuration, described with a JSON file.

This can be first done through the option *\-\-config-file* to give the path of the JSON file. This can be either an absolute path or a relative path, in which case the config in search in the paths given by the environment variable SDK_CONFIGS_PATH, which contains a list of possible paths separated by *:*.

This can also be done through the option *\-\-config* which gives the name of the configuration to simulate. This is equivalent to *\-\-config-file=chips/<name>/<name>.json* where *<name>* is the value of the option.

The configuration is a high-level description of the architecture, where all important properties are specified (e.g. memory sizes). This high-level view of the architecture is used to generate a low-level and detailed view of the architecture which is used by gvsoc to know what to instantiate, configure and connect. Both levels can be customized by the user. The high-level view is called the template, and can be customized to easily change architecture properties such as memory sizes. The low-level view is called the configuration and can be customized to change properties of one specific component, such as a specific behavior of one core.


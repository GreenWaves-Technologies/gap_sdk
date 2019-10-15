Options
-------

Options to the virtual platform are passed by customizing the system configuration.

This can be first done using the option *\-\-property=<path>=<value>* to specify a property in the JSON file to be overwritten. <path> is giving the property path in the JSON file where the property must be overwritten and <value> the value to be set. As a JSON file is hierarchical, <path> describes a hierarchical path, similar to a file system path. As described in the previous section, a property can be changed either in the template or in the configuration. Any property beginning with *config/* will change a property in the configuration while the others will change it in the template.

Here is an example to activate instructions traces: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --property=config/gvsoc/trace=insn

There is however a shortcut for this property, which can be set with this option: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --trace=insn

The second way to customize the system configuration is to provide a INI-style configuration file containing a set of properties through the option *\-\-config-user* . The JSON path of a property is the concatenation of the section name, and the INI property name.

Here is an example of such a file to activate instruction traces: ::

  [config.gvsoc]
  trace=insn

And the command to specify it: ::

  pulp-run --platform=gvsoc --config=gap_rev1 --binary=test prepare run --config-user=myconfig.ini

In both ways, refer to other sections to get the various properties which can be set to configure the system.

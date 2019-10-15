
## Support to SystemC TLM-2.0 Models

Requirements to use this feature:

An environment variable influences the build process and should be defined in
order to use SystemC TLM-2.0 models.

```bash
export VP_USE_SYSTEMC=1
```

Optionally, this variable can be added to an initialization file such as
~/.bashrc.

A convenience script [get_systemc.sh](get_systemc.sh) that downloads and
builds the *SystemC Class Library Proof of Concept implementation* provided by
[Accellera Systems Initiative](http://www.accellera.org) is provided.

Execute the script as follows.

```bash
$ ./get_systemc.sh
```

## References

[1] [http://www.accellera.org](http://www.accellera.org)

[2] [https://standards.ieee.org/standard/1666-2011.html](https://standards.ieee.org/standard/1666-2011.html)

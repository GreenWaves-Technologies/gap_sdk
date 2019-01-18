## Read-only Filesystem

This a read-only filesystem, meaning file must be uploaded in the HyperFlash before use and can only be read.

To upload a file do as follows :
```bash
make [clean] all run WRITE=true FILE=filename
```

Then once uploaded, and as long as Gapuino is not shut down, you can access file on the HyperFlash.
```bash
make [clean] all run
```

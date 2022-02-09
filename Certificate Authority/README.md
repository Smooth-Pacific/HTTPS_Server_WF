# Certificate Authority
These scripts are intended to form the underlying structure that support use of the certificate authorities for the http server and enable https access.
They do not stand independently, nor are they intended to. Rather, they are intended to be adapted once more of the server is designed to work with whatever interface is desired
Included is demo.sh which must be called with a single parameter of where the CA folders and files should be built, and also needs to be the location to which the other scripts, cnf_files, and the example documents must be copied.
Pushing the CAs to an external server must be delayed until a future sprint.
#!/usr/bin/bash

#####################################################
# Convert Certificates and CRL to Publishable Format


#create DER certificate
openssl x509				\
	-in $basepath/ca/rca.crt	\
	-out $basepath/ca/rca.cer	\
	-outform der

#create DER certificate
openssl x509				\
	-in $basepath/ca/servca.crt	\
	-out $basepath/ca/servca.cer	\
	-outform der

#create DER certificate
openssl x509				\
	-in $basepath/ca/docuca.crt	\
	-out $basepath/ca/docuca.cer	\
	-outform der

#The DER CRL is not obviously distinct from the pre-DER form
#create DER CRL
openssl crl				\
	-in $basepath/ca/crl/rca.crl	\
	-out $basepath/ca/crl/rca.crl	\
	-outform der

#create DER CRL
openssl crl					\
	-in $basepath/ca/crl/servca.crl		\
	-out $basepath/ca/crl/servca.crl	\
	-outform der

#create DER CRL
openssl crl					\
	-in $basepath/ca/crl/docuca.crl		\
	-out $basepath/ca/crl/docuca.crl	\
	-outform der
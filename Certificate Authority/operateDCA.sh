#!/usr/bin/bash

################################################
#Document signing example

itemname=$1

#create document request
openssl req					\
	-new					\
	-config $basepath/ca/docucert.cnf	\
	-out $basepath/ca/certs/$itemname.csr	\
	-passout pass:demodocupassword		\
	-keyout $basepath/ca/certs/$itemname.key
echo "end request"
#create server certificate
echo -e "y\ny\n" | 				\
openssl	ca					\
	-config $basepath/ca/docuca.cnf		\
	-in $basepath/ca/certs/$itemname.csr	\
	-out $basepath/ca/certs/$itemname.crt	\
	-passin file:$docucapasspath		\

#create pkcs#12 bundle
openssl pkcs12					\
	-export					\
	-name "Demo Document"			\
	-inkey $basepath/ca/certs/$itemname.key	\
	-passin pass:demodocupassword		\
	-in $basepath/ca/certs/$itemname.crt	\
	-certfile $basepath/ca/docuca_chain.pem	\
	-out $basepath/ca/certs/$itemname.p12	\
	-passout pass:outputdemodocupassword		


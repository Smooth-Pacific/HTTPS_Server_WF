#!/usr/bin/bash

################################################
# Server signing example
# doesn't actually sign a server, because I haven't figured out what object to target yet for that operation

#create server request
openssl req					\
	-new					\
	-config $basepath/ca/servcert.cnf	\
	-out $basepath/ca/certs/$itemname.csr	\
	-passout pass:demoservpassword		\
	-keyout $basepath/ca/certs/$itemname.key

#create server certificate
echo -e "y\ny\n" | 				\
openssl	ca					\
	-config $basepath/ca/servca.cnf		\
	-in $basepath/ca/certs/$itemname.csr	\
	-out $basepath/ca/certs/$itemname.crt	\
	-passin file:$servcapasspath		\

#create pkcs#12 bundle
openssl pkcs12					\
	-export					\
	-name "Demo Server"			\
	-inkey $basepath/ca/certs/$itemname.key	\
	-passin pass:demoservpassword		\
	-in $basepath/ca/certs/$itemname.crt	\
	-certfile $basepath/ca/servca_chain.pem	\
	-out $basepath/ca/certs/$itemname.p12	\
	-passout pass:outputdemoservpassword		


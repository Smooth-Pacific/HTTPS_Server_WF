#!/usr/bin/bash

####################################
# Create Root CA

#create CA request for root
openssl	req 				\
	-new 				\
	-config $basepath/ca/rca.cnf	\
	-passout file:$rcapasspath 	\
	-out $basepath/ca/rca.csr 	\
	-keyout $basepath/ca/rca/private/rca.key

echo "end root request"
#Create CA cert by self-signing
echo -e "y\ny\n" | 			\
openssl ca				\
	-selfsign			\
	-config $basepath/ca/rca.cnf	\
	-passin file:$rcapasspath 	\
	-in $basepath/ca/rca.csr	\
	-out $basepath/ca/rca.crt	\


#create empty/initial CRL
openssl	ca				\
	-gencrl				\
	-config $basepath/ca/rca.cnf	\
	-passin file:$rcapasspath	\
	-out $basepath/ca/crl/rca.crl


#######################################
# Create Server Signing CA

#create CA request for signing CA 
openssl req				\
	-new 				\
	-config $basepath/ca/servca.cnf \
	-passout file:$servcapasspath	\
	-out $basepath/ca/servca.csr	\
	-keyout $basepath/ca/servca/private/servca.key

#create CA certificate
echo -e "y\ny\n" | 			\
openssl	ca				\
	-config $basepath/ca/rca.cnf	\
	-passin file:$rcapasspath 	\
	-in $basepath/ca/servca.csr	\
	-out $basepath/ca/servca.crt	\

#create empty/initial CRL
openssl	ca				\
	-gencrl				\
	-config $basepath/ca/servca.cnf	\
	-passin file:$servcapasspath 	\
	-out $basepath/ca/crl/servca.crl

#create PEM bundle
cat	$basepath/ca/servca.crt	\
	$basepath/ca/rca.crt	\
	> $basepath/ca/servca_chain.pem


#####################################
# Create Document Signing CA

#create CA request for docu CA 
openssl	req				\
	-new				\
	-config $basepath/ca/docuca.cnf	\
	-passout file:$docucapasspath	\
	-out $basepath/ca/docuca.csr	\
	-keyout $basepath/ca/docuca/private/docuca.key

#create CA certificate
echo -e "y\ny\n" | 			\
openssl	ca				\
	-config $basepath/ca/rca.cnf	\
	-passin file:$rcapasspath 	\
	-in $basepath/ca/docuca.csr	\
	-out $basepath/ca/docuca.crt	\

#create empty/initial CRL
openssl	ca				\
	-gencrl				\
	-config $basepath/ca/docuca.cnf	\
	-passin file:$docucapasspath	\
	-out $basepath/ca/crl/docuca.crl

#create PEM bundle
cat	$basepath/ca/docuca.crt	\
	$basepath/ca/rca.crt	\
	> $basepath/ca/docuca_chain.pem


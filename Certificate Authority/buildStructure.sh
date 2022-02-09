#!/usr/bin/bash

####################################
# Root CA

# make some directories
mkdir $basepath/ca
mkdir $basepath/ca/rca
mkdir $basepath/ca/rca/private
mkdir $basepath/ca/rca/db
mkdir $basepath/ca/crl
mkdir $basepath/ca/certs

#make the powerful ones restricted access
#chmod 700 $basepath/ca/rca

#create some supporting files: database, database attributes, certificate serials, and revocation serials
touch $basepath/ca/rca/db/rca.db
touch $basepath/ca/rca/db/rca.db.attr
echo 01 > $basepath/ca/rca/db/rca.crt.srl
echo 01 > $basepath/ca/rca/db/rca.crl.srl


#######################################
# Server Signing CA

# make some directories
mkdir $basepath/ca/servca
mkdir $basepath/ca/servca/private
mkdir $basepath/ca/servca/db

#make the powerful ones restricted access
#chmod 700 $basepath/ca/servca/private

#create some supporting files: database, database attributes, certificate serials, and revocation serials
touch $basepath/ca/servca/db/servca.db
touch $basepath/ca/servca/db/servca.db.attr
echo 01 > $basepath/ca/servca/db/servca.crt.srl
echo 01 > $basepath/ca/servca/db/servca.crl.srl


#####################################
# Document Signing CA

# make some directories
mkdir $basepath/ca/docuca
mkdir $basepath/ca/docuca/private
mkdir $basepath/ca/docuca/db

#make the powerful ones restricted access
#chmod 700 $basepath/ca/docuca/private

#create some supporting files: database, database attributes, certificate serials, and revocation serials
touch $basepath/ca/docuca/db/docuca.db
touch $basepath/ca/docuca/db/docuca.db.attr
echo 01 > $basepath/ca/docuca/db/docuca.crt.srl
echo 01 > $basepath/ca/docuca/db/docuca.crl.srl


########################################
# Copy in the config files to a local location
cp cnf_files/rca.cnf 	$basepath/ca
cp cnf_files/servca.cnf	$basepath/ca
cp cnf_files/docuca.cnf	$basepath/ca

cp cnf_files/servcert.cnf $basepath/ca
cp cnf_files/docucert.cnf $basepath/ca
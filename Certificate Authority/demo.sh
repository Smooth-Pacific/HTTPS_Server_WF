#!/usr/bin/bash
clear

export basepath=$1
echo $basepath

#purge everything in chosen location/ca before starting anew
rm -r -f $basepath/ca


# Build the file structure required for the CAs
./buildStructure.sh

#Create passwords and password storage locations for the demo. How the passwords are imported and where they are stored is still TBD depending on final implementation of the CAs

touch $basepath/ca/rca/private/capass.txt
export rcapasspath="$basepath/ca/rca/private/capass.txt"
echo "DemoCAPass" > $rcapasspath

touch $basepath/ca/servca/private/servcapass.txt
export servcapasspath="$basepath/ca/servca/private/servcapass.txt"
echo "DemoServCAPass" > $servcapasspath

touch $basepath/ca/docuca/private/docucapass.txt
export docucapasspath="$basepath/ca/docuca/private/docucapass.txt"
echo "DemoDocuCAPass" > $docucapasspath

# Acutally make the CAs: root, server, and document
./buildCA.sh

#Use the Server and Document CAs to sign example items
./operateSCA.sh This_is_not_a_server.txt
./operateDCA.sh signable.txt

# Publish the certificates in DER format
./publishDER.sh
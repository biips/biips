
Installer machine virtuelle Mac OS X
============================================
1. Suivre instructions sur l'une des pages suivantes:
	* http://www.cachem.fr/comment-installer-osx-pc-virtualbox/
	* http://www.tech2tech.fr/comment-installer-os-x-mountain-lion-sur-votre-pc-via-virtualbox/
	* http://lifehacker.com/5938332/how-to-run-mac-os-x-on-any-windows-pc-using-virtualbox
	* http://www.macbreaker.com/2012/07/mountain-lion-virtualbox.html

2. Installer programmes :
	* XCode (sur l'App Store) + Command Line Tools (Preferences | Downloads)
	* Sublime Text
	* Google Chrome
	* Dropbox

3. Installer outils de développement pour BiiPS :
	* Git
	* CMake
	* MacPorts avec autoconf
		sudo ports install autoconf
	* R + package Rcpp
	* RStudio
	* MacTex
	* VPN Inria AnnyConnect : http://dsi.inria.fr/services_offerts/vpn/Guide_utilisation/index_html
	* Matlab : https://wiki.bordeaux.inria.fr/sed/doku.php?id=intranet:developpementlogiciel:matlabvpn inria
	* Eclipse CDT (64 bit) + CDT update site + CMakeEd + EGit

4. Compiler Boost :
	* Télécharger Boost pour Linux
	* Décompresser
	* Ouvrir la console et se placer dans le répertoire décompressé :
	* Compiler Boost.Build (nécessite compilateur Visual C++ cl.exe dans le PATH)

		./bootstrap.sh

	* Compiler les bibliothèques désirées et installer dans le sous-répertoire stage\lib

		./b2 --build-type=complete --layout=versioned --with-program_options --with-regex stage


Installer machine virtuelle Mac OS X (10.8 Mountain Lion)
============================================
2. Créer une nouvelle machine virtuelle dans VirtualBox :
	* Mémoire : 4096Mo
	* Disque dur : VDI dynamiquement alloué 60Go
	* Vidéo : 3D 128Mo

1. Suivre instructions sur l'une des pages suivantes:
	* http://www.cachem.fr/comment-installer-osx-pc-virtualbox/
	* http://www.tech2tech.fr/comment-installer-os-x-mountain-lion-sur-votre-pc-via-virtualbox/
	* http://lifehacker.com/5938332/how-to-run-mac-os-x-on-any-windows-pc-using-virtualbox
	* http://www.macbreaker.com/2012/07/mountain-lion-virtualbox.html

2. Installer programmes :
	* XCode (sur l'App store) + Command Line Tools (Preferences | Downloads)
    * Mises à jour de l'App store
    * OSXFuse (avec compatibilité MacFuse) + NTFS-3G (sans MacFuse)
	* Sublime Text
	* Google Chrome
	* Dropbox
	* The Unarchiver (App Store)

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
    * XQuartz
	* Eclipse CDT (64 bit) + CDT update site + CMakeEd + EGit
	* Octave via MacPorts
		sudo ports install octave


Installer machine virtuelle Windows
============================================
1. Télécharger l'iso Windows 7 Pro N (x64) : https://wiki.bordeaux.inria.fr/sed/doku.php?id=intranet:developpementlogiciel:msdnaa

2. Créer une nouvelle machine virtuelle dans VirtualBox :
	* Mémoire : 4096Mo
	* Disque dur : VDI dynamiquement alloué 60Go

3. Charger image iso Win7 : *Stockage* -> *Controlleur IDE* -> choisissez fichier et cocher *Live CD/DVD*

4. Démarrer la machine et suivre instructions d'installation

5. Installer programmes et mises à jour Win7 :
	* Microsoft Security essentials
	* Additions invité : Dans VBox -> Onglet Périphériques -> Installer les additions invité
	* Media Feature Pack for Windows 7 N with Service Pack 1
	* 7zip
	* Foxit Reader
	* Java (ajouter au PATH : C:\Program Files (x86)\Java\jre7\bin)
	* Visual Studio (ajouter au PATH : C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin)
	* Sublime Text
	* VLC
	* Google Chrome
	* Dropbox
	
6. Ejecter l'image Win7 et redémarrer

7. Installer outils de développement pour BiiPS :
	* PuTTY
	* TortoiseGit + msysgit
	* CMake
	* MinGW (ne pas ajouter au PATH)
	* R (ajouter au path : C:\Program Files\R\R-3.0.2\bin) + package Rcpp
	* RTools (ajouter au PATH : C:\Rtools\gcc-4.6.3\bin)
	* RStudio
	* NSIS 2.46
	* MikTeX
	* VPN Inria AnnyConnect : http://dsi.inria.fr/services_offerts/vpn/Guide_utilisation/index_html
	* Matlab : https://wiki.bordeaux.inria.fr/sed/doku.php?id=intranet:developpementlogiciel:matlabvpn inria
    * Octave (MinGW port) : http://wiki.octave.org/Octave_for_Microsoft_Windows#MinGW_ports
	* Eclipse CDT (32 bit) + CDT update site + CMakeEd + EGit
    * clink : http://mridgers.github.io/clink/


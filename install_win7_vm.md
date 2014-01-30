
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
	* Eclipse CDT (32 bit) + CDT update site + CMakeEd + EGit

8. Compiler Boost :
	* Télécharger Boost pour Windows
	* Décompresser
	* Ouvrir la console et se placer dans le répertoire décompressé :
	* Compiler Boost.Build (nécessite compilateur Visual C++ cl.exe dans le PATH)

		bootstrap

	* Compiler les bibliothèques désirées et installer dans le sous-répertoire stage\lib

		.\b2 toolset=gcc address-model=64 --build-type=complete --with-program_options --with-regex stage

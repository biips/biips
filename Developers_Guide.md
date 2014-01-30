Requirements
======================
* git
* cmake
* gcc (>= 4.7)
* boost (>=1.49.0)
* R (>= 2.13)
* Rcpp package
* Matlab or Octave

Manage sources
======================
See http://git-scm.com/doc

and especially
http://git-scm.com/book/en/Git-Basics-Recording-Changes-to-the-Repository

Clone remote repository
-----------------------
Anonymous access (read-only):

    git clone https://gforge.inria.fr/git/biips/biips.git
    
GForge user access:

    git clone git+ssh://<username>@scm.gforge.inria.fr//gitroot/biips/biips.git
    
Commit (local)
-----------------------

See status

    git status -s

Add new files or files already tracked to STAGED zone

    git add file

Commit STAGED zone

    git commit

Commit STAGED zone and all modifications of files already tracked (without having to do git add)

    git commit -a
    
Push (remote)
-----------------------

    git push origin my_branch

Ex. for master branch:

    git push origin master

View all branches
-----------------------

    git branch -a

Create new branch
-----------------------
    git branch new_branch [source_branch]
    git checkout new_branch

or 

    git checkout -b new_branch [source_branch]

Pull (remote)
-----------------------

    git pull origin my_branch

Continuous integration
==========================
Continuous integration is hosted on Inria CI website: https://ci.inria.fr/biips

You first need an account on CI website.

After logging in the CI website, go to *MyAccount* page and add your ssh public key.

Ask the Biips administrators to give you access to the project.

Slaves
--------------
* Ubuntu 12.04 amd64
* Windows 7 64bit

Connect to slaves
--------------
On CI website, go to your *Dashboard*.

In the Biips box, click *Manage Project* and then click *Slaves* on the left panel.

Click the *Connect* button on the desired slave to get connection commands that include your username.


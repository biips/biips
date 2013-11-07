Requirements
======================
To compile Biips, you need:
* git
* cmake
* gcc (>= 4.7)
* boost-X.XX (X>=1.49)
* Matlab or Octave
* R with RCpp package

Manage sources
======================
See http://git-scm.com/doc

and especially
http://git-scm.com/book/en/Git-Basics-Recording-Changes-to-the-Repository

Clone remote repository
-----------------------
Anonymous access:

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


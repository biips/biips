#!/bin/bash
git filter-branch --original refs/original2 --commit-filter '
        if [ "$GIT_COMMITTER_NAME" = "Adrien" ];
        then
                GIT_COMMITTER_NAME="todeschi";
                GIT_AUTHOR_NAME="todeschi";
                GIT_COMMITTER_EMAIL="adrien.todeschini@inria.fr";
                GIT_AUTHOR_EMAIL="adrien.todeschini@inria.fr";
                git commit-tree "$@";
        else
                git commit-tree "$@";
        fi
        if [ "$GIT_COMMITTER_NAME" = "adrien-alea" ];
        then
                GIT_COMMITTER_NAME="todeschi";
                GIT_AUTHOR_NAME="todeschi";
                GIT_COMMITTER_EMAIL="adrien.todeschini@inria.fr";
                GIT_AUTHOR_EMAIL="adrien.todeschini@inria.fr";
                git commit-tree "$@";
        else
                git commit-tree "$@";
        fi' -- --all

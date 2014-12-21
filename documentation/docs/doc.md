# Building this very documentation

Here I explain how I built this very documentation.

## MkDocs : documentation with markdown files

I needed to make a simple and quick documentation, therefore the markdown language seemed adapted.
It is easy to use and it's compilation to html is well supported by many actors (including [Github](http://github.com)).

I found [MkDocs](http://www.mkdocs.org/) which is a "fast, simple and downright gorgeous static site generator" from markdown files with a YAML configuration file.

## Building and hosting

The documentation files are in the `documentation/` folder of the repository.
The documentation web site is published in the github pages of the repository thus in the `gh-pages` branch of the repository.

As changing branch to build the site with MkDocs is not convenient,
I first wanted to use subtree to sync `gh-pages` branch in a folder in `master` branch.
But I had bugs with subtree merging. Finally I chose to simply clone the repository in two different folders.

For the second one I copied only the `gh-pages` branch with the following command :
```
git clone -b gh-pages --single-branch git@github.com:mpizenberg/Singapore_interview.git Singapore_interview_gh-pages/
```
Thus I just need to tell MkDocs were to build the site in `mkdocs.yml` file :
```
site_dir: ../../Singapore_interview_gh-pages
```

Finally, as I can't use the `mkdocs build --clean` command (which delete the `.git/` folder of the other repository),
I made a simple script (`build_docsite.sh`) to do it for me :
```
#!/bin/bash
rm -rf ../../Singapore_interview_gh-pages/*
mkdocs build
```

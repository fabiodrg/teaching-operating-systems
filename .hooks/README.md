# Hook installation

The `pre-commit` hook is responsible to detect changes on `README.md` files under the exercise solution folders, e.g. `f0`, `f1`, etc. In changes are detected, it automatically compiles a new PDF and automatically stages the new version, ahead of the commit.

To install the hook with a symbolic link:

```shell
$ ln -s ../../.hooks/pre-commit .git/hooks/pre-commit
```
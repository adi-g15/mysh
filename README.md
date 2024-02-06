# MySh

This is a basic shell, written to prove that shell is not that complex as a
definition he told :)

# Using this as your default shell

> It is safe to use this as your default shell, because you still will be able
> to start a different shell, such as `zsh` by typing `zsh`, or run a executable
> at any location by typing the full path. So anytime, you can start `zsh`,
> `bash`, or run `chsh` to change default shell

1. Compile

```
gcc -o mysh mysh.c
```

Probably copy `mysh` to somewhere like `/usr/local/bin/`

2. Add entry to `/etc/shells`

Considering the `mysh` binary is in `/usr/local/bin/mysh`, add it to
`/etc/shells`, for eg. my `/etc/shells` now is like this:

```
/bin/sh
/bin/bash
/usr/bin/sh
/usr/bin/bash
/usr/bin/zsh
/bin/zsh
/usr/bin/tmux
/bin/tmux
/bin/ksh
/bin/rksh
/usr/bin/ksh
/usr/bin/rksh
/usr/local/bin/mysh
```

3. Change default shell

```
chsh -s /usr/local/bin/mysh $USER   # Change for current user, replace $USER
with whatever username you want to change the shell for
```


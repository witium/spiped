# Should be sourced by `command -p sh posix-l.sh "$PATH"` from within a Makefile.
if ! [ ${PATH} = "$1" ]; then
	echo "WARNING: POSIX violation: $SHELL's command -p resets \$PATH" 1>&2
	PATH=$1
fi
for LIB in rt xnet; do
	if ${CC} -l${LIB} posix-l.c 2>/dev/null; then
		printf "%s" " -l${LIB}";
	else
		echo "WARNING: POSIX violation: make's CC doesn't understand -l${LIB}" 1>&2
	fi
	rm -f a.out
done

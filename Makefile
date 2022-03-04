reformat:
	for name in Common Lhowon Scratch; do \
		find $$name -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i -style=file; \
	done;

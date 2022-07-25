reformat:
	for name in Common Modules; do \
		find $$name -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i -style=file; \
	done;

librockFillAndSign:
	${CC} -o librockFillAndSign command.c

coverage:
	${CC} -o librockFillAndSign_t -Dlibrock_WANT_BRANCH_COVERAGE --coverage command.c

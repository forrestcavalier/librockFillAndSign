awsFillAndSign:
	${CC} -o awsFillAndSign command.c

coverage:
	${CC} -o awsFillAndSign_t -Dlibrock_WANT_BRANCH_COVERAGE --coverage command.c

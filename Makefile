coverage:
	${CC} -o awsFillAndSign -Dlibrock_WANT_BRANCH_COVERAGE --coverage -DLIBROCK_UNSTABLE command.c

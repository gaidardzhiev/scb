#!/bin/sh

FILES=64

for i in $(seq 1 $FILES); do
cat > file$i.h <<EOF
#ifndef FILE${i}_H
#define FILE${i}_H

void func$i(void);

#endif
EOF

cat > file$i.c <<EOF
#include "file$i.h"
#include <stdio.h>

void func$i(void) {
    printf("This is function func$i\\n");
}
EOF
done

cat > main.c <<'EOF'
#include <stdio.h>
EOF

for i in $(seq 1 $FILES); do
	echo "#include \"file$i.h\"" >> main.c
done

cat >> main.c <<'EOF'

int main(void) {
EOF

for i in $(seq 1 $FILES); do
	echo "    func$i();" >> main.c
done

cat >> main.c <<'EOF'
    return 0;
}
EOF

printf "generated %s source/header files and main.c\n" "$FILES"

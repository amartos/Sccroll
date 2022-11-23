#!/usr/bin/awk -f

BEGIN { ORS=""; FS=",[\n\t ]*"; }

/^SCCROLL_MOCK(.*)$/ {
    if (length($2) > 0) {
        if (f == 0) {
            print "-Wl";
            f = 1;
        }
        print ",--wrap,"$2;
    }
}

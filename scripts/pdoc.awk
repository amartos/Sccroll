#!/usr/bin/awk -f

BEGIN { FS="(#!|# @brief)"; }

/^#!/ {
    doc = $2;
    sub(/^ */,"",doc);
    print doc;
}

/^# @brief/ {
    doc = $2;
    getline;
    cmd = $0;

    sub(/^ */,"",doc);
    sub(/\t*:.*/,"",cmd);
    printf "%s\t%s\n",cmd,doc;
}

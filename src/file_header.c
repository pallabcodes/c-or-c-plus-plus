// Implements writing and validating the header in the binary DB file.

#include "file_header.h"
#include <string.h>
#include <stdio.h>

bool write_db_header(FILE *fp) {
    DBHeader header;
    memcpy(header.magic, DB_MAGIC, DB_MAGIC_SIZE);
    header.version = DB_VERSION;
    memset(header.reserved, 0, sizeof(header.reserved));

    return fwrite(&header, sizeof(DBHeader), 1, fp) == 1;
}

bool validate_db_header(FILE *fp) {
    DBHeader header;

    rewind(fp);
    if (fread(&header, sizeof(DBHeader), 1, fp) != 1) return false;

    if (memcmp(header.magic, DB_MAGIC, DB_MAGIC_SIZE) != 0) return false;
    if (header.version != DB_VERSION) return false;

    return true;
}

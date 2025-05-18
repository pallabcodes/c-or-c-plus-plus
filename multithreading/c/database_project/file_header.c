#include "file_header.h"
#include <string.h>

bool write_db_header(FILE *fp) {
    DBHeader header;
    memcpy(header.magic, DB_MAGIC, DB_MAGIC_SIZE);
    header.version = DB_VERSION;
    memset(header.reserved, 0, sizeof(header.reserved));

    // Write the header to the beginning of the file
    if (fwrite(&header, sizeof(DBHeader), 1, fp) != 1) {
        return false;
    }
    return true;
}

bool validate_db_header(FILE *fp) {
    DBHeader header;

    // Rewind and read the header
    rewind(fp);
    if (fread(&header, sizeof(DBHeader), 1, fp) != 1) {
        return false;
    }

    // Check magic number and version
    if (memcmp(header.magic, DB_MAGIC, DB_MAGIC_SIZE) != 0) {
        return false;
    }

    if (header.version != DB_VERSION) {
        return false;
    }

    return true;
}

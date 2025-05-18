// Declares parse-related functionality (currently unused)

#ifndef PARSE_H
#define PARSE_H

void parse_database_file(const char *filepath);
void export_to_csv(const char *filepath);
void import_from_csv(const char *filepath);
void export_to_json(const char *filepath);
void import_from_json(const char *filepath);


#endif // PARSE_H

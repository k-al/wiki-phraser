#ifndef PHRASER_GUARD
#define PHRASER_GUARD

#include <sstream>

#include "phraser_helper.hpp"
#include "logger.hpp"
#include "args.hpp"

void set_globals (Logger&, Args&);

/**
 * Reads the input Files and places the tags in the given Entry\n
 * also places the command section in content at pos 0
 * @param file the file Entry to phrase
 */
void process_meta_single (Entry* file);


/**
 * scans dirs for files to phrase images to copy...\n
 * executes process_meta_single on all phraseable files to build up the entry data structure\n
 * copies not phraseable files to destination and creates a Entry for those too
 */
void process_meta ();


void command_link (std::stringstream& out_stream, const Command& command, const Entry* active_entry);
void command_time (std::stringstream& out_stream, const Command& command, const Entry* active_entry);

/**
 * iterate through the Entrystructure to find and replace the commands in the phraseable files with html tags\n
 * calls at a found command the secific 'comand_xxx()'
 */
void process_command ();

#endif

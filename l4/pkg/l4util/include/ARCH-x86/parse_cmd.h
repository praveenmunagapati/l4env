/**
 * \file   l4util/include/ARCH-x86/parse_cmd.h
 * \brief  comfortable command-line parsing
 *
 * \date   2002
 * \author Jork Loeser <jork.loeser@inf.tu-dresden.de>
 */

/* (c) 2003 Technische Universitaet Dresden
 * This file is part of DROPS, which is distributed under the terms of the
 * GNU General Public License 2. Please see the COPYING file for details. */

#ifndef __PARSE_CMD_H
#define __PARSE_CMD_H

/** \defgroup parse_cmd Comfortable Command Line Parsing */

enum parse_cmd_type {
    PARSE_CMD_INT,
    PARSE_CMD_SWITCH,
    PARSE_CMD_STRING,
};

/** Parse the command-line for specified arguments and store the values into
 * variables.
 * \ingroup parse_cmd
 *
 * This Functions gets the command-line, and a list of command-descriptors.
 * Then, the command-line is checked against the given descriptors, storing
 * strings, switches and numeric arguments at given addresses. A default help
 * descriptor is added. Its purpose is to present a short command overview in
 * the case the given command-line does not fit to the descriptors.
 *
 * Each command-descriptor has the following form:
 *
 * \e short \e option \e char, \e long \e option \e name, 
 * \e comment, \e type, \e val, \e addr.
 *
 * The \e short \e option \e char specifies the short form of the described
 * option. The short form will be recognized after a single dash, or in a group
 * of short options preceeded by a single dash. Specify ' ' if no short form
 * should be used.
 * 
 * The \e long \e option \e name specifies the long form of the described
 * option. The long form will be recognized after two dashes. Specify 0 if no
 * long form should be used for this option.
 *
 * The \e comment is a string that will be used when presenting the short
 * command-line help.
 *
 * The \e type specifies, if the option should be recognized as a number
 * (\c PARSE_CMD_INT), a switch (PARSE_CMD_SWITCH) or a string 
 * (\c PARSE_CMD_STRING). If \c PARSE_CMD_INT, the option requires a second
 * argument on the command-line after the option. This argument is parsed as
 * a number. It can be preceeded by 0x to present a hex-value or by 0 to
 * present an octal form. If the type is \c PARSE_CMD_SWITCH, no additional
 * argument is expected at the command-line. If the type is 
 * \c PARSE_CMD_STRING, an additional string argument is expected.
 * 
 * The \e addr specifies, how recognized options should be returned. If \e type
 * is \c PARSE_CMD_INT, \e addr is a pointer to an int. The scanned argument
 * from the command-line is stored in this pointer. If \e type is 
 * \c PARSE_CMD_SWITCH, \e addr must be a pointer to int, and the value from 
 * \e val is stored at this pointer. If \e type is \c PARSE_CMD_STRING, 
 * \e addr must be a pointer to const char*, and a pointer to the argument on
 * the command line is stored at this pointer.
 *
 * For the types \c PARSE_CMD_INT and \c PARSE_CMD_STRING, the value in \e val
 * is a default value, which is stored in the given pointers if the
 * corresponding option is not given on the command line. If type is 
 * \c PARSE_CMD_SWITCH, and the corresponding options are not given on the
 * command-line, the value \e addr points to is not modified.
 *
 * The list of command-descriptors is terminated by specifying a binary 0 for
 * the short option char.
 *
 * Note: The short option char 'h' and the long option name "help" must not be
 * specified. They are used for the default help descriptor and produce a short
 * command-options help when specified on the command-line.
 *
 * \param argc  pointer to number of command line parameters as passed to main
 * \param argv  pointer to array of command line parameters as passed to main
 * \param arg0 format list describing the command line options to parse for
 * \return 0 if the command-line was successfully parsed, otherwise:
 * - -1 if the given descriptors are somehow wrong.
 * - -2 if not enough memory was available to hold temporary structs.
 * - -3 if the given command-line args did not meet the specified set.
 * - -4 if the help-option was given.
 *
 * Upon return, argc and argv point to a list of arguments that were not
 * scanned as arguments. See \c getoptlong for details on scanning. */
int parse_cmdline(int *argc, const char***argv, char arg0,...);

#endif


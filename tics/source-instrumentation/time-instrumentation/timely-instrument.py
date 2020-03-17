#!/usr/bin/env python

import sys
import os
import re
import argparse

# Helper functions
def find_closing_brace(data, start_search):
    brace_offset = 1 # We start with one opening brace and no closing braces
    brace_close_start = start_search
    while brace_offset != 0:
        brace_close_start = brace_close_start + 1
        c = c_file_data[brace_close_start]
        if c == '}':
            brace_offset = brace_offset - 1
        elif c == '{':
            brace_offset = brace_offset + 1
    return brace_close_start

def get_diff_indent(indent_a, indent_b):
    if len(indent_a) > len(indent_b):
        long_indent = indent_a
        short_indent = indent_b
    else:
        long_indent = indent_b
        short_indent = indent_a

    diff_indent = long_indent[0:len(long_indent)-len(short_indent)]
    return diff_indent

def find_next_non_whitespace(data, start_search):
    next_non_whitepace = start_search
    while True:
        if not c_file_data[next_non_whitepace].isspace():
            break
        next_non_whitepace = next_non_whitepace + 1

    return next_non_whitepace

def next_word_is(data, word, start_search):
    next_non_whitepace = find_next_non_whitespace(data, start_search)
    # Check if the word exsists
    if data[next_non_whitepace:next_non_whitepace+len(word)] == word:
        print('Next word is ' + word)
        return next_non_whitepace

    return -1


# Parse arguments
parser = argparse.ArgumentParser(description='Instrument C code with timely annotations')
parser.add_argument("file", help="Input C file")
parser.add_argument("-o", "--o", help="Output file (default: stdout)")
args = parser.parse_args()

c_file = args.file
out_file= args.o

print("Instrumenting file: " + c_file)

# Open the file
with open(c_file, 'r') as f:
    c_file_data = f.read()


# Instrument "@expires_afer=n[s|ms]"
print('\n------Instrument @expires_after=------')
expires_after_regex = '(\@expires_after=(\d*)(s|ms))(.*\n)\S*\s([a-zA-Z0-9_]*)(.*(\n|$))'
m = re.search(expires_after_regex, c_file_data)
while m is not None:
    print('@expires_after found at:', m.start(), m.end(), '>', m.group(0))

    text_replace = m.group(1)
    time_ms = int(m.group(2))
    time_unit = m.group(3)
    varname = m.group(5)

    # Convert to ms
    if time_unit == 's':
        time_ms = time_ms*1000

    #print('Time ms: ', time_ms)

    text_to_insert = 'expires_meta_t _' + varname + '_meta = {.expires_after_ms=' + str(time_ms) + '};'
    #print('Text to insert:', text_to_insert)
    c_file_data = c_file_data.replace(text_replace, text_to_insert)
    print('')
    m = re.search(expires_after_regex, c_file_data)

print('\n------AFTER @expires_after=------')
print(c_file_data)


# Instrument "@=" time assignment
print('\n------Intrument @=------')
timely_assign_regex = '\n*(\s*)([a-zA-Z0-9_]*).*(\@=).*(\n|$)'

m = re.search(timely_assign_regex, c_file_data)
while m is not None:
    print('@= found at:', m.start(), m.end(), '>', m.group(0))

    indent = m.group(1)

    varname = m.group(2)
    assign_replace = m.group(3)
    assign_start = int(m.start(1))
    assign_end = int(m.end())

    text_before = \
        indent + 'TIMELY_DISABLE_CHECKPOINTS();\n' + \
        indent + '_temperature_meta.expires = TIMELY_GET_FUTURE_TIME(_temperature_meta.expires_after_ms);\n'

    text_after = indent + 'TIMELY_ENABLE_CHECKPOINTS();\n'

    c_file_data = c_file_data[:assign_start] + text_before + \
            c_file_data[assign_start:assign_end] + text_after + \
            c_file_data[assign_end:]

    # Replace the assignment with a normal assignment
    c_file_data = c_file_data.replace(assign_replace, '=', 1)

    print('')
    #print(c_file_data)
    m = re.search(timely_assign_regex, c_file_data)

print('\n------AFTER @=------')
print(c_file_data)


# Instrument @expires block
print('\n------Intrument @expires------')
expires_regex = '\n*(\s*)(\@expires)\s*\(([a-zA-Z0-9_]*).*(\))\s*(\{)\n*(\s*)'

m = re.search(expires_regex, c_file_data)
while m is not None:
    print('@expires found at:', m.start(), m.end(), '>', m.group(0))

    indent = m.group(1)
    varname = m.group(3)
    metavar = '_' + varname + '_meta'
    indent_in_block = m.group(6)

    expires_kw_start = m.start(2)

    expires_expr_end = m.end(4)
    brace_start = m.start(5)
    block_start = m.end(5)

    text_before = \
        'TIMELY_CHECKPOINT();\n' + \
        indent + 'TIMELY_DISABLE_CHECKPOINTS();\n' + \
        indent + metavar + '.last_ms_remaining = TIMELY_MS_REMAINING(' + metavar + '.expires);\n' + \
        indent + 'if (' + metavar + '.last_ms_remaining)'

    text_start_block = \
        '\n' + indent_in_block + 'TIMELY_START_EXPIRE_TIMER_MS(' +  metavar + '.last_ms_remaining);'

    # Find the closing brace index
    print('Opening brace at index: ' + str(brace_start) + c_file_data[brace_start])
    brace_close_start = find_closing_brace(c_file_data, brace_start)
    print('Closing brace at index: ' + str(brace_close_start) + c_file_data[brace_close_start])
    block_end = brace_close_start + 1

    diff_indent = get_diff_indent(indent, indent_in_block)

    # Find if there is an '@catch' block
    catch_start = next_word_is(c_file_data, '@catch', brace_close_start + 1)
    if catch_start == -1:
        print('\'@expires\' has no \'@catch\' block')

        text_end_block = \
            diff_indent + 'TIMELY_STOP_EXPIRE_TIMER_MS();\n' + \
            indent

        text_after_block = \
            '\n' + indent + 'TIMELY_ENABLE_CHECKPOINTS();'

        c_file_data = c_file_data[:expires_kw_start] + text_before + \
            c_file_data[expires_expr_end:block_start] + text_start_block + \
            c_file_data[block_start:brace_close_start] + text_end_block + \
            c_file_data[brace_close_start:block_end] + text_after_block + \
            c_file_data[block_end:]

    else:
        print('\'@expires\' has \'@catch\' block')

        catch_end = catch_start + len('@catch')
        # Find opening brace start
        catch_brace_start = next_word_is(c_file_data, '{', catch_end)
        catch_brace_end = catch_brace_start + 1
        # Find the next non-whitespace
        catch_block_start = find_next_non_whitespace(c_file_data, catch_brace_end)

        text_end_block = \
            diff_indent + 'TIMELY_STOP_EXPIRE_TIMER_MS();\n' + \
            indent_in_block + 'TIMELY_ENABLE_CHECKPOINTS();\n' + \
            indent

        text_start_catch_block = \
            'TIMELY_ENABLE_CHECKPOINTS();\n' + \
            indent_in_block

        c_file_data = c_file_data[:expires_kw_start] + text_before + \
            c_file_data[expires_expr_end:block_start] + text_start_block + \
            c_file_data[block_start:brace_close_start] + text_end_block + \
            c_file_data[brace_close_start:catch_start] + 'else' + \
            c_file_data[catch_end:catch_block_start] + text_start_catch_block + \
            c_file_data[catch_block_start:]

    print('')
    m = re.search(expires_regex, c_file_data)

print('\n------AFTER @expires------')
print(c_file_data)


# Instrument @timely block
print('\n------Intrument @timely------')
expires_regex = '\n*(\s*)(\@timely)\s*\(([0-9]*)(s|ms).*(\))\s*(\{)\n*(\s*)'
timely_counter = 0

m = re.search(expires_regex, c_file_data)
while m is not None:
    print('@expires found at:', m.start(), m.end(), '>', m.group(0))

    indent = m.group(1)
    time_ms = int(m.group(3))
    time_unit = m.group(4)
    metavar = '_timely_' + str(timely_counter) + '_meta'
    indent_in_block = m.group(7)

    timely_kw_start = m.start(2)

    expires_expr_end = m.end(5)
    brace_start = m.start(6)
    block_start = m.end(6)

    # Convert to ms
    if time_unit == 's':
        time_ms = time_ms*1000

    text_before = \
        'expires_meta_t ' + metavar + ' = {.expires_after_ms = ' + str(time_ms) + '};\n' + \
        indent + 'TIMELY_CHECKPOINT();\n' + \
        indent + 'TIMELY_DISABLE_CHECKPOINTS();\n' + \
        indent + metavar + '.last_ms_remaining = TIMELY_MS_REMAINING(' + metavar + '.expires_after_ms);\n' + \
        indent + 'if (' + metavar + '.last_ms_remaining)'

    text_start_block = \
        '\n' + indent_in_block + 'TIMELY_START_EXPIRE_TIMER_MS(' +  metavar + '.last_ms_remaining);'

    # Find the closing brace index
    print('Opening brace at index: ' + str(brace_start) + c_file_data[brace_start])
    brace_close_start = find_closing_brace(c_file_data, brace_start)
    print('Closing brace at index: ' + str(brace_close_start) + c_file_data[brace_close_start])
    block_end = brace_close_start + 1

    diff_indent = get_diff_indent(indent, indent_in_block)

    # Find if there is an 'else' block
    else_start = next_word_is(c_file_data, 'else', brace_close_start + 1)
    if else_start == -1:
        print('\'@timely\' has no \'else\' block')

        text_end_block = \
            diff_indent + 'TIMELY_STOP_EXPIRE_TIMER_MS();\n' + \
            indent

        text_after_block = \
            '\n' + indent + 'TIMELY_ENABLE_CHECKPOINTS();'


        c_file_data = c_file_data[:timely_kw_start] + text_before + \
                c_file_data[expires_expr_end:block_start] + text_start_block + \
                c_file_data[block_start:brace_close_start] + text_end_block + \
                c_file_data[brace_close_start:block_end] + text_after_block + \
                c_file_data[block_end:]

    else:
        print('\'@timely\' has \'else\' block')

        else_end = else_start + len('else')
        # Find the opening brace start
        else_brace_start = next_word_is(c_file_data, '{', else_end)
        else_brace_end = else_brace_start + 1
        # Find the next non-whitespace
        else_block_start = find_next_non_whitespace(c_file_data, else_brace_end)

        text_end_block = \
            diff_indent + 'TIMELY_STOP_EXPIRE_TIMER_MS();\n' + \
            indent_in_block + 'TIMELY_ENABLE_CHECKPOINTS();\n' + \
            indent

        text_start_else_block = \
            'TIMELY_ENABLE_CHECKPOINTS();\n' + \
            indent_in_block

        c_file_data = c_file_data[:timely_kw_start] + text_before + \
            c_file_data[expires_expr_end:block_start] + text_start_block + \
            c_file_data[block_start:brace_close_start] + text_end_block + \
            c_file_data[brace_close_start:else_block_start] + text_start_else_block + \
            c_file_data[else_block_start:]


    print('')
    timely_counter = timely_counter + 1
    m = re.search(expires_regex, c_file_data)

print('\n------AFTER @timely------')
print(c_file_data)


# Prepend '#include "timely.h"
print('\n------Prepend timely header \'timely.h\'------')

# Prepend the header file
c_file_data = '#include "timely.h"\n' + c_file_data

print('\n------AFTER timely header------')
print(c_file_data)


# Write instrumented code to the output file if provided
if out_file is not None:
    print('\n------Writing output file: ' + out_file + '------')
    with open(out_file, 'w') as f:
        f.write(c_file_data)

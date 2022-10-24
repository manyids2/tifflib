% tifflib(1)
% manyids2
% Oct 2022

# NAME

tifflib - Manipulate tiffs with mpp.

# SYNOPSIS

**tifflib** [&lt;_options_&gt;]

# DESCRIPTION

Serve matched whole slide images and integer masks of different mpps.

# OPTIONS

## General Options

**-f** _&lt;[h|q|k|i]&gt;_, **-\-filepath**=_[h|q|k|i]_
: path to whole slide image

**-m** _&lt;[h|q|k|i]&gt;_, **-\-maskpath**=_[h|q|k|i]_
: path to integer mask


# RETURN VALUES

Exit code is

**0**
: On reading and displaying all images successfully.

**1**
: If any of the images could not be read or decoded or if there was no
image provided.

**2**
: If an invalid option or parameter was provided.

# ENVIRONMENT

**TIFFLIB_DEFAULT_TITLE**
: The default format string used for `--title`. If not given, the default
title format string is \"`%f`\".

# EXAMPLES

Some example invocations are put together at <https://github.com/manyids2/tifflib#examples>

# KNOWN ISSUES

...

# BUGS

Report bugs to <http://github.com/manyids2/tifflib/issues>

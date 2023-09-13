#ifndef T_PARSER_H_INCLUDED
#define T_PARSER_H_INCLUDED

#include "File.h"
#include "common.h"
#include "Theme.h"


/**
  Helps set theme default values based of user defined values in a text file
Parser syntax

# IDENTIFIER                        example ( color, font , image )
# ID                                example ( red  , arial, ballimage)
# VALUE or PATH                     example ( "255,255,255,255"  ,  "fonts/arial.ttf" ,   "images/ballimage.png"  )


some standard options
# COLOR > BG > VIEW > PRIMARY         stands for primary background color for views
# COLOR > BG > VIEW > SECONDARY       stands for alternative or secondary background color for views


# COLOR > FG > TEXT > PRIMARY         stands for primary foreground color for text
# COLOR > FG > TEXT > SECONDARY

# COLOR > FG > LINK > HOVER
# COLOR > FG > LINK > CLICK


N.B any line not starting with # is considered as a comment

A Basic Example of a typical script

# COLOR > BG > VIEW > PRIMARY
# ID  "white"
# VALUE "255,255,255,255"

# IMAGE > BG > VIEW > PRIMARY
# ID  "ball"
# PATH "images/ballimage.png"

# FONT > LINK > PRIMARY
# ID "arial"
# PATH "fonts/arial.ttf"

# FONT > TEXT > PRIMARY
# ID "consola"
# PATH "fonts/consola.ttf"


if no options are needed, then

# COLOR
# ID  "white"
# VALUE "255,255,255,255"

# FONT
# ID "arial"
# PATH "fonts/arial.ttf"

this is best if extra values are given but are not to be used as defaults, possibly to be called by the process at some point

*/




class theme_parser
{
    File* file;
    theme_parser(std::string path)
    {
        file = new File(path);
    }

    void link_colors()
    {

    }

    void link_images()
    {

    }

    void link_fonts()
    {

    }
};


#endif // PARSER_H_INCLUDED

#include <MonkVG/vgu.h>
#include "mkFont.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

struct BmpFntChar {
    int  id;                  // ascii code
    int  x, y, width, height; // position and size of the character in the image
    int  xoffset, yoffset;    // offset of the character from the cursor
    int  xadvance; // how much to advance the cursor after drawing the character
    int  page;     // which page the character is on
    int  chnl;     // which channel the character is on
    char letter;   // the character
};

VG_API_CALL VGFont VG_API_ENTRY vgCreateFontFromBmFnt(
    const char *bmp_fnt_text, VGImage bmp_fnt_image) VG_API_EXIT {
    // create an openvg font object
    VGFont font = vgCreateFont(0);
    if (font == VG_INVALID_HANDLE) {
        return VG_INVALID_HANDLE;
    }

    // parse the bmp fnt text
    std::istringstream                  iss(bmp_fnt_text);
    std::string                         line;
    std::unordered_map<int, BmpFntChar> bmp_fnt_chars;
    while (std::getline(iss, line)) {
        std::istringstream       iss_line(line);
        std::string              token;
        std::vector<std::string> tokens;
        // split the line into tokens
        while (std::getline(iss_line, token, ' ')) {
            tokens.push_back(token);
        }
        // parse the tokens

        // get the number of characters/glyphs
        // example: chars count=256
        if (tokens[0] == "chars") {
            std::stringstream ss(tokens[1]);
            std::string       key;
            std::string       value;
            std::getline(ss, key, '=');
            std::getline(ss, value, '=');
        }
        // example: char id=32 x=0 y=0 width=0 height=0 xoffset=0 yoffset=0
        // xadvance=21 page=0 chnl=15
        if (tokens[0] == "char") {
            BmpFntChar c;
            for (auto &t : tokens) {
                std::istringstream iss_token(t);
                std::string        key;
                std::string        value;
                std::getline(iss_token, key, '=');
                std::getline(iss_token, value, '=');
                if (key == "id") {
                    c.id     = std::stoi(value);
                    c.letter = (char)c.id;
                } else if (key == "x") {
                    c.x = std::stoi(value);
                } else if (key == "y") {
                    c.y = std::stoi(value);
                } else if (key == "width") {
                    c.width = std::stoi(value);
                } else if (key == "height") {
                    c.height = std::stoi(value);
                } else if (key == "xoffset") {
                    c.xoffset = std::stoi(value);
                } else if (key == "yoffset") {
                    c.yoffset = std::stoi(value);
                } else if (key == "xadvance") {
                    c.xadvance = std::stoi(value);
                } else if (key == "page") {
                    c.page = std::stoi(value);
                } else if (key == "chnl") {
                    c.chnl = std::stoi(value);
                }
            }
            bmp_fnt_chars[c.id] = c;

            // create a sub child image from the bitmap font image
            VGImage glyph_image =
                vgChildImage(bmp_fnt_image, c.x, c.y, c.width, c.height);

            // add the glyph to the font
            std::array<VGfloat, 2> glyph_origin = {VGfloat(c.x), VGfloat(c.y)};
            std::array<VGfloat, 2> escapement   = {VGfloat(c.xadvance), 0};
            vgSetGlyphToImage(font, c.id, glyph_image, glyph_origin.data(), escapement.data());

            // we can destroy the glyph image after we create the font because
            // vgSetGlyphToImage increments a reference count to the image
            vgDestroyImage(glyph_image);
        }
    }

    return font;
}

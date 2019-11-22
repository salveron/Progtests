#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;

/**
 * This ProgTest task is to implement a program that flips the given .img files with the given parameters.
 * */

#endif /* __PROGTEST__ */

/**
 * This class stores an image.
 *
 * @var c_endian - encoding of the image (big- or little-endian).
 * @var c_wi width of the image.
 * @var c_he height of the image.
 * @var c_format unigue format encoding, for example: 0x000c = 0b00001100 - 1 channel for pixel, a channel has 8 bits;
 *                                                    0x000e = 0b00001110 - 3 channel for pixel, a channel has 8 bits;
 *                                                    0x0013 = 0b00010011 - 4 channel for pixel, a channel has 16 bits.
 * @var pixels an array of pixels of the image stored like binaries (chars).
 * */
class Image{
public:
    unsigned int c_endian;
    unsigned int c_wi;
    unsigned int c_he;
    unsigned int c_format;

    vector <char> pixels;

    Image(unsigned int endian, unsigned int width, unsigned int height, unsigned int format){
        c_endian = endian;
        c_wi = width;
        c_he = height;
        c_format = format;
    }
    ~Image() = default;
};

/**
 * Reads metadata (header) from the file and saves them to the header variable. The header is then checked if it is valid.
 * */
bool readHeader(ifstream & inputFile,
                unsigned int & endian, unsigned int & wi, unsigned int & he, unsigned int & format,
                unsigned char * header);
/**
 * Reads pixels and saves them to the Image object.
 * */
bool readPixels(ifstream & inputFile, Image & img);
/**
 * Writes the output to the file. The image is already flipped.
 * */
bool writeToTheOutputFile(ofstream & outputFile, Image & img, unsigned char * header);

/**
 * Flips the image from srcFileName file vertically or horizontally and saves it to dstFileName file.
 * This is the main function.
 * */
bool flipImage (const char * srcFileName, const char * dstFileName, bool flipHorizontal, bool flipVertical){

    //opening and checking the input file
    ifstream inFile;
    inFile.open (srcFileName, ios::binary | ios::in);
    if (inFile.fail()) return false;

    unsigned char header[8];
    unsigned int endian, wi, he, format;

    //reading the header
    if (!readHeader(inFile, endian, wi, he, format, header)){
        inFile.close();
        return false;
    }

    if ((format >> 2) % 8 == 0) return false; //1-bit per channel (FOR NOW)

    //creating the image object
    Image img (endian, wi, he, format);

    //reading the pixels
    if (!readPixels(inFile, img)){
        inFile.close();
        return false;
    }

    inFile.close();

    //flipping the image
    if (flipHorizontal){
        unsigned int channels = (img.c_format % 4) + 1;
        unsigned int bpc = 1;//bytes per channel, DEFAULT = 1 BYTE = 8 BIT
        if ((img.c_format >> 2) % 8 == 4) bpc = 2; //2 BYTES = 16 BIT

        unsigned int index_1;
        unsigned int index_2;

        for (unsigned int i = 0; i < img.c_he; i++){
            for (unsigned int j = 0; j < img.c_wi / 2; j++){
                for (unsigned int k = 0; k < channels; k++) {
                    for (unsigned int l = 0; l < bpc; l++) {
                        index_1 = i * img.c_wi * channels * bpc + j * channels * bpc + k * bpc + l;
                        index_2 = i * img.c_wi * channels * bpc + (img.c_wi - j - 1) * channels * bpc + k * bpc + l;

                        char tmp = img.pixels[index_1];
                        img.pixels[index_1] = img.pixels[index_2];
                        img.pixels[index_2] = tmp;
                    }
                }
            }
        }
    }
    if (flipVertical){
        unsigned int channels = (img.c_format % 4) + 1;
        unsigned int bpc = 1;//bytes per channel, DEFAULT = 1 BYTE = 8 BIT
        if ((img.c_format >> 2) % 8 == 4) bpc = 2; //2 BYTES = 16 BIT

        unsigned int index_1;
        unsigned int index_2;

        for (unsigned int i = 0; i < img.c_he / 2; i++){
            for (unsigned int j = 0; j < img.c_wi; j++){
                for (unsigned int k = 0; k < channels; k++){
                    for (unsigned int l = 0; l < bpc; l++) {
                        index_1 = i * img.c_wi * channels * bpc + j * channels * bpc + k * bpc + l;
                        index_2 = (img.c_he - i - 1) * img.c_wi * channels * bpc + j * channels * bpc + k * bpc + l;

                        char tmp = img.pixels[index_1];
                        img.pixels[index_1] = img.pixels[index_2];
                        img.pixels[index_2] = tmp;
                    }
                }
            }
        }
    }

    //opening the output file
    ofstream outFile (dstFileName, ios::binary | ios::out | ios::trunc);
    if (outFile.fail()) return false;

    //writing to the output file
    if (!writeToTheOutputFile(outFile, img, header)){
        outFile.close();
        return false;
    }

    outFile.close();
    return true;
}

bool readHeader(ifstream & inputFile,
                unsigned int & endian,
                unsigned int & wi,
                unsigned int & he,
                unsigned int & format,
                unsigned char * header){

    //reading the header
    inputFile.read((char *)header, 8);
    if (inputFile.fail()) return false;

    //setting header variables
    if (header[0] == (char)0x49 && header[1] == (char)0x49){ //little-endian
        endian = 0x4949;
        wi = ((uint8_t)header[3] << 8 ) + (uint8_t)header[2];
        he = ((uint8_t)header[5] << 8 ) + (uint8_t)header[4];
        format = ((uint8_t)header[7] << 8) + (uint8_t)header[6];
    }
    else if (header[0] == (char)0x4d && header[1] == (char)0x4d){ //big-endian
        endian = 0x4d4d;
        wi = ((uint8_t)header[2] << 8 ) + (uint8_t)header[3];
        he = ((uint8_t)header[4] << 8 ) + (uint8_t)header[5];
        format = ((uint8_t)header[6] << 8) + (uint8_t)header[7];
    }
    else return false; //bad endian

    //checking width and height
    if (wi <= 0 || he <= 0) return false;

    //checking the format
    if (format <= 0
        || format % 4 == 1                                                                 //bad number of channels
        || ((format >> 2) % 8 != 0 && (format >> 2) % 8 != 3 && (format >> 2) % 8 != 4)    //bad number of bits per channel
        || format >> 5 != 0) return false;

    return true;
}

bool readPixels(ifstream & inputFile, Image & img){
    unsigned int i = 0;
    char tmp;

    //reading the pixels
    do{
        inputFile.get(tmp);
        img.pixels.push_back(tmp);
        i++;
    } while (inputFile.good());

    int channels = (img.c_format % 4) + 1;
    int bpc = 1;//DEFAULT, 1 byte per channel
    if ((img.c_format >> 2) % 8 == 4) bpc = 2;//16 bits per channel = 2 bytes

    //checking for errors
    if (!inputFile.eof()) return false;

    return i - 1 == img.c_he * img.c_wi * channels * bpc;
}

bool writeToTheOutputFile(ofstream & outputFile, Image & img, unsigned char * header){

    for (int m = 0; m < 8; m++)
        outputFile << header[m];

    unsigned int channels = (img.c_format % 4) + 1;
    unsigned int bpc = 1;//bytes per channel, DEFAULT = 1 BYTE = 8 BIT
    if ((img.c_format >> 2) % 8 == 4) bpc = 2; //2 BYTES = 16 BIT

    //writing the pixels
    for (unsigned int i = 0; i < img.c_he; i++){
        for (unsigned int j = 0; j < img.c_wi; j++){
            for (unsigned int k = 0; k < channels; k++){
                for (unsigned int l = 0; l < bpc; l++){
                    outputFile << img.pixels[i * img.c_wi * channels * bpc + j * channels * bpc + k * bpc + l];
                    if (outputFile.fail()) return false;
                }
            }
        }
    }

    return true;
}

#ifndef __PROGTEST__
bool identicalFiles (const char * fileName1, const char * fileName2){
    ifstream file1 (fileName1, ios::in | ios::binary);
    ifstream file2 (fileName2, ios::in | ios::binary);
    char a, b;
    do{
        file1.get(a);
        file2.get(b);
    } while (a == b && !file1.fail() && !file2.fail());
    if ( !file1.eof() || !file2.eof() ){
        file1.close();
        file2.close();
        return false;
    }
    file1.close();
    file2.close();
    return true;
}

int main(){
    assert ( flipImage ( "tests/input_00.img", "tests/output_00.img", true, false )
             && identicalFiles ( "tests/output_00.img", "tests/ref_00.img" ) );

    assert ( flipImage ( "tests/input_01.img", "tests/output_01.img", false, true )
             && identicalFiles ( "tests/output_01.img", "tests/ref_01.img" ) );

    assert ( flipImage ( "tests/input_02.img", "tests/output_02.img", true, true )
             && identicalFiles ( "tests/output_02.img", "tests/ref_02.img" ) );

    assert ( flipImage ( "tests/input_03.img", "tests/output_03.img", false, false )
             && identicalFiles ( "tests/output_03.img", "tests/ref_03.img" ) );

    assert ( flipImage ( "tests/input_04.img", "tests/output_04.img", true, false )
             && identicalFiles ( "tests/output_04.img", "tests/ref_04.img" ) );

    assert ( flipImage ( "tests/input_05.img", "tests/output_05.img", true, true )
             && identicalFiles ( "tests/output_05.img", "tests/ref_05.img" ) );

    assert ( flipImage ( "tests/input_06.img", "tests/output_06.img", false, true )
             && identicalFiles ( "tests/output_06.img", "tests/ref_06.img" ) );

    assert ( flipImage ( "tests/input_07.img", "tests/output_07.img", true, false )
             && identicalFiles ( "tests/output_07.img", "tests/ref_07.img" ) );

    assert ( flipImage ( "tests/input_08.img", "tests/output_08.img", true, true )
             && identicalFiles ( "tests/output_08.img", "tests/ref_08.img" ) );

    assert ( ! flipImage ( "tests/input_09.img", "tests/output_09.img", true, false ) );

    // extra inputs (optional & bonus tests)
    assert ( flipImage ( "tests/extra_input_00.img", "tests/extra_out_00.img", true, false )
             && identicalFiles ( "tests/extra_out_00.img", "tests/extra_ref_00.img" ) );
    assert ( flipImage ( "tests/extra_input_01.img", "tests/extra_out_01.img", false, true )
             && identicalFiles ( "tests/extra_out_01.img", "tests/extra_ref_01.img" ) );
    assert ( flipImage ( "tests/extra_input_02.img", "tests/extra_out_02.img", true, false )
             && identicalFiles ( "tests/extra_out_02.img", "tests/extra_ref_02.img" ) );
    assert ( flipImage ( "tests/extra_input_03.img", "tests/extra_out_03.img", false, true )
             && identicalFiles ( "tests/extra_out_03.img", "tests/extra_ref_03.img" ) );
    assert ( flipImage ( "tests/extra_input_04.img", "tests/extra_out_04.img", true, false )
             && identicalFiles ( "tests/extra_out_04.img", "tests/extra_ref_04.img" ) );
    assert ( flipImage ( "tests/extra_input_05.img", "tests/extra_out_05.img", false, true )
             && identicalFiles ( "tests/extra_out_05.img", "tests/extra_ref_05.img" ) );
    assert ( flipImage ( "tests/extra_input_06.img", "tests/extra_out_06.img", true, false )
             && identicalFiles ( "tests/extra_out_06.img", "tests/extra_ref_06.img" ) );
    assert ( flipImage ( "tests/extra_input_07.img", "tests/extra_out_07.img", false, true )
             && identicalFiles ( "tests/extra_out_07.img", "tests/extra_ref_07.img" ) );

    return 0;
}
#endif /* __PROGTEST__ */

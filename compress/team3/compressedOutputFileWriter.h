//
//  compressedOutputFileWriter.h
//  huff
//
//  Created by Adam Bradford on 2/2/14.
//  Copyright (c) 2014 Adam Bradford. All rights reserved.
//

#ifndef huff_compressedOutputFileWriter_h
#define huff_compressedOutputFileWriter_h
#include "encoder.h"

//writes an encoded version of the nonCompressedfile with the given filename.
void writeNonCompressedFileToCompressedOutput(FILE* nonCompressedFile, FILE* compressedFile, huffResult* resultArray);

#endif

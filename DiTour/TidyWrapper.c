//
//  TidyWrapper.c
//  DiTour
//
//  Created by Pelaia II, Tom on 3/2/15.
//  Copyright (c) 2015 UT-Battelle ORNL. All rights reserved.
//

#include "TidyWrapper.h"

#include <stdio.h>
#include <errno.h>

// the project (see the build settings) adds the Mac's /usr/include/tidy to the header search path to pickup
// these two tidy headers which are included in the iOS Simulator SDK but not the iOS iPhone SDK
#include <tidy.h>
#include <buffio.h>


/*
 Convert the raw HTML to proper XHTML using the tidy library.
 Returns a pointer to the XHTML buffer which the caller owns and is responsible for freeing.
 */
char *ConvertC_HTML_TO_XHTML(const char *input) {
	if ( input == NULL || strlen(input) == 0 )  return NULL;	// no content to process so no valid XHTML possible

	// create the tidy document and output buffer
	TidyDoc tidyDocument = tidyCreate();
	TidyBuffer outputBuffer = {0};

	// return code for tidy calls
	int returnCode = -1;

	// configure the tidy options: generate XHTML, quiet output, supress warnings
	short success = tidyOptSetBool( tidyDocument, TidyXhtmlOut, yes ) == 1;					// generate XHTML
	if ( success )  success = tidyOptSetBool( tidyDocument, TidyQuiet, yes ) == 1;			// quiet output
	if ( success )  success = tidyOptSetBool( tidyDocument, TidyShowWarnings, no ) == 1;	// suppress warnings

	// parse the input
	if ( success )  returnCode = tidyParseString( tidyDocument, input );

	// attempt to cleanup and repair HTML
	if ( returnCode >= 0 )  returnCode = tidyCleanAndRepair( tidyDocument );

	// print diagnostic messages
	if ( returnCode >= 0 )  returnCode = tidyRunDiagnostics( tidyDocument );

	// save the document to the output buffer
	if ( returnCode >= 0 )  returnCode = tidySaveBuffer( tidyDocument, &outputBuffer );

	success = success && returnCode >= 0;

	if ( !success ) {
		printf( "Tidy error with return code: %d\n", returnCode );
	}

	// output XHTML plus null termination character
	char *outputXHTML = NULL;
	if ( success && outputBuffer.size > 0 ) {
		// need to be careful as the output buffer is not null terminated and so we must make sure to only copy the characters specified by the output size
		outputXHTML = (char *)malloc( outputBuffer.size + 1 );		// this pointer must be freed by the caller
		memcpy( outputXHTML, outputBuffer.bp, outputBuffer.size );
		outputXHTML[outputBuffer.size] = NULL;		// NULL termination
	}

	// free tidy allocated memory
	tidyBufFree( &outputBuffer );
	tidyRelease( tidyDocument );

	return outputXHTML;
}




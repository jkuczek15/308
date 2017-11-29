/* fat12ls.c 
* 
*  Displays the files in the root sector of an MSDOS floppy disk
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define SIZE 32             /* size of the read buffer */
#define ROOTSIZE 256        /* max size of the root directory */
#define NAME_OFFSET 0x00    /* name offset */
#define ATTR_OFFSET 0x0b    /* attributes offset */
#define TIME_OFFSET 0x16    /* time offset */
#define DATE_OFFSET 0x18    /* date offset*/
#define SIZE_OFFSET 0x1c    /* size offset */

struct BootSector
{
    unsigned char  sName[9];            // The name of the volume
    unsigned short iBytesSector;        // Bytes per Sector
    
    unsigned char  iSectorsCluster;     // Sectors per Cluster
    unsigned short iReservedSectors;    // Reserved sectors
    unsigned char  iNumberFATs;         // Number of FATs
    
    unsigned short iRootEntries;        // Number of Root Directory entries
    unsigned short iLogicalSectors;     // Number of logical sectors
    unsigned char  xMediumDescriptor;   // Medium descriptor
    
    unsigned short iSectorsFAT;         // Sectors per FAT
    unsigned short iSectorsTrack;       // Sectors per Track
    unsigned short iHeads;              // Number of heads
    
    unsigned short iHiddenSectors;      // Number of hidden sectors
};

void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[]);
//  Pre: Calculated directory offset and number of directory entries
// Post: Prints the filename, time, date, attributes and size of each entry

unsigned short endianSwap(unsigned char one, unsigned char two);
//  Pre: Two initialized characters
// Post: The characters are swapped (two, one) and returned as a short

void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[]);
//  Pre: An initialized BootSector struct and a pointer to an array
//       of characters read from a BootSector
// Post: The BootSector struct is filled out from the buffer data

char * toDOSName(char strings[], unsigned char buffer[], int offset);
//  Pre: String is initialized with at least 12 characters, buffer contains
//       the directory array, offset points to the location of the filename
// Post: fills and returns a string containing the filename in 8.3 format

char * parseAttributes(char string[], unsigned char key);
//  Pre: String is initialized with at least five characters, key contains
//       the byte containing the attribue from the directory buffer
// Post: fills the string with the attributes

char * parseTime(char string[], unsigned short usTime);
//  Pre: string is initialzied for at least 9 characters, usTime contains
//       the 16 bits used to store time
// Post: string contains the formatted time

char * parseDate(char string[], unsigned short usDate);
//  Pre: string is initialized for at least 13 characters, usDate contains
//       the 16 bits used to store the date
// Post: string contains the formatted date

int roundup512(int number);
// Pre: initialized integer
// Post: number rounded up to next increment of 512

char fromBinary(const char *s);

// reads the boot sector and root directory
int main(int argc, char * argv[])
{
    int pBootSector = 0;
    unsigned char buffer[SIZE];
    unsigned char rootBuffer[ROOTSIZE * 32];
    struct BootSector sector;
    int iRDOffset = 0;
    
    // Check for argument
    if (argc < 2) {
    	printf("Specify boot sector\n");
    	exit(1);
    }
    
    // Open the file and read the boot sector
    pBootSector = open(argv[1], O_RDONLY);
    read(pBootSector, buffer, SIZE);
    
    // Decode the boot Sector
    decodeBootSector(&sector, buffer);
    
    // Calculate the location of the root directory
    iRDOffset = (1 + (sector.iSectorsFAT * sector.iNumberFATs) )
                 * sector.iBytesSector;
    
    // Read the root directory into buffer
    lseek(pBootSector, iRDOffset, SEEK_SET);
    read(pBootSector, rootBuffer, ROOTSIZE);
    close(pBootSector);
    
    // Parse the root directory
    parseDirectory(iRDOffset, sector.iRootEntries, rootBuffer);
} // end main

// Converts two characters to an unsigned short with two, one
unsigned short endianSwap(unsigned char one, unsigned char two)
{
    return (short) (((unsigned int) one) | (((unsigned int) two) << 8));
}// end endianSwap()

// Fills out the BootSector Struct from the buffer
void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[])
{
	int i; 	

	// Pull the name and put it in the struct
	for(i = 3; i < 11; i++){
	  pBootS->sName[i-3] = buffer[i];
	}// end for loop pulling in name
	
	// Add the null terminator
	pBootS->sName[8] = '\0';
		
	// Read bytes/sector and convert to big endian
	pBootS->iBytesSector = endianSwap(buffer[11], buffer[12]);
	
	// Read sectors/cluster, Reserved sectors and Number of Fats
	pBootS->iSectorsCluster = buffer[13];	
  	pBootS->iReservedSectors = endianSwap(buffer[14], buffer[15]);
	pBootS->iNumberFATs = buffer[16]; 
	
	// Read root entries, logicical sectors and medium descriptor
	pBootS->iRootEntries = endianSwap(buffer[17], buffer[18]);
	pBootS->iLogicalSectors = endianSwap(buffer[19], buffer[20]);
	pBootS->xMediumDescriptor = buffer[21];
	
	// Read and covert sectors/fat, sectors/track, and number of heads
	pBootS->iSectorsFAT = endianSwap(buffer[22], buffer[23]);
	pBootS->iSectorsTrack = endianSwap(buffer[24], buffer[25]);
	pBootS->iHeads = endianSwap(buffer[26], buffer[27]);
	
	// Read hidden sectors
	pBootS->iHiddenSectors = endianSwap(buffer[28], buffer[29]);

	return;
}// end decodeBootSector()

// iterates through the directory to display filename, time, date,
// attributes and size of each directory entry to the console
void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[])
{
    int i = 0;
    int offset;
    char string[13];
    
    // Display table header with labels
    printf("Filename\tAttrib\tTime\t\tDate\t\tSize\n");

    // loop through directory entries to print information for each
    for(i = 0; i < (iEntries); i = i + 32)   {
        // for loop over all dictionary entries            
    	if (buffer[i] != 0x00 && buffer[i] != 0xe5 ) {                                                                                                                                                                                   
            // Display filename
            memset(string, 0, 13);                                                                                                                                                                                                                                                                                                                                              strcpy(string, "16SEC.TXT");
    		printf("%s\t", toDOSName(string, buffer, NAME_OFFSET + i)  );                                                                                                                                                                                                                                                                                                       
            // Display Attributes
            printf("%s\t", parseAttributes(buffer, ATTR_OFFSET + i)  );
            // Display Time
            memset(string, 0, 13);
    		printf("%s\t", parseTime(string, endianSwap(TIME_OFFSET+i, TIME_OFFSET+i+1) )  );
            // Display Date
            memset(string, 0, 13);
    		printf("%s\t", parseDate(string, endianSwap(DATE_OFFSET+i, DATE_OFFSET+i+1))  );
            // Display Size
            memset(string, 0, 13);
    		printf("%d\n", SIZE_OFFSET + i);
    	}// end if this is an invalid file
    }// end for loop over dictionary entries
    
    // Display key
    printf("(R)ead Only (H)idden (S)ystem (A)rchive\n");
} // end parseDirectory()

// Parses the attributes bits of a file
char * parseAttributes(char string[], unsigned char key)
{
    memset(string, 0, 13);   
    int i = 0;
    
    if (key & 0x00){
		string[i] = 'R';
		i++;
    }// end if 0th bit set

    if (key & 0x01){
		string[i] = 'H';
		i++;
    }// end if 1st bit set

	if (key & 0x02){
        // system attribute
		string[i] = 'S';
		i++;
	}// end if 2nd bit set
   
	if (key & 0x05){
         // archive attribute
		string[i] = 'A';
		i++;
    }// end if 
    
	return string;
} // end parseAttributes()

// Decodes the bits assigned to the time of each file
char * parseTime(char string[], unsigned short usTime)
{
    unsigned char hour = 0x00, min = 0x00, sec = 0x00;
    int i;
    int count = 0;
    char bits[7];
    bits[6] = '\0';

    for(i = 0; i < 16; i++){
        int bitVal = (usTime & ( 1 << i )) >> i;
        bits[count] = bitVal;
        sprintf(bits, "%d%s", bitVal, bits);

        if(i == 4){
            count = 0;
            hour = fromBinary(bits);
            bits[0] = 0;
            bits[1] = 0;
            bits[2] = 0;
            bits[3] = 0;
            bits[4] = 0;
            bits[5] = 0;
        }// end if 5th bit

        if(i == 10){
            count = 0;
            min = fromBinary(bits);
            bits[0] = 0;
            bits[1] = 0;
            bits[2] = 0;
            bits[3] = 0;
            bits[4] = 0;
            bits[5] = 0;
        }// end if 11th bit

        if(i == 15){
            sec = fromBinary(bits);
        }// end if 16th bit (last)

        count++;
    }// end for loop over all bits

    sprintf(string, "%02i:%02i:%02i", hour, min, sec);
    
    return string;
} // end parseTime()


// Decodes the bits assigned to the date of each file
char * parseDate(char string[], unsigned short usDate)
{
    unsigned short year = 0x0000;
    unsigned char month = 0x00, day = 0x00;
   
    // parse the year
    year = 1980 + ((usDate & 0xFE00) >> 9);

    // parse the month
    month = ((usDate & 0x01E0) >> 5);

    // parse the day
    day = (usDate & 0x000F);

    sprintf(string, "%d/%d/%d", year, month, day);

    return string;   
} // end parseDate()

// Formats a filename string as DOS (adds the dot to 8-dot-3)
char * toDOSName(char string[], unsigned char buffer[], int offset)
{
    int i, j;

    for(i = offset; i < offset + 8; i++) {
        // for loop over first 8 bytes
        if(buffer[i] == 0x20){
            string[i] = '.';
            j = i + 1;
            break;
        }else{
            string[i] = buffer[i];
        }// end if we've reached the end of this shit
    }// end for loop over first eight bytes
      
    for(i = offset + 8; i < offset + 13; i++) {
        if(buffer[i] == 0x20){
            break;
        }// end if we've reached a space
        string[j++] = buffer[i];
    }// end for loop over extension three bytes

    string[j] = '\0';
    return string;
} // end toDosNameRead-Only Bit

char fromBinary(const char *s) 
{
    return strtol(s, NULL, 2);
}// end fromBinary()

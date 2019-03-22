
/*
*
*	The Kernel
*
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "kernel.h"
#include "hal/hal.h"
#include "string.h"

#include "drivers/stdio/emb-stdio.h"			// Needed for printf
#include "drivers/sdcard/SDCard.h"

void kernel_init(void);
void input_output_init(void);
void sys_info( uint8_t* );
void sd_card_fs_demo();
void print (uint8_t* );
void print_file_contents(char *);
void DisplayDirectory(const char*);

char buffer[500];


/*
 *		Kernel's entry point
**/
void main(uint32_t r0, uint32_t r1, uint32_t atags){

  //Init
  kernel_init();
  input_output_init();
  printf_serial("\n\n");
  sdInitCard (&printf_serial, &printf_serial, true);\

  //Welcome Msg Video
  hal_io_video_puts( "\n\r\n\rWelcome to MiniOS Pi Zero\n\r", 3, VIDEO_COLOR_GREEN );
  hal_io_serial_puts( SerialA, "\n\r\n\rWelcome to MiniOS Pi Zero\n\r" );
  hal_io_video_puts( "\n\r$ ", 2, VIDEO_COLOR_GREEN );
  hal_io_serial_puts( SerialA, "\n\r$ " );

  uint8_t c;
  char cArr[100];
  char* arr;

  int index= 0;
  char* currentPath = "\\*.*";
  int wordCount = 0;
  char* wordArr[10];

	while (1){
    c = hal_io_serial_getc( SerialA );

    if (c =='\r') {
      printf_serial("\n");
      printf_video("\n\r");

      char *tok = strtok(cArr, " ");

        if (strncmp(tok, "ls", 2) == 0)
        { 
          printf_serial("\n\nDirectory (/): \n");
          DisplayDirectory(currentPath);
        }
        else if (strncmp(tok, "cd", 2) == 0)
        { 
          char *arg = strtok(NULL, " ");
          printf_serial(arg);
          
        }
        else if (strncmp(tok, "cat", 3) == 0)
        {
          char *arg = strtok(NULL, " ");
          print_file_contents(arg);
          printf_serial("\n");
          printf_video("\n\r");
        }
        else if (strncmp(tok, "sysinfo", 7) == 0)
        { 
          print("System Info: This is RPI2\n"); 
        }

      memset(cArr, 0, 100);
      index = 0;
      wordCount = 0;
      
    } 
     else {
      cArr[index++] = c;
      printf_video( "%c", c );  //<<--- We also have printfs
      printf_serial( "%c", c );
    }
  
  }

}

/*
* Initializes the kernel
*/
void kernel_init(void){

  hal_io_init();
  //console_init();
  //system_calls_init();
  //scheduler_init();
  //faults_init();

}

/*
* Initializes All IO
*/
void input_output_init(void){
  uint32_t video_init_res = HAL_FAILED;

#ifdef VIDEO_PRESENT
  video_init_res = hal_io_video_init();
#endif

#ifdef SERIAL_PRESENT
  hal_io_serial_init();
#endif
//NOTE: PAST THIS POINT WE CAN USE printf
//      (printf needs both serial and video inits to be called first)

  if ( video_init_res == HAL_SUCCESS )
    sys_info( "Video Initialized\n\r" );
  else
    sys_info( "Video Init Failed [x]\n\r" );

    sys_info( "Serial Initialized\n\r" );
}

void sys_info( uint8_t* msg ){
  printf_video( msg );
  printf_serial( msg );
}

void print (uint8_t* msg) {
   printf_video( msg );
  printf_serial( msg );
}

/////////////////////////////////////////////////////////////////
////////////////    D E M O    C O D E    ///////////////////////
/////////////////////////////////////////////////////////////////

void print_file_contents(char *filename)
{
  printf_serial("\nOpening: ");
  printf_video("\n\rOpening: ");

  printf_serial(filename);
  printf_video(filename);
  printf_serial("\n");
  printf_video("\n\r");
  HANDLE fHandle = sdCreateFile(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (fHandle != 0)
  {
    uint32_t bytesRead;

    if ((sdReadFile(fHandle, &buffer[0], 500, &bytesRead, 0) == true))
    {
      buffer[bytesRead - 1] = '\0'; ///insert null char
      printf_serial("File Contents: %s\n", &buffer[0]);
      printf_video("File Contents: %s\n\r", &buffer[0]);
    }
    else
    {
      printf_serial("Failed to read");
      printf_video("Failed to read");
    }
    // Close the file
    sdCloseHandle(fHandle);
  }
}

void sd_card_fs_demo(){
  
  /* Display root directory */
  printf_serial("\n\nDirectory (/): \n");
  DisplayDirectory("\\*.*");

  printf_serial("\n");
  printf_serial("Opening Alice.txt \n");

  HANDLE fHandle = sdCreateFile("Alice.txt", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (fHandle != 0) {
    uint32_t bytesRead;

    if ((sdReadFile(fHandle, &buffer[0], 500, &bytesRead, 0) == true))  {
        buffer[bytesRead-1] = '\0';  ///insert null char
        printf_serial("File Contents: %s", &buffer[0]);
    }
    else{
      printf_serial("Failed to read" );
    }

    // Close the file
    sdCloseHandle(fHandle);

  }

}

void DisplayDirectory(const char* dirName) {
	HANDLE fh;
	FIND_DATA find;
	char* month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	fh = sdFindFirstFile(dirName, &find);							// Find first file
	do {
		if (find.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
      printf_serial("%s <DIR>\n", find.cFileName);
      printf_video("%s <DIR>\n\r", find.cFileName);
    }
    else {
      printf_serial("%c%c%c%c%c%c%c%c.%c%c%c Size: %9lu bytes, %2d/%s/%4d, LFN: %s\n",
			find.cAlternateFileName[0], find.cAlternateFileName[1],
			find.cAlternateFileName[2], find.cAlternateFileName[3],
			find.cAlternateFileName[4], find.cAlternateFileName[5],
			find.cAlternateFileName[6], find.cAlternateFileName[7],
			find.cAlternateFileName[8], find.cAlternateFileName[9],
			find.cAlternateFileName[10],
			(unsigned long)find.nFileSizeLow,
			find.CreateDT.tm_mday, month[find.CreateDT.tm_mon],
			find.CreateDT.tm_year + 1900,
			find.cFileName);				
      printf_video("%c%c%c%c%c%c%c%c.%c%c%c Size: %9lu bytes, %2d/%s/%4d, LFN: %s\n\r",
			find.cAlternateFileName[0], find.cAlternateFileName[1],
			find.cAlternateFileName[2], find.cAlternateFileName[3],
			find.cAlternateFileName[4], find.cAlternateFileName[5],
			find.cAlternateFileName[6], find.cAlternateFileName[7],
			find.cAlternateFileName[8], find.cAlternateFileName[9],
			find.cAlternateFileName[10],
			(unsigned long)find.nFileSizeLow,
			find.CreateDT.tm_mday, month[find.CreateDT.tm_mon],
			find.CreateDT.tm_year + 1900,
			find.cFileName);		
    }					// Display each entry
	} while (sdFindNextFile(fh, &find) != 0);						// Loop finding next file
	sdFindClose(fh);												// Close the serach handle
}

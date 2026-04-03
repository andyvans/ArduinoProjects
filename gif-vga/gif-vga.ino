#include <fabgl.h> // Include FabGL library for VGA rendering
#include <AnimatedGIF.h> // Include AnimatedGIF library

// GIF files
#include "hud_a.h" // GIF size in FLASH memory is 1.7MB

fabgl::VGAController DisplayController; // FabGL VGA controller object

// GIF to display
#define GifData death_star // Change image to display (image name in gif_files\[image header file].h)

void setup()
{
  Serial.begin(115200);

  // Initialize FabGL VGA controller
  DisplayController.begin();
  DisplayController.setResolution(VGA_320x240_60Hz); // Set resolution to 320x240
  DisplayController.setPaletteMode(fabgl::PaletteMode::RGB565); // Use RGB565 palette mode

  AnimatedGIF *gif;
  gif = openGif((uint8_t *)GifData, sizeof(GifData));
  if (gif == NULL)
  {
    Serial.println("Cannot open GIF");
    while (true)
    {
      // No need to continue
    }
  }

  while (true)
  {
    gif->playFrame(false, NULL);
  }
}

void loop()
{
  delay(1);
}

// Open Gif and allocate memory
AnimatedGIF *openGif(uint8_t *gifdata, size_t gifsize)
{
  AnimatedGIF *gif;
  gif = (AnimatedGIF *)malloc(sizeof(AnimatedGIF));
  if (gif == NULL)
  {
    Serial.println("Not RAM Enough memory for GIF structure");
    return NULL;
  }

  gif->begin(GIF_PALETTE_RGB565_BE); // Set the cooked output type we want (compatible with VGA)

  if (gif->open(gifdata, gifsize, GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif->getCanvasWidth(), gif->getCanvasHeight());
    Serial.printf("GIF memory size is %ld (%2.2f MB)", gifsize, (float)gifsize / (1024 * 1024));
    gif->setDrawType(GIF_DRAW_COOKED); // We want the Animated GIF library to generate ready-made pixels
    if (gif->allocFrameBuf(GIFAlloc) != GIF_SUCCESS)
    {
      Serial.println("Not Enough RAM memory for frame buffer");
      return NULL;
    }
    return gif;
  }
  else
  {
    printGifErrorMessage(gif->getLastError());
    return NULL;
  }
}

// Memory allocation callback function
void *GIFAlloc(uint32_t u32Size)
{
  return malloc(u32Size);
}

// Memory free callback function
void GIFFree(void *p)
{
  free(p);
}

// Draw callback from the AnimatedGIF decoder
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0)
  { // Set the memory window (once per frame) when the first line is rendered
    DisplayController.setWindow(pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // Push the pixels to the VGA display
  DisplayController.writePixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
}

// Get human-readable error related to GIF
void printGifErrorMessage(int errorCode)
{
  switch (errorCode)
  {
  case GIF_DECODE_ERROR:
    Serial.println("GIF Decoding Error");
    break;
  case GIF_TOO_WIDE:
    Serial.println("GIF Too Wide");
    break;
  case GIF_INVALID_PARAMETER:
    Serial.println("Invalid Parameter for gif open");
    break;
  case GIF_UNSUPPORTED_FEATURE:
    Serial.println("Unsupported feature in GIF");
    break;
  case GIF_FILE_NOT_OPEN:
    Serial.println("GIF File not open");
    break;
  case GIF_EARLY_EOF:
    Serial.println("GIF early end of file");
    break;
  case GIF_EMPTY_FRAME:
    Serial.println("GIF with empty frame");
    break;
  case GIF_BAD_FILE:
    Serial.println("GIF bad file");
    break;
  case GIF_ERROR_MEMORY:
    Serial.println("GIF memory Error");
    break;
  default:
    Serial.println("Unknown Error");
    break;
  }
}